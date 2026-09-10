#include <math.h>

#define PI_2_HI  0x1.920000p+0f
#define PI_2_LO  0x1.fb5444p-12f
#define PI_HI    0x1.920000p+1f
#define PI_LO    0x1.fb5444p-11f
#define PI_4     0x1.921fb6p-1f
#define PI_4_3   0x1.2d97c8p+1f

#define HIGH16(v) (*(unsigned int *)&(v))
#define LOW16(v)  (*((unsigned int *)&(v) + 1))

#define W_INF  0x7f80u

float atan2f(float y, float x)
{
  unsigned int ax_w, ay_w;
  float ay, r;

  ax_w = HIGH16(x) & 0x7fffu;
  ay_w = HIGH16(y) & 0x7fffu;

  if ((ax_w == 0u || ax_w == W_INF) && ax_w == ay_w
  &&  LOW16(x) == 0u && LOW16(y) == 0u) {
    if (ax_w == 0u) {
      r = signbit(x) ? PI_HI + PI_LO : 0.0f;
    } else {
      r = signbit(x) ? PI_4_3 : PI_4;
    }
    return signbit(y) ? -r : r;
  }

  ay = fabsf(y);

  // The ratio stays inside [-1, 1], so atanf never takes its own reciprocal
  if (ay > fabsf(x)) {
    r = PI_2_HI + (PI_2_LO - atanf(x / ay));
    return signbit(y) ? -r : r;
  }

  r = atanf(y / x);
  if (!signbit(x)) {
    return r;
  }
  return signbit(y) ? -(PI_HI + (PI_LO - r)) : PI_HI + (PI_LO + r);
}
