#ifndef debug_hpp
#define debug_hpp

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

template<typename CHAIN_IO>
struct DebugIO
{
  template<typename Register>
  static void init(Register & reg)
  {
    CHAIN_IO::init(reg);
  }

  template<typename Device, typename Register>
  static int read(Device & dev, Register & reg)
  {
    int ret = CHAIN_IO::read(dev, reg);
    int current;
    reg.get(current);

    cout << "Device " << &dev << ". Reading register "
         << typeid(Device).name() << ". Value = " << current << endl;

    return ret;
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    int current;
    reg.get(current);
    cout << "Device " << &dev << ". Writing register "
         << NAME << ". Value = " << current << endl;
    return CHAIN_IO::write(dev, reg);
  }
};

#endif