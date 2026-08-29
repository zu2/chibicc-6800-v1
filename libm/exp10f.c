//
// exp10f(x) = 10^x
//
// Cody–Waite range reduction
//
// 10^x = e^(r * ln10) * 2^n,  r = x - n * log10(2)
//
// LOG10_2_HI: top bits of log10(2)
// LOG10_2_LO: the remaining part of log10(2)
//
// r * ln10 = 2 * r + r * LN10_C,  LN10_C = ln10 - 2
// 2 * r is exact. only r * LN10_C rounds
//

#include <math.h>

// LOG10_2_HI has 11 mantissa bits. n_f * LOG10_2_HI is exact
// LOG10_2_LO holds the rest of log10(2)
#define LOG2_10      0x1.a934f0p+1f
#define LOG10_2_HI   0x1.344000p-2f
#define LOG10_2_LO   0x1.3509f8p-18f
#define LN10_C       0x1.35d8dep-2f

#define QLN2 744261120L

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float exp10f(float x)
{
  if (isnan(x))
    return x;
  // 38.531841f is the smallest float such that 10^x rounds to infinity
  if (x >= 38.531841f)
    return INFINITY;
  // -0x1.693c6ap+5f is the smallest float such that 10^x >= 2^-150
  if (x < -0x1.693c6ap+5f)
    return 0.0f;

  n_f = roundf(x * LOG2_10);
  r = (x - n_f * LOG10_2_HI) - n_f * LOG10_2_LO;
  z = (long)ldexpf(r, 31) + (long)ldexpf(r * LN10_C, 30);
  if (z < 0) {
    z += QLN2;
    n_f = n_f - 1.0f;
  }
  n = (int)n_f;
  y = __u32exp((unsigned long)z);

  return ldexpf((float)y, n - 30);
}
