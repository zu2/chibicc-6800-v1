#include <math.h>

const float PI_OVER_4 = 0x1.921fb6p-1f; // π/4 (0.7853981634f)
const float PI_OVER_2 = 0x1.921fb6p0f;  // π/2 (1.5707963268f)
const float TWO_PI = 0x1.921fb6p2f;     // 2π (6.2831853072f)

float tanf(float x)
{
  float cos_val = cosf(x);
  if (cos_val == 0.0f) {
    return (x > 0) ? INFINITY : -INFINITY;
  }
  return sinf(x) / cos_val;
}
