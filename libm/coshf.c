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
  // Return INFINITY for values beyond the threshold to avoid overflow
  if (x > COSH_THRESHOLD) {
    return INFINITY;
  }
  if (x < -COSH_THRESHOLD) {
    return INFINITY;
  }
  // For |x| <= threshold, use the identity cosh(x) = (exp(x) + exp(-x)) / 2
  float expx = expf(x);
  float expmx = expf(-x);
  return (expx + expmx) / 2.0f;
}
