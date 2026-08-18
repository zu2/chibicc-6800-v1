#include <math.h>

//
// Cody–Waite range reduction
// 
// LN2_HI: top 24 bits of ln2
// LN2_LO: the remaining part of ln2
//
#define LN2_HI   0x1.62e000p-1f
#define LN2_LO   0x1.0bfbe8p-15f
#define INV_LN2  0x1.715476p+0f

// exp(r) = 1 + r + r*r*P(r) on |r| <= ln2/2, minimax fit
#define P0  0x1.000000p-1f
#define P1  0x1.5554dcp-3f
#define P2  0x1.55551ap-5f
#define P3  0x1.120b6cp-7f
#define P4  0x1.6d1106p-10f

float expf(float x)
{
  if (x == 0.0f)
    return 1.0f;
  if (isnan(x))
    return x;
  if (x > 0x1.62e430p+6f)
    return INFINITY;
  if (x < -0x1.9fe368p+6f)
    return 0.0f;

  // n = round(x / ln2)
  float n_f = roundf(x * INV_LN2);
  int n = (int)n_f;

  // r = x - n*ln2, Cody–Waite range reduction
  float r = (x - n_f * LN2_HI) - n_f * LN2_LO;

  float r2 = r * r;
  float p = P0 + r * (P1 + r * (P2 + r * (P3 + r * P4)));
  float e = 1.0f + (r + r2 * p);

  return ldexpf(e, n);
}
