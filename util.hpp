#ifndef util_hpp
#define util_hpp

template<typename T>
struct is_sequence : public boost::is_same<typename T::tag, mpl::aux::list_tag >
{};

/////////////////////////////

template<typename It, typename End>
struct for_each_fold_impl
{
  template<typename State, typename ForwardOp, typename Functor>
  void operator()(Functor & f)
  {
    f.operator()<typename It::type, State>();
    for_each_fold_impl<typename It::next, End> next;
    typedef typename mpl::apply2<ForwardOp, State, typename It::type>::type NextState;
    next.operator()<NextState, ForwardOp>(f);
  }
};

template<typename End>
struct for_each_fold_impl<End, End>
{
  template<typename State, typename ForwardOp, typename Functor>
  void operator()(Functor &)
  {}
};

template<typename Sequence, typename State, typename ForwardOp, typename Functor>
void for_each_fold(Functor & f)
{
  for_each_fold_impl<typename mpl::begin<Sequence>::type, typename mpl::end<Sequence>::type> first;
  first.operator()<State, ForwardOp>(f);
};

/////////////////////////////

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
  typedef typename mpl::fold<Sequence, EmptyType, ScFun>::type type;
};

#endif