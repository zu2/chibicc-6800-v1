#include <float.h>
#include <math.h>

// The band tests read the top 16 bits of x as an integer, so every
// threshold sits on a 16 bit boundary. Each one is rounded down, which
// only moves points onto the slower and more accurate path
#define W_SIN_SMALL  0x3a12u   // 0x1.24p-11, sin(x) == x below this
#define W_COS_SMALL  0x3980u   // 0x1.00p-12, cos(x) == 1 below this
#define W_PI_4       0x3f80u   // 1.0
#define W_INF        0x7f80u   // NaN and infinity are at or above this

int __rem_pio2f(float x, float *rp);
float __polyf(float x, const float *c, int n);

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


// sin(r) = r + r*u*S(u) on |r| < 1, u = r*r, fpminimax fit over that range
// Zero tails on purpose: the software multiply costs 25 cycles per 1 bit
#define S0  -0x1.555556p-3f
#define S1  0x1.111130p-7f
#define S2  -0x1.a01a00p-13f
#define S3  0x1.6d2100p-19f

// cos(r) = 1 - u/2 + u*u*C(u) on |r| < 1, fpminimax fit, same zero tails
static const float CC[3] = {
  0x1.960900p-16f, -0x1.6bfaa0p-10f, 0x1.555524p-5f,
};

static const float SS[4] = {
  S3, S2, S1, S0,
};

static float kernel_sin(float r)
{
  static float u;

  u = r * r;
  return r + r * (u * __polyf(u, SS, 3));
}

static float kernel_cos(float r)
{
  static float u, p, hz, w;

  u = r * r;
  // below 2^-12 the polynomial falls under the last bit, and u may be subnormal
  if (*(unsigned int *)&u < 0x3980u) {
    return 1.0f - 0.5f * u;
  }
  hz = u;
  *(unsigned int *)&hz -= 0x0080u;
  p = u * (u * __polyf(u, CC, 2));
  if (*(unsigned int *)&hz >= 0x3e00u) {
    // (1 - w) - hz is the exact rounding error of w, so it can be added back
    w = 1.0f - hz;
    return w + (((1.0f - w) - hz) + p);
  }
  return 1.0f - (hz - p);
}

// Returns the quadrant and leaves the reduced argument in *rp
static int reduce(float x, unsigned int w, float *rp)
{
  // For small n the reduction needs no multiply: walk the boundaries with
  // 16 bit compares, then subtract the three words of n * pi/2. The
  // subtractions are exact. TB ends in a sentinel, so the walk needs no
  // counter, and the low half only decides the one ambiguous bucket.
  // The locals are static by hand: reduce calls __rem_pio2f, so chibicc
  // cannot promote them itself
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
  // Payne-Hanek takes every w the small n table does not cover
  return __rem_pio2f(x, rp);
}

float sinf(float x)
{
  unsigned int w;

  w = *(unsigned int *)&x & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && *((unsigned int *)&x + 1) == 0u) ? NAN : x;
  }
  if (w < W_SIN_SMALL) {
    return x;
  }
  if (w < W_PI_4) {
    return kernel_sin(x);
  }
  float r;
  int n = reduce(x, w, &r);

  switch ((int)(n & 3)) {
  case 0:
    return kernel_sin(r);
  case 1:
    return kernel_cos(r);
  case 2:
    return -kernel_sin(r);
  default:
    return -kernel_cos(r);
  }
}

float cosf(float x)
{
  unsigned int w;

  w = *(unsigned int *)&x & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && *((unsigned int *)&x + 1) == 0u) ? NAN : x;
  }
  if (w < W_COS_SMALL) {
    return 1.0f;
  }
  if (w < W_PI_4) {
    return kernel_cos(x);
  }
  float r;
  int n = reduce(x, w, &r);

  switch ((int)(n & 3)) {
  case 0:
    return kernel_cos(r);
  case 1:
    return -kernel_sin(r);
  case 2:
    return -kernel_cos(r);
  default:
    return kernel_sin(r);
  }
}
