#include <float.h>
#include <math.h>

// Threshold to avoid overflow in expf(x)
// cosh(x) = (exp(x) + exp(-x)) / 2
// For x > threshold, cosh(x) ≈ exp(x)/2 → exp(x) < FLT_MAX
// x <= logf(2 * FLT_MAX) ≈ 88.722595
#define COSH_THRESHOLD 88.722595f

float coshf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (isinf(x)) {
    return INFINITY;
  }
  if (fabsf(x) < 1e-10f) {
    return 1.0f;
  }
  if (fabsf(x) < 1.0f) {
    float x2 = x * x;
    return (720.0f + x2 * (360.0f + x2 * (30.0f + x2))) / 720.0f;
  }
  // Return INFINITY for values beyond the threshold to avoid overflow
  if (x > COSH_THRESHOLD) {
    return INFINITY;
  }
  if (x < -COSH_THRESHOLD) {
    return INFINITY;
  }
  // For |x| <= threshold, use the identity cosh(x) = (exp(x) + exp(-x)) / 2
  return (expf(x) + expf(-x)) / 2.0f;
}
