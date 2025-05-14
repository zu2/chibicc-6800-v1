#include <math.h>

static const float sin_sign_map[8] = {1.0f,  1.0f,  -1.0f, -1.0f,
                               -1.0f, -1.0f, 1.0f,  1.0f};

float sinf(float x)
{
  float x_red = fmodf(x, M_TWOPI);
  if (x_red < 0) {
    x_red += M_TWOPI;
  }

  int quadrant = (int)(x_red / M_PI_4);
  quadrant %= 8;

  float base = quadrant * M_PI_4;
  float ref_angle = x_red - base;
  if (quadrant % 2 == 1) {
    ref_angle = M_PI_4 - ref_angle;
  }

  float x2 = ref_angle * ref_angle;
  float poly =
      1.0f - x2 * (1.0f / 6.0f - x2 * (1.0f / 120.0f -
                                       x2 * (1.0f / 5040.0f - x2 / 362880.0f)));
  float result = ref_angle * poly;

  return result * sin_sign_map[quadrant];
}
