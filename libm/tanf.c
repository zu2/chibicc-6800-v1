#include <math.h>

float tanf(float x)
{
  float cos_val = cosf(x);
  if (cos_val == 0.0f) {
    return (x > 0) ? INFINITY : -INFINITY;
  }
  return sinf(x) / cos_val;
}
