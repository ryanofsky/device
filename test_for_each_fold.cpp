#include "util.h"

struct Print
{
  template<typename T, typename State>
  void operator()()
  {
    cout << "Type " << State::value << ": " << typeid(T).name() << endl;
  }
};

int main()
{
  Print p;
  typedef mpl::list3<int, short, char> l;
  typedef mpl::lambda<mpl::plus<mpl::_1, mpl::int_c<1> > >::type inc;
  for_each_fold<l, mpl::int_c<0>, inc>(p);      
}