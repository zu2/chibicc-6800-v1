#include <math.h>

float tanf(float x)
{
  if (x < 0) {
    return -tanf(-x);
  }

  x = fmodf(x, M_TWOPI);
  if (x > M_PI) {
    return -tanf(M_TWOPI - x);
  }
  if (x > M_PI_2) {
    return -tanf(M_PI - x);
  }

  float delta = M_PI_2 - x;
  if (delta < 0.01f) {
    if (delta == 0.0f) {
      return copysignf(INFINITY, delta);
    }
    return 1.0f / delta;
  }

  if (x > M_PI_4) {
    return 1.0f / tanf(M_PI_2 - x);
  }
  if (x > M_PI_8) {
    // tan(x) = (2*tan(x/2)) / (1 - tan(x/2)^2)
    float t = tanf(x / 2.0f);
    return (2.0f * t) / (1.0f - t * t);
  }

  // return sinf(x)/cosf(x);
  float x2 = x * x;
  return x * (0.99999948f +
              x2 * (0.33337722f + x2 * (0.13232848f + x2 * 0.06236088f)));
}
