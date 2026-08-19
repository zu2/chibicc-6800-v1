#include <math.h>

union fword {
  float f;
  unsigned int w[2];
};

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

#define W_INF   0x7f80u   // NaN and infinity are at or above this
#define W_0_75  0x3f40u   // 0.75

float logf(float x)
{
  union fword b;
  unsigned int w, a;

  b.f = x;
  w = b.w[0];
  a = w & 0x7fffu;
  if (a >= W_INF) {
    if (a == W_INF && b.w[1] == 0u && (w & 0x8000u) == 0u) {
      return INFINITY;
    }
    return NAN;
  }
  if (a == 0u && b.w[1] == 0u) {
    return -INFINITY;
  }
  if (w & 0x8000u) {
    return NAN;
  }

  int e;
  b.f = frexpf(x, &e);

  // Precise range reduction: [0.75, 1.5)
  if (b.w[0] < W_0_75) {
    b.w[0] += 0x0080u;
    e--;
  }
  float m = b.f;

  float f = m - 1.0f;
  float s = f / (2.0f + f);
  float z = s * s;
  union fword h;
  h.f = f * f;
  if (h.w[0] != 0u) {
    h.w[0] -= 0x0080u;
  }
  float hfsq = h.f;
  float R = z * (L0 + z * (L1 + z * L2));

  if (e == 0) {
    return f - (hfsq - s * (hfsq + R));
  }

  float e_f = (float)e;
  return e_f * LN2_HI - ((hfsq - (s * (hfsq + R) + e_f * LN2_LO)) - f);
}
