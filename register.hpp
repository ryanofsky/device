#ifndef register_hpp
#define register_hpp

template<int BITS, typename IO>
struct Register
{
  enum { IntBits = 8 * sizeof(unsigned int) };
  enum { CacheDim = (BITS + IntBits - 1)/IntBits };
  unsigned int cache[CacheDim];

  enum { Bits = BITS };
  typedef IO Io;

  template<typename INT>
  void set(INT n, unsigned int pos = 0, unsigned int len = 8*sizeof(INT))
  {
    BOOST_STATIC_ASSERT(sizeof(INT) <= sizeof(unsigned int));
    assert(pos + len <= Bits);
    assert(len <= IntBits);
    
    INT nMask = ~0;
    n &= ~(nMask << len);
    
    int p = pos / IntBits;
    int offset = pos - p * IntBits;
    int leftover = offset + len - IntBits;

    unsigned int mask = ~0;
    unsigned int bits = n;
    bits <<= offset;

    if (leftover > 0)
    {
      cache[p] = (cache[p] & ~(mask << offset)) | bits;
      ++p;
      bits = n >> (IntBits - offset);
      cache[p] = (cache[p] & (mask << leftover)) | bits;
    }
    else
    {
      unsigned int m = (mask << offset) & (mask >> -leftover);
      cache[p] = (cache[p] & ~m) | bits;
    }
  }
  
  template<typename INT>
  void get(INT & n, unsigned int pos = 0, unsigned int len = 8*sizeof(INT))
  {
    assert(pos + len <= Bits);
    assert(len <= IntBits);

    int p = pos / IntBits;
    int offset = pos - p * IntBits;
    int leftover = offset + len - IntBits;

    unsigned int mask = ~0;

    if (leftover > 0)
    {
      n = cache[p] >> offset;
      n |= (cache[p+1] & ~(mask << leftover)) << (IntBits - offset);
    }
    else
    {
      unsigned int m = (mask << offset) & (mask >> -leftover);
      n = (cache[p] & (mask >> -leftover)) >> offset;
    }
  }
  
  void clear()
  {
    for (int i = 0; i < CacheDim; ++i)
      cache[i] = 0;
  }
  
  Register()
  {
    IO::init(*this);
  }
  
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