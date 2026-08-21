#include <float.h>
#include <math.h>

#define TWO_OVER_PI  0x1.45f306p-1f

// Cody-Waite: PIO2_0 to PIO2_2 hold 8 bits each, so n * PIO2_k stays exact
// while n < 2^16. PIO2_3 carries the tail
#define PIO2_0  0x1.920000p+0f
#define PIO2_1  0x1.fa0000p-12f
#define PIO2_2  0x1.540000p-20f
#define PIO2_3  0x1.10b462p-30f

// The band tests read the top 16 bits of x as an integer, so every
// threshold sits on a 16 bit boundary. Each one is rounded down, which
// only moves points onto the slower and more accurate path
#define W_SIN_SMALL  0x3a12u   // 0x1.24p-11, sin(x) == x below this
#define W_COS_SMALL  0x3980u   // 0x1.00p-12, cos(x) == 1 below this
#define W_PI_4       0x3f80u   // 1.0
#define W_3PI_4      0x4020u   // 2.5
#define W_PH         0x47c9u   // n reaches 2^16 here, so Cody-Waite stops being exact
#define W_INF        0x7f80u   // NaN and infinity are at or above this

int __rem_pio2f(float x, float *rp);
float __polyf(float x, const float *c, int n);

union fword {
  float f;
  unsigned int w[2];
};

// sin(r) = r + r*u*S(u) on |r| < 1, u = r*r, fpminimax fit over that range
#define S0  -0x1.555556p-3f
#define S1  0x1.111136p-7f
#define S2  -0x1.a01ae2p-13f
#define S3  0x1.6d210ep-19f

// cos(r) = 1 - u/2 + u*u*C(u) on |r| < 1, fpminimax fit over that range
static const float CC[3] = {
  0x1.960924p-16f, -0x1.6bfaa0p-10f, 0x1.555524p-5f,
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
  static union fword ub;
  static float u, p, hz, w;

  u = r * r;
  ub.f = u;
  // below 2^-12 the polynomial falls under the last bit, and u may be subnormal
  if (ub.w[0] < 0x3980u) {
    return 1.0f - 0.5f * u;
  }
  ub.w[0] -= 0x0080u;
  hz = ub.f;
  p = u * (u * __polyf(u, CC, 2));
  if (ub.w[0] >= 0x3e00u) {
    // (1 - w) - hz is the exact rounding error of w, so it can be added back
    w = 1.0f - hz;
    return w + (((1.0f - w) - hz) + p);
  }
  return 1.0f - (hz - p);
}

// Up to 3*pi/4 the quadrant is +-1, so the four products drop out
static float reduce_one(float x)
{
  float r = x - copysignf(PIO2_0, x);
  r = r - copysignf(PIO2_1, x);
  r = r - copysignf(PIO2_2, x);
  return r - copysignf(PIO2_3, x);
}

// Returns the quadrant and leaves the reduced argument in *rp
static long reduce(float x, unsigned int w, float *rp)
{
  if (w >= W_PH) {
    return __rem_pio2f(x, rp);
  }

  float n_f = roundf(x * TWO_OVER_PI);
  float r = x - n_f * PIO2_0;
  r = r - n_f * PIO2_1;
  r = r - n_f * PIO2_2;
  *rp = r - n_f * PIO2_3;
  return (long)n_f;
}

float sinf(float x)
{
  union fword b;
  unsigned int w;

  b.f = x;
  w = b.w[0] & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && b.w[1] == 0u) ? NAN : x;
  }
  if (w < W_SIN_SMALL) {
    return x;
  }
  if (w < W_PI_4) {
    return kernel_sin(x);
  }
  if (w < W_3PI_4) {
    return copysignf(kernel_cos(reduce_one(x)), x);
  }

  float r;
  long n = reduce(x, w, &r);

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
  union fword b;
  unsigned int w;

  b.f = x;
  w = b.w[0] & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && b.w[1] == 0u) ? NAN : x;
  }
  if (w < W_COS_SMALL) {
    return 1.0f;
  }
  if (w < W_PI_4) {
    return kernel_cos(x);
  }
  // cos is even, so the sign of x drops out here
  if (w < W_3PI_4) {
    return -kernel_sin(reduce_one(fabsf(x)));
  }

  float r;
  long n = reduce(x, w, &r);

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
