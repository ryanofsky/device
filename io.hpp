#ifdef io_hpp
#define io_hpp

template<typename RegIO, typename IndexVar, int index>
struct IndexIO
{
  template<typename Device, typename Register>
  static int read(Device & dev, Register & reg)
  {
    dev.template set<IndexVar>(index);
    dev.write<IndexVar>();
    return RegIO::read(dev, reg);
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    dev.template set<IndexVar>(index);
    dev.write<IndexVar>();
    return RegIO::write(dev, reg);
  }
};

struct NoIO
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
    reg.set(nextval);
    ++nextval;
    return 0;
  }

  template<typename Device, typename Register>  
  static int write(Device & dev, Register & reg)
  {
    return 0;
  }
};

#endif