#include <float.h>
#include <math.h>

// sinh(x) overflows above this point
#define SINH_THRESHOLD 89.4159927f

// expf(x) overflows above this point
#define SINH_HALF 0x1.62e430p+6f

// sinhf returns x below this point. The truncation error stays under 1 ulp
// and beats the expm1f path up to here
#define SINH_SMALL 7.04223407e-04f

float sinhf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x > SINH_THRESHOLD) {
    return INFINITY;
  }
  if (x < -SINH_THRESHOLD) {
    return -INFINITY;
  }

  float a = fabsf(x);
  if (a < SINH_SMALL) {
    return x;
  }
  if (a < SINH_HALF) {
    float t = expm1f(a);
    return copysignf((t + t / (t + 1.0f)) * 0.5f, x);
  }

  float w = expf(a * 0.5f);
  return copysignf((w * 0.5f) * w, x);
}
