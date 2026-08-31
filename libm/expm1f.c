//
// expm1f(x) = e^x - 1
//
// Cody-Waite range reduction
//
// e^x - 1 = 2^n * y - 1,  y = e^r in Q30,  r = x - n * ln2
//
// 2^n * y - 1 = 2^n * (y - 2^-n),  and 2^-n leaves in Q30
//
#include <math.h>

// Cody-Waite: LN2_HI keeps 13 bits of ln2, LN2_LO the rest
#define LN2_HI   0x1.62e000p-1f
#define LN2_LO   0x1.0bfbe8p-15f
#define INV_LN2  0x1.715476p+0f

#define QONE 0x40000000UL      // 1.0 * 2^30
#define QLN2 0x2C5C85FDL       // ln2 * 2^30

// exp(x) - 1 = x + x*x*P(x) on |x| <= 0.45, fit weighted by the ulp of the result
#define P0  0x1.0000000000000p-1f
#define P1  0x1.5555540000000p-3f
#define P2  0x1.5553be0000000p-5f
#define P3  0x1.1111f80000000p-7f
#define P4  0x1.6e3a620000000p-10f
#define P5  0x1.a0756a0000000p-13f

static const float PP[6] = { P5, P4, P3, P2, P1, P0 };

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float expm1f(float x)
{
  if (x == 0.0f)
    return x;
  if (isnan(x))
    return x;
  if (x >= 0x1.62e430p+6f)     // ln(FLT_MAX)
    return INFINITY;
  if (x <= -0x1.154246p+4f)    // e^x - 1 rounds to -1.0f
    return -1.0f;

  // 0.45 is the lowest split point. the Q30 path gets worse below it
  if (fabsf(x) <= 0.45f) {
    float x2 = x * x;
    return x + x2 * __polyf(x, PP, 5);
  }

  n_f = floorf(x * INV_LN2);    // z < 0 on 56 inputs over the whole domain
  r = (x - n_f * LN2_HI) - n_f * LN2_LO;
  z = (long)ldexpf(r, 30);
  if (z < 0) {
    z += QLN2;
    n_f = n_f - 1.0f;
  }
  n = (int)n_f;

  // y = e^r in Q30, so 1 << 30 <= y < 1 << 31
  // __u32exp lands between -11.5 and +0.6 of the true value,
  // +4 gives the least max ULP
  y = __u32exp((unsigned long)z) + 4UL;

  if (n >= 31)
    return ldexpf((float)y, n - 30);

  // n <= 30 here, so 1 leaves in Q30. the subtraction is exact
  if (n >= 0)
    return ldexpf((float)(y - (QONE >> n)), n - 30);

  return ldexpf((float)((long)(y >> -n) - (long)QONE), -30);
}
