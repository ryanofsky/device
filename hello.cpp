#include <iostream>
#include <cassert>

#include <boost/static_assert.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/int_c.hpp>
#include <boost/mpl/arithmetic/plus.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/range_c.hpp>

using namespace std;
namespace mpl = boost::mpl;

#include "util.hpp"
#include "variable.hpp"
#include "register.hpp"
#include "io.hpp"
#include "debug.hpp"

int NoIO::nextval = 0;

template<typename REGISTER, int FIRSTBIT, int LASTBIT>
struct AtRegister
{
  typedef REGISTER Register;
  enum { FirstBit = FIRSTBIT , LastBit = LASTBIT };
  enum { Width = LASTBIT - FIRSTBIT + 1 };
  enum { value = Width };
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Register::Bits > LastBit);
    BOOST_STATIC_ASSERT(Register::Bits > FirstBit);
    BOOST_STATIC_ASSERT(LastBit >= FirstBit);
  }  
};


template<typename DEVICE, typename INTEGER>
struct reader
{
  DEVICE & device;
  INTEGER & value;

  reader(DEVICE & device_, INTEGER & value_)
  : device(device_), value(value_) {}
  
  template<typename RegInfo, typename Position>
  void operator()()
  {
    RegInfo::verify();
    RegInfo::Register & reg = device.reg<RegInfo::Register>();
    INTEGER result;
    reg.get(result, RegInfo::FirstBit, RegInfo::Width);
    value |= result << Position::value;
    return;
  }
};

template<typename DEVICE, typename INTEGER>
struct writer
{
  DEVICE & device;
  INTEGER & value;

  writer(DEVICE & device_, INTEGER & value_)
  : device(device_), value(value_) {}
  
  template<typename RegInfo, typename Position>
  void operator()()
  {
    RegInfo::verify();
    RegInfo::Register & reg = device.reg<RegInfo::Register>();
    reg.set(value >> Position::value, RegInfo::FirstBit, RegInfo::Width);
    return;
  }
};

typedef mpl::lambda<mpl::plus<mpl::_1, mpl::_2> >::type inc;

template<typename REGISTERS>
struct SplitVariable
{
  typedef mpl::if_<is_sequence<REGISTERS>, REGISTERS, mpl::list<REGISTERS> >::type Registers;
  typedef unsigned int type;
  typedef mpl::fold<Registers, mpl::int_c<0>, inc>::type Width;
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Width::value <= 8*sizeof(type));
  }
  
  template<typename DEVICE>
  static type get(DEVICE & dev)
  {
    verify();
    type v;
    reader<DEVICE, type> r(dev, v);
    for_each_fold<Registers, mpl::int_c<0>, inc>(r);
    return v;
  }  

  template<typename DEVICE>
  static void set(DEVICE & dev, type i)
  {
    verify();
    writer<DEVICE, type> w(dev, i);
    for_each_fold<Registers, mpl::int_c<0>, inc>(w);
  }  

  template<typename DEVICE>
  static void read(DEVICE & dev)
  {
    dev.reg<Register>().read(dev);
  }
  
  template<typename DEVICE>
  static void write(DEVICE & dev)
  {
    dev.reg<Register>().write(dev);
  }
};

template<typename REGISTER_LIST, typename ACTUAL>
struct Device : public HoldClass<REGISTER_LIST>::type
{
  typedef typename HoldClass<REGISTER_LIST>::type Parent;

  template<typename REGISTER>
  REGISTER & reg()
  {
    Holder<REGISTER> * p = this;
    return p->value;
  }

  template<typename VAR>
  typename VAR::type get()
  {
    return VAR::get(*static_cast<ACTUAL*>(this));
  }

  template<typename VAR>
  void set(typename VAR::type i)
  {
    VAR::set(*static_cast<ACTUAL*>(this), i);
  }  
};

typedef Register<18, DebugIO<NoIO> > ControlReg;
typedef SplitVariable<AtRegister<ControlReg, 16, 17> > High;
typedef SplitVariable<AtRegister<ControlReg, 0, 7> > X;
typedef SplitVariable<AtRegister<ControlReg, 8, 15> > Y;

typedef SplitVariable<mpl::vector<AtRegister<ControlReg, 16, 17>, AtRegister<ControlReg, 0, 1> > >
 Z;

struct Mouse : Device< mpl::vector<ControlReg>, Mouse>
{
};

int main()
{
  Mouse m;

  cout << is_sequence<mpl::vector<int> >::value << endl;
  cout << is_sequence<mpl::list<int> >::value << endl;  
  cout << is_sequence<mpl::range_c<int, 5, 10> >::value << endl;  
  cout << is_sequence<int>::value << endl;


/*
  
  m.set<Z>(-1);
  
  cout << m.reg<ControlReg>() << endl;
  
  m.get<Z>();
  
  cout << m.reg<ControlReg>() << endl;
  
  m.set<High>(0);
  m.set<X>(715827882);
  m.set<Y>(1431655765);
  
  cout << m.get<X>() << endl;
  cout << m.get<Y>() << endl;
  
  cout << m.reg<ControlReg>() << endl;
  
  m.set<Y>(-1);
  
  cout << m.reg<ControlReg>() << endl;

  return 0;
*/  
}
