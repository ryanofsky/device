#ifndef device_hpp
#define device_hpp

template<typename REGISTER_LIST, typename ACTUAL>
struct Device : public HoldClass<REGISTER_LIST>::type
{
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
  typename X::type get()
  {
    return X::get();
  }

  template<typename X>
  void set(typename X::type i)
  {
    X::set(actual(), i);
  }  
  
  template<typename X>
  void read()
  {
    X::read(actual());  
  }
  
  template<typename X>
  void write()
  {
    X::write(actual());  
  }
};

#endif