#include <math.h>

float tanf(float x)
{
  // Use fmodf to reduce x into [-PI, PI]
  float r = fmodf(x, (float)M_PI);

  // Fold into [-PI/2, PI/2]
  if (r > (float)M_PI_2) {
    r -= (float)M_PI;
  } else if (r < -(float)M_PI_2) {
    r += (float)M_PI;
  }

  // Use 1/delta approximation near the asymptote for |r| close to π/2
  float delta = (r > 0) ? (M_PI_2 - r) : (-M_PI_2 - r);
  if (fabsf(delta) < 0.01f) {
    if (delta == 0.0f) {
      return (r > 0) ? INFINITY : -INFINITY;
    }
    return (r > 0 ? 1.0f : -1.0f) / delta;
  }

  // Standard computation using sinf/cosf
  float cos_val = cosf(r);
  if (cos_val == 0.0f) {
    return (r > 0) ? INFINITY : -INFINITY;
  }
  return sinf(r) / cos_val;
}
