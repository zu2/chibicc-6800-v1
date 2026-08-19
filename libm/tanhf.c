#include <float.h>
#include <math.h>

// tanh(x) rounds to 1 above this point
#define TANH_THRESHOLD 9.2f

// tanh(x) = x + x*x*x*Q(x*x) on |x| <= 1, minimax fit
#define Q0  -0x1.555554p-2f
#define Q1  0x1.11101cp-3f
#define Q2  -0x1.b9dad4p-5f
#define Q3  0x1.630eaap-6f
#define Q4  -0x1.0e0012p-7f
#define Q5  0x1.4b77b6p-9f
#define Q6  -0x1.bf8d96p-12f

float tanhf(float x)
{
  // The polynomial turns -0.0f into +0.0f
  if (x == 0.0f) {
    return x;
  }
  if (isnan(x)) {
    return x;
  }
  if (x > TANH_THRESHOLD) {
    return 1.0f;
  }
  if (x < -TANH_THRESHOLD) {
    return -1.0f;
  }

  if (fabsf(x) <= 1.0f) {
    float x2 = x * x;
    float q = Q0 + x2 * (Q1 + x2 * (Q2 + x2 * (Q3 + x2 * (Q4 +
                x2 * (Q5 + x2 * Q6)))));
    return x + (x2 * x) * q;
  }

  float t = expm1f(2.0f * fabsf(x));
  return copysignf(1.0f - 2.0f / (t + 2.0f), x);
}
