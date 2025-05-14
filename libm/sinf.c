#include <math.h>

const float PI_OVER_4 = 0x1.921fb6p-1f; // π/4 (0.7853981634f)
const float PI_OVER_2 = 0x1.921fb6p0f;  // π/2 (1.5707963268f)
const float TWO_PI = 0x1.921fb6p2f;     // 2π (6.2831853072f)
const float sin_sign_map[8] = {1.0f,  1.0f,  -1.0f, -1.0f,
                               -1.0f, -1.0f, 1.0f,  1.0f};

float sinf(float x)
{
  float x_red = fmodf(x, TWO_PI);
  if (x_red < 0) {
    x_red += TWO_PI;
  }

  int quadrant = (int)(x_red / PI_OVER_4);
  quadrant %= 8;

  float base = quadrant * PI_OVER_4;
  float ref_angle = x_red - base;
  if (quadrant % 2 == 1) {
    ref_angle = PI_OVER_4 - ref_angle;
  }

  float x2 = ref_angle * ref_angle;
  float poly =
      1.0f - x2 * (1.0f / 6.0f - x2 * (1.0f / 120.0f -
                                       x2 * (1.0f / 5040.0f - x2 / 362880.0f)));
  float result = ref_angle * poly;

  return result * sin_sign_map[quadrant];
}
