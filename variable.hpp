#ifndef variable_hpp
#define variable_hpp

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
      typedef T::register type;  
    }  
  };

  template<typename DEVICE>
  struct RegisterReader
  {
    DEVICE & device;
  
    var_reader(DEVICE & device_)
    : device(device_) {}
  
    template<typename Register>
    void operator()()
    {
      Register & r = device.reg<Register>();
      r.read(device);
    }
  };
  
  template<typename DEVICE>
  struct RegisterWriter
  {
    DEVICE & device;
  
    var_reader(DEVICE & device_)
    : device(device_) {}
  
    template<typename Register>
    void operator()()
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
  struct VariableSetter
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
};

template<typename REGISTERS>
struct Variable
{
  typedef mpl::if_<is_sequence<REGISTERS>, REGISTERS, mpl::list<REGISTERS> >::type Registers;
  typedef unsigned int Integer;
  typedef mpl::fold<Registers, boost::mpl::int_c<0>, aux::Sum>::type Width;
  
  typedef unsorted_unique<boost::mpl::transform<Registers, aux::GetRegister>::type> AccessList;
  
  static void verify()
  {
    BOOST_STATIC_ASSERT(Width::value <= 8*sizeof(type));
  }
  
  template<typename DEVICE>
  static Integer get(DEVICE & dev)
  {
    verify();
    Integer v;
    reader<DEVICE, type> r(dev, v);
    for_each_fold<Registers, boost::mpl::int_c<0>, aux::Sum>(r);
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
    verify();
    VariableReader<DEVICE> r(dev);
    boost::mpl::for_each<AccessList>(r);    
  }
  
  template<typename DEVICE>
  static void write(DEVICE & dev)
  {
    verify();
    VariableWriter<DEVICE> r(dev);
    dev.reg<Register>().write(dev);
  }
};

#endif