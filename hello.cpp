#include <iostream>
#include <cassert>

#include <boost/static_assert.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/int_c.hpp>
#include <boost/mpl/arithmetic/plus.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/range_c.hpp>

using namespace std;
namespace mpl = boost::mpl;

#include "util.hpp"
#include "variable.hpp"
#include "register.hpp"
#include "io.hpp"
#include "debug.hpp"

int NoIO::nextval = 0;

typedef Register<18, DebugIO<NoIO> > ControlReg;
typedef Variable<AtRegister<ControlReg, 16, 17> > High;
typedef Variable<AtRegister<ControlReg, 0, 7> > X;
typedef Variable<AtRegister<ControlReg, 8, 15> > Y;

typedef Variable<mpl::vector<AtRegister<ControlReg, 16, 17>, AtRegister<ControlReg, 0, 1> > >
 Z;

struct Mouse : Device< mpl::vector<ControlReg>, Mouse>
{
};

int main()
{
  Mouse m;
  
  mouse.set<X>(-1);
  
  cout << mouse.reg<ControlReg>() << endl;


/*
  
  m.set<Z>(-1);
  
  cout << m.reg<ControlReg>() << endl;
  
  m.get<Z>();
  
  cout << m.reg<ControlReg>() << endl;
  
  m.set<High>(0);
  m.set<X>(715827882);
  m.set<Y>(1431655765);
  
  cout << m.get<X>() << endl;
  cout << m.get<Y>() << endl;
  
  cout << m.reg<ControlReg>() << endl;
  
  m.set<Y>(-1);
  
  cout << m.reg<ControlReg>() << endl;

  return 0;
*/  
}
