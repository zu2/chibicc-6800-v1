#include <float.h>
#include <math.h>

// Threshold to avoid overflow in expf(x)
// sinh(x) = (exp(x) - exp(-x)) / 2
// For x > threshold, sinh(x) ≈ exp(x)/2 → exp(x) < FLT_MAX
// x <= logf(2 * FLT_MAX) ≈ 88.722595
#define SINH_THRESHOLD 88.722595f

float sinhf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (isinf(x)) {
    return x;
  }
  if (fabsf(x) < 1e-10f) {
    return x;
  }

  // Return ±INFINITY for values beyond the threshold to avoid overflow
  if (x > SINH_THRESHOLD) {
    return INFINITY;
  }
  if (x < -SINH_THRESHOLD) {
    return -INFINITY;
  }
  // For |x| <= threshold, use the identity sinh(x) = (exp(x) - exp(-x)) / 2
  float expx = expf(x);
  float expmx = expf(-x);
  return (expx - expmx) / 2.0f;
}

