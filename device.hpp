#ifndef device_hpp
#define device_hpp

template<typename Device>
struct dread
{
  Device * device;
  dread(Device * device_) : device(device_) {}
  void operator()()
  
}

template<typename REGISTER_LIST, typename ACTUAL>
struct Device : public HoldClass<REGISTER_LIST>::type
{
  typedef Device<REGISTER_LIST, ACTUAL> ThisType;

  template<typename REGISTER>
  REGISTER & reg()
  {
    Holder<REGISTER> * p = this;
    return p->value;
  }
  
  ACTUAL & actual()
  {
    return *static_cast<ACTUAL*>(this);
  }
  
  template<typename X>
  typename X::Integer get()
  {
    return X::get(actual());
  }

  template<typename X>
  void set(typename X::Integer i)
  {
    X::set(actual(), i);
  }  
  
  template<typename X>
  void read()
  {
    typedef boost::mpl::as_sequence<X>::type VariableList;
    typedef boost::mpl::fold_backward<VariableList, boost::mpl::list<>, 
      boost::mpl::apply<boost::mpl::_2, boost::mpl::_1> >
    >::type RegisterList;
    
    typedef unsorted_unique<RegisterList>::type y;
    
    boost::mpl::for_each<y, boost::mpl::identity<boost::mpl::_1>(dread<ThisType>(&device));
    
    X::read(actual());  
  }
  
  template<typename X>
  void write()
  {
    X::write(actual());  
  }
};

#endif