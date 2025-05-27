#include <float.h>
#include <math.h>

// Threshold to avoid overflow in expf(2.0f * x)
#define TANH_THRESHOLD 44.3614f

float tanhf(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (isinf(x)) {
    return copysignf(1.0f, x);
  }
  if (fabsf(x) < 1e-7f) {
    return x;
  }

  // Return ±1.0 for values beyond the threshold to avoid overflow
  if (x > TANH_THRESHOLD) {
    return 1.0f;
  }
  if (x < -TANH_THRESHOLD) {
    return -1.0f;
  }
  // For |x| <= threshold, use the identity tanh(x) = (exp(2x) - 1)/(exp(2x) +
  // 1)
  float exp2x = expf(2.0f * x);
  return (exp2x - 1.0f) / (exp2x + 1.0f);
}
