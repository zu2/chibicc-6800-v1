#include <math.h>
#include <stdint.h>

// Fast exponentiation for integer exponents (binary method)
static float fast_int_pow(float x, int n)
{
  if (n < 0) {
    x = 1.0f / x;
    n = -n;
  }
  float result = 1.0f;
  while (n) {
    if (n & 1) {
      result = result * x;
    }
    x = x * x;
    n >>= 1;
  }
  return result;
}

float powf(float x, float y)
{
  // Handle NaN cases
  if (isnan(x) || isnan(y)) {
    return NAN;
  }

  // Handle y is infinite
  if (isinf(y)) {
    if (fabsf(x) == 1.0f) {
      return 1.0f; // 1^Inf = 1
    } else if (fabsf(x) > 1.0f) {
      return (y > 0) ? INFINITY : 0.0f;
    }
    return (y > 0) ? 0.0f : INFINITY;
  }

  // Handle x is infinite
  if (isinf(x)) {
    if (y == 0.0f) {
      return 1.0f; // Inf^0 = 1
    } else if (y > 0.0f) {
      return (x > 0.0f) ? INFINITY
                        : (fmodf(y, 2.0f) == 0.0f ? INFINITY : -INFINITY);
    }
    return 0.0f; // y < 0.0f
  }

  // Return immediately if x == 1.0f (no need to check y)
  if (x == 1.0f) {
    return 1.0f;
  }

  // Return immediately if y == 0.0f (except 0^0, already handled)
  if (y == 0.0f) {
    return 1.0f;
  }

  // Handle x == 0
  if (x == 0.0f) {
    if (y > 0.0f) {
      return 0.0f;
    } else if (y < 0.0f) {
      return INFINITY;
    }
    return NAN; // 0^0 is undefined
  }

  // Use Taylor expansion for x near 1
  if (fabsf(x - 1.0f) < 1e-3f) {
    float product = y * log1pf(x - 1.0f);
    if (product > 88.722839f) {
      return INFINITY;
    }
    return expf(product);
  }

  // Fast path for integer exponents
  if (y == (int)y) {
    return fast_int_pow(x, (int)y);
  }

  // General case
  float product = y * logf(x);
  if (product > 88.722839f) {
    return INFINITY;
  }
  if (product < -103.972084f) {
    return 0.0f;
  }
  return expf(product);
}
