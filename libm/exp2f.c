//
// exp2f(x) = 2^x
//
// 2^x = e^(r * ln2) * 2^n,  r = x - n,  n = round(x)
//
// r = x - n_f is exact. only r * LN2 rounds
//

#include <math.h>

#define LN2 0x1.62e430p-1f
#define QLN2 744261120L   // = LN2 * 2^30 = 0x2C5C8600

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float exp2f(float x)
{
  if (isnan(x))
    return x;
  if (x >= 128.0f)
    return INFINITY;
  if (x < -150.0f)
    return 0.0f;

  n_f = roundf(x);

  r = x - n_f;

  // |r| <= 0.5, so |z| <= 0.5 * ln2 * 2^30
  z = (long)ldexpf(r * LN2, 30);

  // __u32exp needs 0 <= z < ln2 * 2^30
  // e^z * 2^n = e^(z + ln2) * 2^(n - 1)
  if (z < 0) {
    z += QLN2;
    n_f = n_f - 1.0f;
  }
  n = (int)n_f;

  // y = e^(z / 2^30) in Q30. 1 <= y < 2
  y = __u32exp((unsigned long)z);

  return ldexpf((float)y, n - 30);
}
