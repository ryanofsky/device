#ifndef util_hpp
#define util_hpp

#include <boost/type_traits/object_traits.hpp>

#   define AUX_VECTOR_PARAMS(param) \
    BOOST_PP_ENUM_PARAMS( \
          BOOST_MPL_LIMIT_VECTOR_SIZE \
        , param \
        ) \


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
  static Small Test(boost::mpl::list_node<VAR, par, mar>);

  template<typename VAR>
  static Small Test(boost::mpl::vector0<VAR>);

  template<typename VAR, VAR Start, VAR Finish>
  static Small Test(boost::mpl::range_c<VAR, Start, Finish>);

#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
  template<AUX_VECTOR_PARAMS(typename U)>
  static Small Test(boost::mpl::vector_impl<AUX_VECTOR_PARAMS(typename U)>);
#endif

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

//////////////////////////////////////////////////////
/*
begin<Sequence>::type
==
begin_traits<Sequence::tag>::template algorithm< Sequence >::type

struct subsequence_tag;

typedef<typename begin_, typename end_>
struct subsequence
{
  typedef subsequence_tag;
  typedef begin_ begin;
  typedef end_ end;
};

template<>
struct boost::mpl::begin_traits<subsequence_tag>
{
  template<typename subsequence>
  struct algorithm
  {
    typedef subsequence::begin type;
  };
};

template<>
struct boost::mpl::end_traits<subsequence_tag>
{
  template<typename subsequence>
  struct algorithm
  {
    typedef subsequence::end type;
  };
};

template<typename Sequence>
struct Unique
{
  typedef mpl::fold_backward<>::type type;
};

*/

#endif