#include <iostream>
#include <cassert>

#include <boost/static_assert.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/int_c.hpp>
#include <boost/mpl/arithmetic/plus.hpp>

using namespace std;
namespace mpl = boost::mpl;



struct DebugIO
{
  static int nextval;
  
  template<typename Register>
  static void init(Register & reg)
  {
    reg.clear();
  }

  template<typename Device, typename Register>
  static int read(Device & dev, Register & reg)
  {
    cout << "Device " << &dev << ". Reading register "
         << typeid(Device).name() << ". Value = " << nextval << endl;
    reg.set(nextval);
    ++nextval;
    return 0;
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    int current;
    reg.get(current);
    cout << "Device " << &dev << ". Writing register "
         << NAME << ". Value = " << current << endl;
    return 0;
  }
};

template<typename RegIO, typename IndexVar, int index>
struct IndexIO
{
  template<typename Device, typename Register>
  static int read(Device & dev, Register & reg)
  {
    dev.set<IndexVar>(index))
    dev.write<IndexVar>();
    return RegIO::read(dev, reg);
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    dev.set<IndexVar>(index))
    dev.write<IndexVar>();
    return RegIO::write(dev, reg);
  }
};


int DebugIO::nextval = 0;

// missing from devil:
//  bit masks 
//    bit masks accomplish two things:
//      runtime constraint checking
//        express that drivers require certain register bits to be 0 or 1
//        and allow generated code to check for this when reading and writing
//        this would be better implemented here in the IO struct
//
//      compile time safety and completeness check
//        mask marks every bit as used or unused
//        checks that every used bit maps to a variable (completeness)
//          this would be impossible to implement anyway under the current
//          framework, because you can't deduce variable types from a 
//          register type. It is redundant because if you forget
//          to declare a variable, the code that tries to use the variable
//          won't compile. 
//        checks that no unused bit is mapped to a variable (safety)
//          for this purpose masks provide a useful redundancy
//         
//  parameterized registers - using the example in the Devil manual
//
//    register reg(i : int(2)) = read base @ 0, pre {index = i} : bit[8];
//    register x_low = reg(0);
//
//    template<int i>
//    struct reg : Register<8, IndexIO< PortIO<0>, index, i > > {};
//
//    typedef reg<0> x_low;
//      instead of
//        typedef MyRegister1<32, PortIO<0>
//
//  Map Type Equation / Algebraic types:
//    algebraic types are essentially enumerations, instead of
//    just being defined by a bit width, they are described as 
//    lists of possible values. The simplest uses of algebraic t
//    types are just like C++ enum's. One feature they have
//    that enum's lack is that possible values may be described
//    wildcards
//
//  variable features missing:
//    volatile option
//      causes every device.get<var>()
//      to be implicity preceded by a device.read<var>()
//
//    trigger option
//      causes every device.set<var>()
//      to be implicity followed by a device.write<var>()
//
// structure serialized as
//   the default Structure reads and writes from registers
//   in the order first encountered in the variable list
//   This should almost always be sufficient. When it is not
//   simply define a custom structure classed with its
//   own read() and write() methods. The 


template<int BITS, typename IO>
struct Register
{
  enum { IntBits = 8 * sizeof(unsigned int) };
  enum { CacheDim = (BITS + IntBits - 1)/IntBits };
  unsigned int cache[CacheDim];

  enum { Bits = BITS };
  typedef IO Io;

  template<typename INT>
  void set(INT n, unsigned int pos = 0, unsigned int len = 8*sizeof(INT))
  {
    BOOST_STATIC_ASSERT(sizeof(INT) <= sizeof(unsigned int));
    assert(pos + len <= Bits);
    assert(len <= IntBits);
    
    INT nMask = ~0;
    n &= ~(nMask << len);
    
    int p = pos / IntBits;
    int offset = pos - p * IntBits;
    int leftover = offset + len - IntBits;

    unsigned int mask = ~0;
    unsigned int bits = n;
    bits <<= offset;

    if (leftover > 0)
    {
      cache[p] = (cache[p] & ~(mask << offset)) | bits;
      ++p;
      bits = n >> (IntBits - offset);
      cache[p] = (cache[p] & (mask << leftover)) | bits;
    }
    else
    {
      unsigned int m = (mask << offset) & (mask >> -leftover);
      cache[p] = (cache[p] & ~m) | bits;
    }
  }
  
  template<typename INT>
  void get(INT & n, unsigned int pos = 0, unsigned int len = 8*sizeof(INT))
  {
    assert(pos + len <= Bits);
    assert(len <= IntBits);

    int p = pos / IntBits;
    int offset = pos - p * IntBits;
    int leftover = offset + len - IntBits;

    unsigned int mask = ~0;

    if (leftover > 0)
    {
      n = cache[p] >> offset;
      n |= (cache[p+1] & ~(mask << leftover)) << (IntBits - offset);
    }
    else
    {
      unsigned int m = (mask << offset) & (mask >> -leftover);
      n = (cache[p] & (mask >> -leftover)) >> offset;
    }
  }
  
  void clear()
  {
    for (int i = 0; i < CacheDim; ++i)
      cache[i] = 0;
  }
  
  Register()
  {
    IO::init(*this);
  }
  
  template<typename DEVICE>
  int read(DEVICE & device)
  {
    return IO::read(device, *this);
  }

  template<typename DEVICE>
  int write(DEVICE & device)
  {
    return IO::write(device, *this);
  }
};

template<int BITS, typename IO>
ostream & operator<<(ostream & out, Register<BITS, IO> const & reg)
{
  char str[BITS+1];
  const int d = 8*sizeof(unsigned int);
  for (int i = 0; i < BITS; ++i)
    str[BITS-i-1] = (reg.cache[i / d] & 1 << (i % d)) ? '1' : '0';
  str[BITS] = 0;
  out << str;
  return out;
}

template<typename REGISTER, int FIRSTBIT, int LASTBIT>
struct Variable
{
  typedef unsigned int type;

  typedef REGISTER Register;
  enum { FirstBit = FIRSTBIT , LastBit = LASTBIT };
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Register::Bits > LastBit);
    BOOST_STATIC_ASSERT(Register::Bits > FirstBit);
    BOOST_STATIC_ASSERT(LastBit >= FirstBit);
  }
  
  template<typename DEVICE>
  static type get(DEVICE & dev)
  {
    verify();
    Register & reg = dev.reg<Register>();
    type result;
    reg.get(result, FirstBit, LastBit - FirstBit + 1);
    return result;
  }  
  
  template<typename DEVICE>
  static void set(DEVICE & dev, type i)
  {
    verify();
    Register & reg = dev.reg<Register>();
    reg.set(i, FirstBit, LastBit - FirstBit + 1);
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


template<typename T>
struct is_sequence : public mpl::bool_c<boost::is_same<T::tag, mpl::aux::list_tag >::value>
{};

/*








template<typename REGISTER, int FIRSTBIT, int LASTBIT>
struct AtRegister
{
  typedef REGISTER Register;
  enum { FirstBit = FIRSTBIT , LastBit = LASTBIT };
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Register::Bits > LastBit);
    BOOST_STATIC_ASSERT(Register::Bits > FirstBit);
    BOOST_STATIC_ASSERT(LastBit >= FirstBit);
  }  

  template<typename DEVICE>
  static int type get(DEVICE & dev)
   v
    verify();
    Register & reg = dev.reg<Register>();
    type result;
    reg.get(result, FirstBit, LastBit - FirstBit + 1);
    return result;
  }  
  
  template<typename DEVICE>
  static void set(DEVICE & dev, type i)
  {
    verify();
    Register & reg = dev.reg<Register>();
    reg.set(i, FirstBit, LastBit - FirstBit + 1);
  }
};


template<typename IT, int POS>
struct reader
{
  template<typename DEVICE, typename INTEGER>
  static void type get(DEVICE & dev, INTEGER & i)
  {
    typedef IT::item Reg;
    reg::verify();
    BOOST_STATIC_ASSERT(8*sizeof(INTEGER) >= POS + Reg::LastBit - Reg::FirstBit + 1);
    
    Register & reg = dev.reg<Register>();
    INTEGER result;
    reg.get(result, Reg::FirstBit, Reg::LastBit - Reg::FirstBit);
    
    i |= reg << POS;
    
    typedef nextreader;
    nextreader::get(dev, i);
    
    return;
  }
  
}

template<typename IT, int POS>
struct nullreader
{
  template<typename DEVICE, typename INTEGER>
  static void type get(DEVICE & dev, INTEGER & i)
  {
    return;
  }
  
}


using boost::is_same;

template<typename REGISTERS>
struct SplitVariable
{
  typedef unsigned int type;
  
  template<typename DEVICE>
  static int type get(DEVICE & dev)
  {
    typedef mpl::begin<REGISTERS>::type It;
    typedef mpl::if<is_same<>, reader<It,0>, nullreader<It,0> > rd;
    
    rd::read
  
    verify();
    Register & reg = dev.reg<Register>();
    type result;
    reg.get(result, FirstBit, LastBit - FirstBit + 1);
    return result;
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
*/



















struct EmptyType {};

template <class L, class R>
struct InheritTwo : public L, public R
{
};


template<typename T>
struct Holder
{
  T value;
};

// try replacing this with lambda
struct ScFun
{
  template<typename STATE, typename VAL>
  struct apply
  {
    typedef InheritTwo< Holder<VAL>, STATE > type;
  };
};

template<typename Sequence>
struct HoldClass
{
  typedef mpl::fold<Sequence, EmptyType, ScFun>::type type;
};

template<typename REGISTER_LIST, typename ACTUAL>
struct DeviceBase : public HoldClass<REGISTER_LIST>::type
{
  typedef HoldClass<REGISTER_LIST>::type Parent;

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

template<typename REGISTER_LIST>
struct Device : public DeviceBase<REGISTER_LIST, Device<REGISTER_LIST> >
{};

typedef Register<18, DebugIO> ControlReg;
typedef Variable<ControlReg, 16, 17> High;
typedef Variable<ControlReg, 0, 7> X;
typedef Variable<ControlReg, 8, 15> Y;

struct Mouse : DeviceBase< mpl::list1<ControlReg>, Mouse >
{
};






template<typename It, typename End>
struct for_each_impl
{
  template<typename State, typename ForwardOp, typename Functor>
  void operator()(Functor & f)
  {
    f.operator()<typename It::type, State>();
    for_each_impl<It::next, End> next;
    typedef mpl::apply2<ForwardOp, State, It::type>::type NextState;
    next<NextState, ForwardOp>(f);
  }
};

template<typename End>
struct for_each_impl<End, End>
{
  template<typename State, typename ForwardOp, typename Functor>
  void operator()(Functor &)
  {}
};

template<typename Sequence, typename State, typename ForwardOp, typename Functor>
void for_each(Functor & f)
{
  for_each_impl<mpl::begin<Sequence>::type, mpl::end<Sequence>::type> first;
  first.operator()<State, ForwardOp>(f);
};

struct Print
{
  template<typename T, typename State>
  void operator()()
  {
    cout << "Type " << State::value << ": " << typeid(T).name() << endl;
  }
};

int main()
{
	typedef mpl::list3<int, short, char> l;
	Print p;
	for_each<l, mpl::int_c<0>, mpl::plus<mpl::_1, mpl::_2> >(p);
	
	
	
	
	//extern char const name[] = "hsll";

/*
  Mouse m;

	cout << m.reg<ControlReg>() << endl;
  
  m.set<High>(0);
  m.set<X>(715827882);
  m.set<Y>(1431655765);

  cout << m.get<X>() << endl;
  cout << m.get<Y>() << endl;

	cout << m.reg<ControlReg>() << endl;

  m.set<Y>(-1);
  
	cout << m.reg<ControlReg>() << endl;
*/
	
	return 0;
}
