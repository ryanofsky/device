#ifndef util_hpp
#define util_hpp

#include <boost/type_traits/object_traits.hpp>
#include <boost/mpl/apply_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/iter_fold_backward.hpp>

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

template<typename begin_, typename end_>
struct subsequence
{
  struct tag;
  typedef begin_ begin;
  typedef end_ end;
};

template<typename begin>
struct unsorted_unique_op
{
  template<typename state, typename i>
  struct apply
  {
    typedef boost::mpl::apply_if
    < boost::is_same<i, boost::mpl::find<subsequence<begin, i>, i::type>::type>,
      boost::mpl::identity<state>,
      boost::mpl::push_front<state, i::type>
    >::type type;
  };
};

template<typename sequence>
struct unsorted_unique
{
  typedef boost::mpl::begin<sequence>::type begin;
  typedef boost::mpl::end<sequence>::type end;
  
  typedef boost::mpl::iter_fold_backward
  < sequence,
    boost::mpl::clear<sequence>::type,
    unsorted_unique_op<begin>
  >::type type;
};








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