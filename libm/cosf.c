#include <math.h>

const float PI_OVER_4 = 0x1.921fb6p-1f; // π/4 (0.7853981634f)
const float PI_OVER_2 = 0x1.921fb6p0f;  // π/2 (1.5707963268f)
const float TWO_PI = 0x1.921fb6p2f;     // 2π (6.2831853072f)
const float cos_sign_map[8] = {1.0f,  1.0f,  0.0f, -1.0f,
                               -1.0f, -1.0f, 0.0f, 1.0f};

float cosf(float x)
{
  float x_red = fmodf(x, TWO_PI);
  if (x_red < 0) {
    x_red += TWO_PI;
  }

  int quadrant = (int)(x_red / PI_OVER_4) % 8;
  float base = quadrant * PI_OVER_4;
  float ref_angle = x_red - base;
  if (quadrant % 2 == 1) {
    ref_angle = PI_OVER_4 - ref_angle;
  }

  float x2 = ref_angle * ref_angle;
  float x4 = x2 * x2;
  float x6 = x4 * x2;

  float poly = 1.0f - x2 / 2.0f + x4 / 24.0f - x6 / 720.0f;

  return poly * cos_sign_map[quadrant];
}
