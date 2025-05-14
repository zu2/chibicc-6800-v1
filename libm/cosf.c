#include <math.h>

static const float cos_sign_map[8] = {1.0f,  1.0f,  0.0f, -1.0f,
                               -1.0f, -1.0f, 0.0f, 1.0f};

float cosf(float x)
{
  float x_red = fmodf(x, M_TWOPI);
  if (x_red < 0) {
    x_red += M_TWOPI;
  }

  int quadrant = (int)(x_red / M_PI_4) % 8;
  float base = quadrant * M_PI_4;
  float ref_angle = x_red - base;
  if (quadrant % 2 == 1) {
    ref_angle = M_PI_4 - ref_angle;
  }

  float x2 = ref_angle * ref_angle;
  float x4 = x2 * x2;
  float x6 = x4 * x2;

  float poly = 1.0f - x2 / 2.0f + x4 / 24.0f - x6 / 720.0f;

  return poly * cos_sign_map[quadrant];
}
