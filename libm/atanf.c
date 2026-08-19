#include <math.h>

// atan(x)/x = P(x*x) on [0, 0.5], minimax fit
#define P0  0x1.000000p+0f
#define P1  -0x1.5554f4p-2f
#define P2  0x1.997574p-3f
#define P3  -0x1.22157ep-3f
#define P4  0x1.9e54f8p-4f
#define P5  -0x1.9f06c8p-5f

// atan(x) = Q(x) on [0.5, 1], minimax fit
#define Q0  0x1.0485dap-9f
#define Q1  0x1.f49958p-1f
#define Q2  0x1.b34dccp-4f
#define Q3  -0x1.3854e6p-1f
#define Q4  0x1.a24e38p-2f
#define Q5  -0x1.7ac126p-4f
#define Q6  -0x1.d6f91ep-7f
#define Q7  0x1.fcf8bep-8f

// PI_2_HI holds the top 12 bits of pi/2, PI_2_LO the rest
#define PI_2_HI  0x1.920000p+0f
#define PI_2_LO  0x1.fb5444p-12f

float atanf(float x)
{
  if (isnan(x)) {
    return x;
  }

  float a = fabsf(x);
  if (a == INFINITY) {
    return copysignf(PI_2_HI + PI_2_LO, x);
  }

  int inv = a > 1.0f;
  if (inv) {
    a = 1.0f / a;
  }

  float r;
  if (a < 0.5f) {
    float a2 = a * a;
    r = a * (P0 + a2 * (P1 + a2 * (P2 + a2 * (P3 + a2 * (P4 + a2 * P5)))));
  } else {
    r = Q0 + a * (Q1 + a * (Q2 + a * (Q3 + a * (Q4 + a * (Q5 + a * (Q6 + a * Q7))))));
  }

  if (inv) {
    r = (PI_2_HI - r) + PI_2_LO;
  }
  return copysignf(r, x);
}
