#include <math.h>
#include <stdio.h>

static float atanf_taylor(float x)
{
  float x2 = x * x;
  return x - (x2 * x) / 3.0f + (x2 * x2 * x) / 5.0f -
         (x2 * x2 * x2 * x) / 7.0f + (x2 * x2 * x2 * x2 * x) / 9.0f;
}

#if 1
static float atanf_frac(float x)
{
    float x2 = x * x;
    return x / (1.0f + x2 / (3.0f +
           (4.0f * x2) / (5.0f +
           (9.0f * x2) / (7.0f +
           (16.0f * x2) / (9.0f +
           (25.0f * x2) / (11.0f +
           (36.0f * x2) / (13.0f +
           (49.0f * x2) / (15.0f +
           (64.0f * x2) / (17.0f +
           (81.0f * x2) / (19.0f +
           (100.0f * x2) / (21.0f +
           (121.0f * x2) / (23.0f))))))))))));
}
#else
static float atanf_frac(float x)
{
  float r = 0.0f;
  float x2 = x * x;

  r = (121.0f * x2) / (23.0f + r); // 11
  r = (100.0f * x2) / (21.0f + r); // 10
  r = (81.0f * x2) / (19.0f + r);  //  9
  r = (64.0f * x2) / (17.0f + r);  //  8
  r = (49.0f * x2) / (15.0f + r);  //  7
  r = (36.0f * x2) / (13.0f + r);  //  6
  r = (25.0f * x2) / (11.0f + r);  //  5
  r = (16.0f * x2) / (9.0f + r);   //  4
  r = (9.0f * x2) / (7.0f + r);    //  3
  r = (4.0f * x2) / (5.0f + r);    //  2
  r = (x2) / (3.0f + r);           //  1

  return (x / (1.0f + r)); // 0
}
#endif

float atanf(float x)
{
  if (isnan(x)) {
    return NAN;
  }
  if (x == INFINITY) {
    return M_PI_2;
  }
  if (x == -INFINITY) {
    return -M_PI_2;
  }
  if (x == 0.0) {
    return x;
  }

  if (fabsf(x) < 0.5f) {
    return copysignf(atanf_taylor(fabsf(x)),x);
  } else if (fabsf(x) < 1.5f) {
    return copysignf(atanf_frac(fabsf(x)),x);
  } else {
    float inv_x = 1.0f / fabsf(x);
    return copysignf(M_PI_2 - atanf(inv_x),x);
  }
}
