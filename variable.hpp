#ifndef variable_hpp
#define variable_hpp

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
    Register & reg = dev.template reg<Register>();
    type result;
    reg.get(result, FirstBit, LastBit - FirstBit + 1);
    return result;
  }  
  
  template<typename DEVICE>
  static void set(DEVICE & dev, type i)
  {
    verify();
    Register & reg = dev.template reg<Register>();
    reg.set(i, FirstBit, LastBit - FirstBit + 1);
  }
  
  template<typename DEVICE>
  static void read(DEVICE & dev)
  {
    dev.template reg<Register>().read(dev);
  }
  

  template<typename DEVICE>
  static void write(DEVICE & dev)
  {
    dev.template reg<Register>().write(dev);
  }
};

#endif