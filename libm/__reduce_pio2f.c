#include <math.h>

// n * pi/2 for n = 1 to NSMALL, split so that each subtraction is exact.
// TB holds the top and low halves of the boundary B(n+0.5) in turn, and
// ends with a sentinel so the walk needs no counter
#define NSMALL  4
#define W_SMALL 0x40e2u

static const float SA[3*NSMALL] = {
  1.570796371e+00f, -4.371138829e-08f, -1.715124510e-15f,
  3.141592741e+00f, -8.742277657e-08f, -3.430249020e-15f,
  4.712388992e+00f, -1.192488064e-08f, 1.836970147e-16f,
  6.283185482e+00f, -1.748455531e-07f, -6.860498040e-15f,
};

static const unsigned int TB[2*NSMALL] = {
  0x4016u, 0xcbe4u,
  0x407bu, 0x53d1u,
  0x40afu, 0xeddfu,
  0xffffu, 0x0000u,
};

int __rem_pio2f(float x, float *rp);

// Returns the quadrant and leaves the reduced argument in *rp. The locals
// are static by hand: __reduce_pio2f calls __rem_pio2f, so chibicc cannot
// promote them itself
int __reduce_pio2f(float x, unsigned int w, float *rp)
{
  if (w < W_SMALL) {
    static const unsigned int *t;
    static const float *p;
    static float r;
    static int n;
    t = TB - 2;
    p = SA;
    n = 1;
    while (w >= t[2]) {
      t += 2;
      p += 3;
      n++;
    }
    // the low half only decides the one bucket the top half cannot
    if (n != 1 && w == t[0] && *((unsigned int *)&x + 1) < t[1]) {
      p -= 3;
      n--;
    }
    r = ((fabsf(x) - p[0]) - p[1]) - p[2];
    *rp = r;
    if (signbit(x)) {
      *(unsigned char *)rp ^= 0x80u;
      return -n;
    }
    return n;
  }
  return __rem_pio2f(x, rp);
}
