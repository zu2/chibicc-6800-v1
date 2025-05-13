#include <math.h>
#include <stdint.h>

// Fast exponentiation for integer exponents (binary method)
static float fast_int_pow(float x, int n) {
    if (n < 0) {
        x = 1.0f / x;
        n = -n;
    }
    float result = 1.0f;
    while (n) {
        if (n & 1) result *= x;
        x *= x;
        n >>= 1;
    }
    return result;
}

float powf(float x, float y) {
    // Handle NaN and Inf cases
    if (isnan(x) || isnan(y)) return NAN;
    if (isinf(y)) {
        float z = fabsf(x);
        if (z == 1.0f) return NAN; // 1^Inf is undefined
        if (z > 1.0f)  return (y > 0) ? INFINITY : 0.0f;
        if (z < 1.0f)  return (y > 0) ? 0.0f : INFINITY;
    }
    if (isinf(x)) {
        if (y == 0.0f) return 1.0f; // Inf^0 = 1
        if (y > 0.0f)  return (x > 0.0f) ? INFINITY : (fmodf(y, 2.0f) == 0.0f ? INFINITY : -INFINITY);
        if (y < 0.0f)  return 0.0f;
    }

    // Fast path for x == 1 or y == 0
    if (x == 1.0f || y == 0.0f) return 1.0f;
    if (x == 0.0f) {
        if (y > 0.0f) return 0.0f;
        if (y < 0.0f) return INFINITY;
        return NAN; // 0^0 is undefined
    }

    // Fast path for integer exponents
    if (y == (int)y) {
        return fast_int_pow(x, (int)y);
    }

    // General case
    return expf(y * logf(x));
}

