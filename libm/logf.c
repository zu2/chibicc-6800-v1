#include <math.h>

//
// Cody–Waite range reduction
//
// LN2_HI: top 12 bits of ln2, so e * LN2_HI is exact
// LN2_LO: the remaining part of ln2
//
#define LN2_HI  0x1.62e000p-1f
#define LN2_LO  0x1.0bfbe8p-15f

// log1p(f) = f - hfsq + s*(hfsq + R), s = f/(2+f), R = z*L(z), z = s*s
#define L0  0x1.555566p-1f
#define L1  0x1.996298p-2f
#define L2  0x1.32bed2p-2f

float logf(float x)
{
  if (x < 0.0f || isnan(x)) {
    return NAN;
  }
  if (x == 0.0f) {
    return -INFINITY;
  }
  if (isinf(x)) {
    return INFINITY;
  }

  int e;
  float m = frexpf(x, &e);

  // Precise range reduction: [0.75, 1.5)
  if (m < 0.75f) {
    m *= 2.0f;
    e--;
  }

  float f = m - 1.0f;
  float s = f / (2.0f + f);
  float z = s * s;
  float hfsq = 0.5f * f * f;
  float R = z * (L0 + z * (L1 + z * L2));

  if (e == 0) {
    return f - (hfsq - s * (hfsq + R));
  }

  float e_f = (float)e;
  return e_f * LN2_HI - ((hfsq - (s * (hfsq + R) + e_f * LN2_LO)) - f);
}
