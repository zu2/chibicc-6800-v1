#include <math.h>

#define TWO_OVER_PI  0x1.45f306p-1f

// Cody-Waite: PIO2_0 to PIO2_2 hold 8 bits each, so n * PIO2_k stays exact
// while n < 2^16. PIO2_3 carries the tail
#define PIO2_0  0x1.920000p+0f
#define PIO2_1  0x1.fa0000p-12f
#define PIO2_2  0x1.540000p-20f
#define PIO2_3  0x1.10b462p-30f

// tan(r) = r + r*u*A(u)/B(u) on |r| <= pi/4, u = r*r, minimax fit
#define A0  0x1.555556p-2f
#define A1  -0x1.e82d14p-7f
#define B1  -0x1.c75dd8p-2f
#define B2  0x1.05b3d2p-6f

// cot(r) = 1/r - r*C(u) on the same range, minimax fit
#define C0  0x1.555556p-2f
#define C1  0x1.6c169ap-6f
#define C2  0x1.15762ep-9f
#define C3  0x1.b78192p-13f
#define C4  0x1.a4dccap-16f

static float tan_small(float r)
{
  float u = r * r;
  float b = 1.0f + u * (B1 + u * B2);
  float w = u * (A0 + u * A1);
  return r + r * (w / b);
}

static float cot_small(float r)
{
  float u = r * r;
  float c = C0 + u * (C1 + u * (C2 + u * (C3 + u * C4)));
  return -(1.0f / r) + r * c;
}

float tanf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x == INFINITY || x == -INFINITY) {
    return NAN;
  }

  // Below pi/4 the quadrant is 0, so the reduction would only subtract zeros
  float a = fabsf(x);
  if (a < 0.7853981f) {
    return tan_small(x);
  }
  // Up to 3*pi/4 the quadrant is 1, so roundf and the four products drop out
  if (a < 2.3561944f) {
    float r1 = x - copysignf(PIO2_0, x);
    r1 = r1 - copysignf(PIO2_1, x);
    r1 = r1 - copysignf(PIO2_2, x);
    r1 = r1 - copysignf(PIO2_3, x);
    return cot_small(r1);
  }

  float n_f = roundf(x * TWO_OVER_PI);
  long n = (long)n_f;

  float r = x - n_f * PIO2_0;
  r = r - n_f * PIO2_1;
  r = r - n_f * PIO2_2;
  r = r - n_f * PIO2_3;

  if (n & 1) {
    return cot_small(r);
  }
  return tan_small(r);
}
