#include <math.h>

const float PI_OVER_4 = 0x1.921fb6p-1f; // π/4 (0.7853981634f)
const float PI_OVER_2 = 0x1.921fb6p0f;  // π/2 (1.5707963268f)
const float TWO_PI = 0x1.921fb6p2f;     // 2π (6.2831853072f)

float atanf(float x)
{
  const float a1 = 0.99997726f, a3 = -0.33262347f, a5 = 0.19354346f;
  const float a7 = -0.11643287f, a9 = 0.05265332f, a11 = -0.01172120f;

  if (fabsf(x) <= 1.0f) {
    float x2 = x * x;
    return x * (a1 + x2 * (a3 + x2 * (a5 + x2 * (a7 + x2 * (a9 + x2 * a11)))));
  } else {
    float inv = 1.0f / fabsf(x);
    float x2 = inv * inv;
    float core =
        inv * (a1 + x2 * (a3 + x2 * (a5 + x2 * (a7 + x2 * (a9 + x2 * a11)))));
    float y = 1.57079637f - core; // π/2
    return (x < 0) ? -y : y;
  }
}

