#ifndef util_hpp
#define util_hpp

#include <boost/type_traits/object_traits.hpp>

// this is a temporary kludge that will have to suffice until a real
// is_sequence method gets added to MPL. 
// It is not implemented using boost's is_convertible type trait
// because that can't match against template classs. Instead it uses
// a tweaked version of the Conversion metafunction described in MC++D
template<typename T>
struct is_sequence
{
  typedef char Small;
  class Big { char dummy[2]; };
  template<typename VAR, typename par, typename mar>
  static Small Test(mpl::list_node<VAR, par, mar>);

  // this doesn't detect vector with msvc60
  // there, vectors inherit from a vector_impl class that takes
  // different number of types depending on number max params
  template<typename VAR>
  static Small Test(mpl::vector0<VAR>);

  template<typename VAR, VAR Start, VAR Finish>
  static Small Test(mpl::range_c<VAR, Start, Finish>);

  static Big Test(...);
  static T MakeT();
  enum { value = sizeof(Test(MakeT())) == sizeof(Small) };  
};

/*
template<typename T>
struct is_sequence
{
  enum { value = false };
};

template<typename T, typename U, typename V>
struct is_sequence<
is_same<mpl::aux::list_tag>

mpl::list_node<T, U, V> >
{
  enum { value = true };
};
*/
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