#ifndef debug_hpp
#define debug_hpp

template<int BITS, typename IO>
ostream & operator<<(ostream & out, Register<BITS, IO> const & reg)
{
  char str[BITS+1];
  const int d = 8*sizeof(unsigned);
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
    cout << "Device " << &dev << ". Reading register "
         << typeid(Device).name() << ". Value = " << reg << endl;
    return ret;
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    cout << "Device " << &dev << ". Writing register "
         << ". Value = " << reg << endl;
    return CHAIN_IO::write(dev, reg);
  }
};

struct printer
{
  printer(std::ostream& s) : f_stream(&s) {}
  template< typename U > void operator()(mpl::identity<U>)
  {
    *f_stream << typeid(U).name() << '\n';
  }

 private:
    std::ostream* f_stream;
};

template<typename LIST>
void PrintList()
{
  boost::mpl::for_each<LIST,boost::mpl::make_identity<boost::mpl::_> >(printer(std::cout));
}

#endif