#ifndef is_sequence_hpp
#define is_sequence_hpp

#include <boost/mpl/list.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/range_c.hpp>

// this is a temporary kludge that will have to suffice until a real
// is_sequence method gets added to MPL. 
// It is not implemented using boost's is_convertible type trait
// because that can't match against template classes. Instead it uses
// a tweaked version of the Conversion metafunction described in MC++D
//
// is_sequence is defined in two classes instead of one
// because sizeof doesn't seem to work in g++ on the return
// values of static member functions defined in the same class
// where sizeof is being invoked. this happens with g++ 2.95.4
// and g++ version 3.2 (mingw special 20020817-1)

template<typename T>
struct is_sequence_impl
{
  static T MakeT();

  typedef char Small;
  class Big { char dummy[2]; };

  // list detection
  template<typename U, typename V, typename W>
  static Small Test(boost::mpl::list_node<U, V, W>);

  // range detection
  template<typename U, U V, U W>
  static Small Test(boost::mpl::range_c<U, V, W>);

  // vector detection
  static Small Test(boost::mpl::vector0<>);

  // more vector detection for some compilers
#if !defined(BOOST_MPL_TYPEOF_BASED_VECTOR_IMPL)
#  include "boost/preprocessor/enum_params.hpp"
#  include "boost/preprocessor/repeat_from_to.hpp"
#  define AUX_DECL(N, I, DATA)                            \
     template<BOOST_PP_ENUM_PARAMS(I, typename U)>        \
     static Small Test(BOOST_PP_CAT(boost::mpl::vector,I) \
       <BOOST_PP_ENUM_PARAMS(I, U)>);
  BOOST_PP_REPEAT_FROM_TO(1, BOOST_MPL_LIMIT_VECTOR_SIZE, AUX_DECL, DATA)
#  undef AUX_DECL
#endif

  // fallback
  static Big Test(...);
};

template<typename T>
struct is_sequence
{
  typedef is_sequence_impl<T> i;
  enum { value = sizeof(i::Test(i::MakeT()))==sizeof(typename i::Small) };
};

#endif