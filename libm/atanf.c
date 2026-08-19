#include <math.h>

// atan(x)/x = P(x*x) on [0, 0.5], minimax fit
#define P0  0x1.000000p+0f
#define P1  -0x1.5554f4p-2f
#define P2  0x1.997574p-3f
#define P3  -0x1.22157ep-3f
#define P4  0x1.9e54f8p-4f
#define P5  -0x1.9f06c8p-5f

// (atan(x) - atan(0.75))/(x - 0.75) = R(x - 0.75) on [0.5, 1], minimax fit
// C0_HI holds the top 24 bits of atan(0.75), C0_LO the rest
#define C0_HI  0x1.4978fap-1f
#define C0_LO  0x1.934f70p-28f
#define R0  0x1.47ae14p-1f
#define R1  -0x1.3a92a2p-2f
#define R2  0x1.ec234cp-5f
#define R3  0x1.c2f886p-5f
#define R4  -0x1.0be980p-4f
#define R5  0x1.d7ca20p-6f
#define R6  0x1.fce0f8p-8f
#define R7  -0x1.3a0684p-6f

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
    float u = a - 0.75f;
    r = C0_HI + (u * (R0 + u * (R1 + u * (R2 + u * (R3 + u * (R4 + u * (R5 + u * (R6 + u * R7))))))) + C0_LO);
  }

  if (inv) {
    r = PI_2_HI + (PI_2_LO - r);
  }
  return copysignf(r, x);
}
