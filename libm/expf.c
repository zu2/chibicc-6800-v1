//
// expf(x) = e^x
//
// Cody–Waite range reduction
//
// e^x = e^r * 2^n,  r = x - n * ln2
//
// LN2_HI: top bits of ln2
// LN2_LO: the remaining part of ln2
//

#include <math.h>

// LN2_HI has 11 mantissa bits. n_f * LN2_HI is exact
// LN2_LO holds the rest of ln2
#define LN2_HI   0x1.62e000p-1f
#define LN2_LO   0x1.0bfbe8p-15f
#define INV_LN2  0x1.715470p+0f

// __u32exp truncates. QLN2 rounds up by 2 to cancel the bias
// ln2 * 2^30 = 0x2C5C85FD.F4
#define QLN2    0x2C5C8600L

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float expf(float x)
{
  if (x == 0.0f)
    return 1.0f;
  if (isnan(x))
    return x;

  // 0x1.62e430p+6f is the smallest float such that e^x rounds to infinity
  if (x >= 0x1.62e430p+6f)
    return INFINITY;

  // -0x1.9fe368p+6f is the smallest float such that e^x > 2^-150
  if (x < -0x1.9fe368p+6f)
    return 0.0f;

  n_f = roundf(x * INV_LN2);
  r = (x - n_f * LN2_HI) - n_f * LN2_LO;
  z = (long)ldexpf(r, 30);
  if (z < 0) {
    z += QLN2;
    n_f = n_f - 1.0f;
  }
  n = (int)n_f;
  y = __u32exp((unsigned long)z);

  return ldexpf((float)y, n - 30);
}
