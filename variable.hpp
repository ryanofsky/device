#ifndef variable_hpp
#define variable_hpp

#include "is_sequence.hpp"
#include <boost/mpl/transform.hpp>
#include <boost/mpl/for_each.hpp>

template<typename REGISTER, int FIRSTBIT, int LASTBIT>
struct AtRegister
{
  typedef REGISTER Register;
  enum { FirstBit = FIRSTBIT , LastBit = LASTBIT };
  enum { Width = LASTBIT - FIRSTBIT + 1 };
  enum { value = Width };
  typedef Register type;
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Register::Bits > LastBit);
    BOOST_STATIC_ASSERT(Register::Bits > FirstBit);
    BOOST_STATIC_ASSERT(LastBit >= FirstBit);
  }  
};

namespace aux
{
  struct GetRegister
  {
    template<typename T>
    struct apply
    {
      typedef typename T::Register type;  
    };
  };

  template<typename DEVICE>
  struct RegisterReader
  {
    DEVICE & device;
  
    RegisterReader(DEVICE & device_)
    : device(device_) {}
  
    template<typename Register>
    void operator()(boost::mpl::identity<Register>)
    {
      Register & r = device.reg<Register>();
      r.read(device);
    }
  };
  
  template<typename DEVICE>
  struct RegisterWriter
  {
    DEVICE & device;
  
    RegisterWriter(DEVICE & device_)
    : device(device_) {}
  
    template<typename Register>
    void operator()(boost::mpl::identity<Register>)
    {
      Register & r = device.reg<Register>();
      r.write(device);
    }
  };
  
  typedef mpl::lambda<mpl::plus<mpl::_1, mpl::_2> >::type Sum;
  
  template<typename DEVICE, typename INTEGER>
  struct VariableGetter
  {
    DEVICE & device;
    INTEGER & value;
  
    VariableGetter(DEVICE & device_, INTEGER & value_)
    : device(device_), value(value_) {}
    
    template<typename RegInfo, typename Position>
    void operator()()
    {
      RegInfo::verify();
      RegInfo::Register & reg = device.reg<RegInfo::Register>();
      INTEGER result;
      reg.get<RegInfo::FirstBit, RegInfo::Width>(result);
      value |= result << Position::value;
      return;
    }
  };
  
  template<typename DEVICE, typename INTEGER>
  struct VariableSetter
  {
    DEVICE & device;
    INTEGER & value;
  
    VariableSetter(DEVICE & device_, INTEGER & value_)
    : device(device_), value(value_) {}
    
    template<typename RegInfo, typename Position>
    void operator()()
    {
      RegInfo::verify();
      RegInfo::Register & reg = device.reg<RegInfo::Register>();
      reg.set<RegInfo::FirstBit, RegInfo::Width>(value >> Position::value);
      return;
    }
  };
}


struct IdentityClass
{
  template<typename T>
  struct apply
  {
    typedef boost::mpl::identity<T> type;
  };
};

//typedef boost::mpl::lambda<boost::mpl::identity<boost::mpl::_> > IdentityClass;

template<typename A, typename B>
struct append : boost::mpl::fold_backward<A, B
  boost::mpl::push_front<boost::mpl::_1, boost::mpl::_2> >;
{};

template<typename REGISTERS>
struct Variable
{
  typedef mpl::if_<is_sequence<REGISTERS>, REGISTERS, mpl::list<REGISTERS> >::type Registers;
  typedef unsigned Integer;
  typedef mpl::fold<Registers, boost::mpl::int_c<0>, aux::Sum>::type Width;
  
  typedef unsorted_unique<boost::mpl::transform<Registers, aux::GetRegister>::type> AccessList;
  
  template<typename REGISTER_LIST>
  struct apply
  {
    typedef boost::mpl::push_front<REGISTER_LIST>::type type;
  }
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Width::value <= 8*sizeof(Integer));
  }
  
  template<typename DEVICE>
  static Integer get(DEVICE & dev)
  {
    verify();
    Integer v(0);
    aux::VariableGetter<DEVICE, Integer> r(dev, v);
    for_each_fold<Registers, boost::mpl::int_c<0>, aux::Sum>(r);
    return v;
  }  

  template<typename DEVICE>
  static void set(DEVICE & dev, Integer i)
  {
    verify();
    aux::VariableSetter<DEVICE, Integer> w(dev, i);
    for_each_fold<Registers, mpl::int_c<0>, aux::Sum>(w);
  }  

  template<typename DEVICE>
  static void read(DEVICE & dev)
  {
    verify();
    aux::RegisterReader<DEVICE> r(dev);
    boost::mpl::for_each<AccessList, IdentityClass>(r);    
  }
  
  template<typename DEVICE>
  static void write(DEVICE & dev)
  {
    verify();
    aux::RegisterWriter<DEVICE> w(dev);
    PrintList<AccessList>();
    boost::mpl::for_each<AccessList, IdentityClass>(w);    
  }
};

#endif