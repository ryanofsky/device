#include <iostream>

template<int BASE, int SIZE>
struct Register
{
  enum { Base = BASE, Size = SIZE };
};

template<typename REGISTER, int STARTBIT, int ENDBIT>
struct AtRegister
{
 
  template<typename INTR, typename INTL>
  void AssignBits(INTR & r, INTL const & l)
  {
    const INTR ones = ~static_cast<INTR>(0);
    const INTR mask = (ones << STARTBIT) | (ones >> ENDBIT);
    const INTR val = static_cast<INTL>(l) << STARTBIT;
    r = (r & mask) | (val & mask);
  }

  template<typename INTR, typename INTL>
  void AssignBits(INTR & r, INTL const & l)
  {
    const INTR ones = ~static_cast<INTR>(0);
    const INTR mask = (ones << STARTBIT) | (ones < ENDBIT);
    const INTR val = static_cast<INTL>(l) << STARTBIT;
    r = l >> STARTBIT;
  }  
};

#define ONES(type) static_cast<type>(~static_cast<type>(0))


#define MASK(type, start, end) ONES(type) << static_cast<type>(end)

/*


#define MASK(type, start, end) (                            \
                                                           \
    ~(ONES(type) << static_cast<type>(end))                 \
 &  (ONES(type) << static_cast<type>(start))               \
                                                           \
 )





#define MASK(type, start, end) (                            \
~(                                                          \
     (ONES(type) << static_cast<type>(end))                 \
 |  ~(ONES(type) << static_cast<type>(start))               \
 )                                                          \
 )
*/

template<int SIZE, typename LOCATION>
struct Variable
{
  enum { Size = SIZE }; 
  
};

template<typename INTT>
void PrintBinary(INTT i)
{
  for (int s = sizeof(INTT) * 8 - 1; s >=0; --s)
  {
    std::cout << (1 & (i >> s));
  }
  std::cout << std::endl;
}

int main()
{
  PrintBinary(ONES(unsigned long long));
  PrintBinary(MASK(unsigned long, 2, 8));
 
  return 0;
}