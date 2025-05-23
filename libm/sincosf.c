#include <float.h>
#include <math.h>

static float minimax_sinf(float x)
{
  float x2 = x * x;
  return x * (1.0f
      + x2 * (-0.1666666667f
      + x2 * (0.0083333333f
      + x2 * (-0.0001984127f
      + x2 * (2.7557319e-06f
      + x2 * -2.5050759e-08f)))));
}

static float minimax_cosf(float x)
{
  float x2 = x * x;
  return 1.0f
       + x2 * (-0.5f
       + x2 * (0.041666641f
       + x2 * (-0.0013888377f
       + x2 * (2.4801587e-05f
       + x2 * -2.7557319e-07f))));
}

float sinf(float x)
{
  if (isnan(x))
    return x;

  if (isinf(x))
    return NAN;

  x = fmodf(x + M_PI, M_TWOPI) - M_PI;

  if (fabsf(x) < 0.001f)
    return x;

  float n_f = roundf(x / M_PI_2);
  int n = (int)n_f;
  float y = x - n_f * M_PI_2;

  switch (n & 3) {
  case 0: // 0〜π/2
    return minimax_sinf(y);
  case 1: // π/2〜π
    return minimax_cosf(y);
  case 2: // π〜3π/2
    return -minimax_sinf(y);
  case 3: // 3π/2〜2π
    return -minimax_cosf(y);
  }
  return 0.0f;  // ??
}

float cosf(float x)
{
  if (isnan(x))
    return x;

  if (isinf(x))
    return NAN;

  x = fabsf(x);
  x = fmodf(x + M_PI, M_TWOPI) - M_PI;

  int n = (int)roundf(x / M_PI_2);
  float y = x - n * M_PI_2;

  float result = minimax_cosf(y);

  switch (n & 3) {
  case 0:
    return result;
  case 1:
    return -minimax_sinf(y);
  case 2:
    return -result;
  case 3:
    return minimax_sinf(y);
  }
  return 0.0f;    // ??
}
