//
// tanhf(x) = tanh(x)
//
// tanh(|x|) = 1 - 2 / (e^(2|x|) + 1)
//
// Cody-Waite range reduction
//
// e^a = e^r * 2^n,  a = 2|x|,  r = a - n * ln2
//
// e^a + 1 = (e^r + 2^-n) * 2^n
//

#include <float.h>
#include <math.h>

// tanh(x) rounds to 1 above this point
#define TANH_THRESHOLD 9.2f

// tanh(x) = x + x*x*x*Q(x*x) on |x| <= TANH_POLY, minimax fit
// 0x1.9cp-1f is where the poly and __u32exp error curves cross.
// max ULP is lowest here
#define TANH_POLY 0x1.9cp-1f

#define Q0  -0x1.555552p-2f
#define Q1  0x1.111022p-3f
#define Q2  -0x1.b9ecd4p-5f
#define Q3  0x1.641a72p-6f
#define Q4  -0x1.13d7d8p-7f
#define Q5  0x1.67bc1ap-9f
#define Q6  -0x1.11a7d8p-11f

#define QONE 0x40000000UL   // 1.0 in Q30

// round(ln2 * 2^30) = 0x2C5C85FE. +2 cancels the truncation in (long)ldexpf(r, 30)
#define QLN2 0x2C5C8600L

#define INV_LN2 (1.0f / M_LN2)

// LN2_HI has 12 mantissa bits. n_f * LN2_HI is exact
// LN2_LO holds the rest of ln2
#define LN2_HI  0x1.62e000p-1f
#define LN2_LO  0x1.0bfbe0p-15f

static const float QQ[7] = { Q6, Q5, Q4, Q3, Q2, Q1, Q0 };

static float n_f, r;
static long z;
static unsigned long y;
static int n;

float tanhf(float x)
{
  // The polynomial turns -0.0f into +0.0f
  if (x == 0.0f) {
    return x;
  }
  if (isnan(x)) {
    return x;
  }
  if (x > TANH_THRESHOLD) {
    return 1.0f;
  }
  if (x < -TANH_THRESHOLD) {
    return -1.0f;
  }

  if (fabsf(x) <= TANH_POLY) {
    float x2 = x * x;
    return x + (x2 * x) * __polyf(x2, QQ, 6);
  }

  float a2 = fabsf(x) + fabsf(x);
  n_f = roundf(a2 * INV_LN2);
  r = (a2 - n_f * LN2_HI) - n_f * LN2_LO;
  z = (long)ldexpf(r, 30);
  n = (int)n_f;
  if (z < 0) {
    z += QLN2;
    n--;
  }
  // Q30: e^r + 2^-n
  y = __u32exp((unsigned long)z) + (QONE >> n);
  return copysignf(1.0f - 2.0f / ldexpf((float)y, n - 30), x);
}
