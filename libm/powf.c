//
// powf(x, y) = exp2(y * log2 x)
//
// single float. no hi/lo split
// log2f error is multiplied by y
//
// a split costs too much size and speed on 6800
//

#include <math.h>

float powf(float x, float y)
{
  static unsigned char is_int;
  static float sign, v;

  // C11 F.10.4.4: return 1 even if the other operand is NaN
  if (x == 1.0f || y == 0.0f) {
    return 1.0f;
  }

  if (isnan(x) || isnan(y)) {
    return NAN;
  }
  if (x == -1.0f && isinf(y)) {
    return 1.0f;
  }

  is_int = 0;
  sign = 1.0f;
  if (y == floorf(y)) {
    is_int = 1;

    // |y| >= 2^24: y is even. sign stays +
    // (int) saturates at 32767 on chibicc-6800. use long
    if (fabsf(y) < 0x1p24f && signbit(x) && ((long)y & 1L)) {
      sign = -1.0f;
    }
  }

  if (x == 0.0f || isinf(x) || isinf(y)) {
    if (fabsf(x) > 1.0f) {
      return copysignf(signbit(y) ? 0.0f : INFINITY, sign);
    }
    return copysignf(signbit(y) ? INFINITY : 0.0f, sign);
  }

  if (x < 0.0f) {
    if (!is_int) {
      return NAN;
    }
    x = fabsf(x);
  }

  v = y * log2f(x);
  if (v > 128.0f) {     // 2^v overflows at v >= 128
    return copysignf(INFINITY, sign);
  }
  if (v <= -150.0f) {    // 2^v rounds to 0 below -150
    return copysignf(0.0f, sign);
  }

  return copysignf(exp2f(v), sign);
}
