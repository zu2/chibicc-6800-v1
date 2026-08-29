//
// sinhf(x) = (e^x - e^-x) / 2
//
// Cody-Waite range reduction
//
// a = |x|,  e^a = e^r * 2^n,  r = a - n * ln2
//
// LN2_HI: top bits of ln2. n_f * LN2_HI is exact
// LN2_LO: the remaining part of ln2
//
// z = r * 2^30           Q30
// y = __u32exp(z)        y = e^r * 2^30
//
#include <float.h>
#include <math.h>

// 89.4159927f is the smallest float such that sinh(x) rounds to infinity
#define SINH_THRESHOLD 89.4159927f

// |x| < 2^-11: x itself is correct to under 1 ulp
#define SINH_SMALL 0x1.p-11f

// sinh(x) - x = x^3 * (S0 + S1*x^2 + S2*x^4 + S3*x^6),  |x| < SINH_POLY
#define SINH_POLY 0x1.p+0f
#define S0 0x1.555556p-3f
#define S1 0x1.11111ep-7f
#define S2 0x1.a000e8p-13f
#define S3 0x1.79ab04p-19f

#define SINH_BIG 9.0f

#define QONE 0x40000000UL      // 1.0 * 2^30
#define QLN2 0x2C5C85FDL       // ln2 * 2^30

#define INV_LN2 0x1.715476p+0f
#define LN2_HI  0x1.62e000p-1f
#define LN2_LO  0x1.0bfbe0p-15f

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float sinhf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x >= SINH_THRESHOLD) {
    return INFINITY;
  }
  if (x <= -SINH_THRESHOLD) {
    return -INFINITY;
  }

  float a = fabsf(x);
  if (a < SINH_SMALL) {
    return x;
  }
  if (a < SINH_POLY) {
    float x2 = x * x;
    return x + (x * x2) * (S0 + x2 * (S1 + x2 * (S2 + x2 * S3)));
  }

  n_f = floorf(a * INV_LN2);
  r = (a - n_f * LN2_HI) - n_f * LN2_LO;
  z = (long)ldexpf(r, 30);
  if (z < 0) {
    z += QLN2;
    n_f = n_f - 1.0f;
  }
  n = (int)n_f;
  y = __u32exp((unsigned long)z);

  // e^-a is below 1 ulp of e^a. sinh(a) = e^a / 2
  if (a > SINH_BIG) {
    return copysignf(ldexpf((float)y, n - 31), x);
  }

  // t = e^a - 1
  // sinh(a) = (t + t / (t + 1)) / 2
  float t = ldexpf((float)(y - (QONE >> n)), n - 30);
  return copysignf((t + t / (t + 1.0f)) * 0.5f, x);
}
