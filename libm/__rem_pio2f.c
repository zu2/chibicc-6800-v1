#include <math.h>

// 2/pi in 416 bits, from the top of the fraction down
static const unsigned long TP[13] = {
  0xa2f9836eUL, 0x4e441529UL, 0xfc2757d1UL,
  0xf534ddc0UL, 0xdb629599UL, 0x3c439041UL,
  0xfe5163abUL, 0xdebbc561UL, 0xb7246e3aUL,
  0x424dd2e0UL, 0x06492eeaUL, 0x09d1921cUL,
  0xfe1deb1cUL,
};

#define PIO2_HI  0x1.921fb6p+0f
#define PIO2_LO  -0x1.777a5cp-25f

static unsigned long window(int p)
{
  unsigned char w, s;
  unsigned long hi, lo;

  if (p < 0) {
    return (p <= -32) ? 0UL : (TP[0] >> (-p));
  }
  w = (unsigned char)(p >> 5);
  s = p & 31;
  hi = (w < 13) ? TP[w] : 0UL;
  lo = (w + 1 < 13) ? TP[w + 1] : 0UL;
  if (s == 0) {
    return hi;
  }
  return (hi << s) | (lo >> (32 - s));
}

// 24 bit x 96 bit. Only the digits around the binary point are built:
// acc0 and acc1 sit above it and never reach the result, and the digits
// below acc4 can only carry in, which is worth less than 0.001 ulp
int __rem_pio2f(float x, float *rp)
{
  int e, p;
  unsigned long m, v0, v1, v2, t;
  unsigned long a2, a3, a4;
  unsigned int m0, m1, w0, w1, w2, w3, w4;
  signed char q;
  float frac;

  {
    union {
      float f;
      unsigned int w[2];
      unsigned long u;
    } a;
    a.f = x;
    e = (int)((a.w[0] >> 7) & 0xffu) - 150;
    m = (a.u & 0x7fffffUL) | 0x800000UL;
  }
  p = e - 2;

  v0 = window(p);
  v1 = window(p + 32);
  v2 = window(p + 64);

  m0 = (unsigned int)(m >> 16);
  m1 = (unsigned int)(m & 0xffffUL);
  w0 = (unsigned int)(v0 >> 16);
  w1 = (unsigned int)(v0 & 0xffffUL);
  w2 = (unsigned int)(v1 >> 16);
  w3 = (unsigned int)(v1 & 0xffffUL);
  w4 = (unsigned int)(v2 >> 16);

  a2 = 0UL;
  a3 = 0UL;
  a4 = 0UL;

  t = (unsigned long)m0 * w1;  a2 += t & 0xffffUL;
  t = (unsigned long)m1 * w0;  a2 += t & 0xffffUL;
  t = (unsigned long)m0 * w2;  a2 += t >> 16;  a3 += t & 0xffffUL;
  t = (unsigned long)m1 * w1;  a2 += t >> 16;  a3 += t & 0xffffUL;
  t = (unsigned long)m0 * w3;  a3 += t >> 16;  a4 += t & 0xffffUL;
  t = (unsigned long)m1 * w2;  a3 += t >> 16;  a4 += t & 0xffffUL;
  t = (unsigned long)m0 * w4;  a4 += t >> 16;
  t = (unsigned long)m1 * w3;  a4 += t >> 16;

  a3 += a4 >> 16;  a4 &= 0xffffUL;
  a2 += a3 >> 16;  a3 &= 0xffffUL;

  q = (signed char)((a2 >> 14) & 3UL);

  {
    unsigned long h = ((a2 & 0x3fffUL) << 16) | a3;
    unsigned long l = a4;
    unsigned char neg = 0;

    // Taking the complement here keeps the low bits that a float
    // subtraction of 1.0f would drop
    if (h >= 0x20000000UL) {
      unsigned long borrow = (l != 0UL) ? 1UL : 0UL;
      l = (0UL - l) & 0xffffUL;
      h = (0x40000000UL - h - borrow) & 0x3fffffffUL;
      neg = 1;
      q += 1;
    }
    frac = (float)(h >> 14) * 0x1p-16f
         + (float)(((h & 0x3fffUL) << 16) | l) * 0x1p-46f;
    if (neg) {
      frac = -frac;
    }
  }

  if (signbit(x)) {
    frac = -frac;
    q = -q;
  }

  *rp = frac * PIO2_HI + frac * PIO2_LO;
  return q & 3;
}
