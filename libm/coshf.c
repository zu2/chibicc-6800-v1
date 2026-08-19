#include <float.h>
#include <math.h>

// cosh(x) overflows above this point
#define COSH_THRESHOLD 89.4159927f

// expf(x) overflows above this point
#define COSH_HALF 0x1.62e430p+6f

// cosh(x) == 1.0f below this point
#define COSH_SMALL 3.45267006e-04f

float coshf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x > COSH_THRESHOLD || x < -COSH_THRESHOLD) {
    return INFINITY;
  }

  float a = fabsf(x);
  if (a < COSH_SMALL) {
    return 1.0f;
  }
  if (a < 1.0f) {
    float t = expm1f(a);
    float u = t + 1.0f;
    return 1.0f + (t * t) / (u + u);
  }
  if (a < COSH_HALF) {
    float u = expf(a);
    return u * 0.5f + 0.5f / u;
  }

  float w = expf(a * 0.5f);
  return (w * 0.5f) * w;
}
