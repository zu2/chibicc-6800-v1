//
// coshf(x) = (e^a + e^-a) / 2,  a = |x|
//
// Cody-Waite range reduction
//
// e^a = e^r * 2^n,  r = a - n * ln2
//
// h        = e^a / 2  = y * 2^(n-31)
// 0.25 / h = e^-a / 2
//
// result = h + 0.25 / h
// the add is done on y:  y += (0.25 / h) * 2^(31-n)
//
#include <float.h>
#include <math.h>

#define COSH_THRESHOLD 89.4159927f

// x*x/2 falls below half an ulp of 1.0 here
#define COSH_SMALL 0x1.p-12f

// polynomial below, __u32exp above. accuracy vs speed
// __u32exp accepts z in [0, ln2) only
#define COSH_LN2 0x1.62e430p-1f

// above this point exp(-a) is below half an ulp of exp(a)
#define COSH_BIG 0x1.1p+3f

// cosh(x) = 1 + x2 * (C0 + C1*x2 + C2*x2^2 + C3*x2^3),  x2 = x * x
#define C0 0x1.000000p-1f
#define C1 0x1.555560p-5f
#define C2 0x1.6c0eccp-10f
#define C3 0x1.a53744p-16f

// round(ln2 * 2^30) = 0x2C5C85FE. +2 cancels the truncation in (long)ldexpf(r, 30)
#define QLN2 0x2C5C8600L

#define INV_LN2 (1.0f / M_LN2)

// LN2_HI has 12 mantissa bits. n_f * LN2_HI is exact
// LN2_LO holds the rest of ln2
#define LN2_HI  0x1.62e000p-1f // ln2 split in two: HI holds the top 12 bits
#define LN2_LO  0x1.0bfbe0p-15f // HI + LO is ln2 to 1.6e-11, so n * ln2 stays exact

static const float CC[4] = { C3, C2, C1, C0 };

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float coshf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x >= COSH_THRESHOLD || x <= -COSH_THRESHOLD) {
    return INFINITY;
  }

  float a = fabsf(x);
  if (a < COSH_SMALL) {
    return 1.0f;
  }
  if (a < COSH_LN2) {
    float x2 = x * x;
    return 1.0f + x2 * __polyf(x2, CC, 3);
  }

  n_f = roundf(a * INV_LN2);
  r = (a - n_f * LN2_HI) - n_f * LN2_LO;
  z = (long)ldexpf(r, 30);
  n = (int)n_f;
  if (z < 0) {
    z += QLN2;
    n--;
  }
  y = __u32exp((unsigned long)z);

  float h = ldexpf((float)y, n - 31);
  if (a > COSH_BIG) {
    return h;
  }
  y = y + (unsigned long)ldexpf(0.25f / h, 31 - n);
  return ldexpf((float)y, n - 31);
}
