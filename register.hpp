#ifndef register_hpp
#define register_hpp

template<int BITS, typename IO>
struct Register
{
  enum { IntBits = 8 * sizeof(unsigned) };
  enum { CacheDim = (BITS + IntBits - 1)/IntBits };
  unsigned cache[CacheDim];

  enum { Bits = BITS };
  typedef IO Io;

  template<unsigned pos, unsigned len, typename INT>
  void set(INT n)
  {
    BOOST_STATIC_ASSERT(sizeof(INT) <= sizeof(unsigned));
    BOOST_STATIC_ASSERT(pos + len <= Bits);
    BOOST_STATIC_ASSERT(len <= IntBits);
    
    INT nMask = ~0;
    n &= ~(nMask << len);
    
    int p = pos / IntBits;
    int o = pos - p * IntBits;
    int leftover = o + len - IntBits;

    unsigned mask = ~0;
    unsigned bits = (n & ~(mask << len)) << o;

    if (leftover > 0)
    {
      cache[p] = (cache[p] & ~(mask << o)) | bits;
      ++p;
      bits = n >> (IntBits - o);
      cache[p] = (cache[p] & (mask << leftover)) | bits;
    }
    else
    {
      unsigned m = (mask << o) & (mask >> -leftover);
      cache[p] = (cache[p] & ~m) | bits;
    }
  }
  
  template<unsigned pos, unsigned len, typename INT>
  void get(INT & n)
  {
    assert((pos + len) / IntBits <= Bits / IntBits);
    assert(len <= IntBits);

    int p = pos / IntBits;
    int o = pos - p * IntBits;
    int leftover = o + len - IntBits;

    unsigned mask = ~0;

    if (leftover > 0)
    {
      n = cache[p] >> o;
      n |= (cache[p+1] & ~(mask << leftover)) << (IntBits - o);
    }
    else
    {
      unsigned m = (mask << o) & (mask >> -leftover);
      n = (cache[p] & (mask >> -leftover)) >> o;
    }
  }
  
  Register()
  {
    // this can be unrolled
    for (int i = 0; i < CacheDim; ++i)
      cache[i] = 0;
  }
  
  // xxx: these should be static methods, are they even being used in their current form?
  
  template<typename DEVICE>
  int read(DEVICE & device)
  {
    return IO::read(device, *this);
  }

  template<typename DEVICE>
  int write(DEVICE & device)
  {
    return IO::write(device, *this);
  }
};

#endif