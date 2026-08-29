#include <math.h>


// asin(t) = t + t*v*Q(v), v = t*t, minimax fit on |t| <= 0.5
#define Q0  0x1.5555c6p-3f
#define Q1  0x1.330384p-4f
#define Q2  0x1.74479ep-5f
#define Q3  0x1.8ebe24p-6f
#define Q4  0x1.56e21cp-5f

static const float QQ[5] = {
  Q4, Q3, Q2, Q1, Q0,
};

// PI_2_HI holds the top 12 bits of pi/2, PI_2_LO the rest
#define PI_2_HI  0x1.920000p+0f
#define PI_2_LO  0x1.fb5444p-12f
#define PI_HI    0x1.920000p+1f
#define PI_LO    0x1.fb5444p-11f

static float kernel_asin(float t)
{
  static float v;

  v = t * t;
  return t + (t * v) * __polyf(v, QQ, 4);
}

// 1-a is exact on [0.5, 1], and s lands in [0, 0.5], so one kernel serves both
static float half_asin(float a)
{
  static float t;

  t = kernel_asin(sqrtf((1.0f - a) * 0.5f));
  return t + t;
}

float asinf(float x)
{
  float a = fabsf(x);
  float r;

  if (a > 1.0f) {
    return NAN;
  }
  if (a <= 0.5f) {
    return kernel_asin(x);
  }
  r = PI_2_HI + (PI_2_LO - half_asin(a));
  return signbit(x) ? -r : r;
}

float acosf(float x)
{
  float a = fabsf(x);
  float r;

  if (a > 1.0f) {
    return NAN;
  }
  if (a <= 0.5f) {
    return PI_2_HI + (PI_2_LO - kernel_asin(x));
  }
  r = half_asin(a);
  return signbit(x) ? PI_HI + (PI_LO - r) : r;
}
