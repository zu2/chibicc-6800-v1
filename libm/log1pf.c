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
#define L0  0x1.555556p-1f
#define L1  0x1.999a6ap-2f
#define L2  0x1.242ce0p-2f
#define L3  0x1.e6587ap-3f

float log1pf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x == 0.0f) {
    return x;
  }
  if (x < -1.0f) {
    return NAN;
  }
  if (x == -1.0f) {
    return -INFINITY;
  }
  if (isinf(x)) {
    return INFINITY;
  }

  float u = 1.0f + x;

  // c is the exact rounding error of u, and stays inside the final rounding
  float c = (x > 1.0f) ? (1.0f - (u - x)) : (x - (u - 1.0f));
  c /= u;

  int e;
  float m = frexpf(u, &e);

  // Precise range reduction: [0.75, 1.5)
  if (m < 0.75f) {
    m *= 2.0f;
    e--;
  }

  float f = m - 1.0f;
  float s = f / (2.0f + f);
  float z = s * s;
  float hfsq = 0.5f * f * f;
  float R = z * (L0 + z * (L1 + z * (L2 + z * L3)));

  if (e == 0) {
    return f - (hfsq - (s * (hfsq + R) + c));
  }

  float e_f = (float)e;
  return e_f * LN2_HI - ((hfsq - (s * (hfsq + R) + (e_f * LN2_LO + c))) - f);
}
