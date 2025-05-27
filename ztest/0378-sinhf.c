#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_printf.c"

// Structure to hold a test case
typedef struct {
  float input;
  float expected;
} TestCase;

// Threshold to avoid overflow in expf(x)
// sinh(x) = (exp(x) - exp(-x)) / 2
// For x > threshold, sinh(x) ≈ exp(x)/2 → exp(x) < FLT_MAX
// x <= logf(2 * FLT_MAX) ≈ 88.7226
#define SINH_THRESHOLD 88.722595f

static TestCase test_cases[] = {
    {-2.00000000e+01f, -2.42582592e+08f},
    {-1.00000000e+01f, -1.10132324e+04f},
    {-1.00000000e+00f, -1.17520118e+00f},
    {-5.00000000e-01f, -5.21095276e-01f},
    {0.00000000e+00f, 0.00000000e+00f},
    {5.00000000e-01f, 5.21095276e-01f},
    {1.00000000e+00f, 1.17520118e+00f},
    {1.00000000e+01f, 1.10132324e+04f},
    {2.00000000e+01f, 2.42582592e+08f},
    {1.40129846e-45f, 1.40129846e-45f},
    {-1.40129846e-45f, -1.40129846e-45f},
    {0.00000000e+00f, 0.00000000e+00f},
    {-0.00000000e+00f, -0.00000000e+00f},
    {3.14159274e+00f, 1.15487394e+01f},
    {-3.14159274e+00f, -1.15487394e+01f},
    {2.71828175e+00f, 7.54413700e+00f},
    {-2.71828175e+00f, -7.54413700e+00f},
    {8.00000000e+01f, 2.77031124e+34f},
    {8.50000000e+01f, 4.11150651e+36f},
    {8.80000000e+01f, 8.25818133e+37f},
    {8.87226028e+01f, INFINITY},
    {8.90000000e+01f, INFINITY},
    {1.00000000e+02f, INFINITY},
    {INFINITY, INFINITY},
    {-INFINITY, -INFINITY},
    {NAN, NAN},
    {1.00000000e-30f, 1.00000000e-30f},
    {-1.00000000e-30f, -1.00000000e-30f},
    {9.99999968e-21f, 9.99999968e-21f},
    {-9.99999968e-21f, -9.99999968e-21f},
    {9.39619064e+01f, INFINITY},
    {-8.19467545e+01f, -1.94085606e+35f},
    {7.88125305e+01f, 8.44923416e+33f},
    {7.70193710e+01f, 1.40622948e+33f},
    {9.55179062e+01f, INFINITY},
    {3.30971031e+01f, 1.18265968e+14f},
    {2.07294502e+01f, 5.03101376e+08f},
    {-9.04855194e+01f, -INFINITY},
    {1.61077137e+01f, 4.94835500e+06f},
    {-9.48603516e+01f, -INFINITY},
    {3.40282347e+38f, INFINITY},
    {-3.40282347e+38f, -INFINITY},
};

float my_sinhf(float x)
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
  if (fabsf(x) < 1.0f) {
    float x2 = x * x;
    return (((x2 + 42.0f) * x2 + 840.0f) * x2 + 5040.0f) * x / 5040.0f;
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
  return (expf(x) - expf(-x)) / 2.0f;
}

// Function to compute ULP difference between two floats
int32_t ulp_diff(float a, float b)
{
  if (isnan(a) || isnan(b) || isinf(a) || isinf(b) ||
      signbit(a) != signbit(b)) {
    return -1;
  }
  int32_t ia, ib;
  memcpy(&ia, &a, sizeof(float));
  memcpy(&ib, &b, sizeof(float));
  if (a < 0) {
    ia = 0x80000000 - ia;
    ib = 0x80000000 - ib;
  }
  return abs(ia - ib);
}

int main()
{
  int n_tests = sizeof(test_cases) / sizeof(test_cases[0]);
  int passes = 0;
  float tolerance = 10.0f * FLT_EPSILON;

  // Print header row
  printf("%14s %14s %14s %14s %4s %8s\n", "Input", "Expected", "Actual", "Diff",
         "Pass", "ULP");
  printf("%14s %14s %14s %14s %4s %8s\n", "-------------", "-------------",
         "-------------", "-------------", "----", "--------");

  for (int i = 0; i < n_tests; ++i) {
    float x = test_cases[i].input;
    float expected = test_cases[i].expected;
    float actual = sinhf(x);
//  float actual = my_sinhf(x);
    int pass;
    if (isnan(expected)) {
      pass = isnan(actual);
    } else if (isinf(expected)) {
      pass = isinf(actual) && (signbit(expected) == signbit(actual));
    } else {
      float diff = fabsf(actual - expected);
      pass = (diff <= tolerance * fabsf(expected)) ||
             (diff <= FLT_MIN); // Relative or absolute tolerance
    }
    passes += pass;
    float diff =
        (isnan(expected) || isnan(actual) || isinf(expected) || isinf(actual))
            ? 0.0f
            : fabsf(actual - expected);
    int32_t ulp = ulp_diff(actual, expected);
    printf("%14e %14e %14e %14e %4s %8d\n", x, expected, actual, diff,
           pass ? "YES" : "NO", ulp);
  }

  printf("\nPass rate: %d/%d (%.2f%%)\n", passes, n_tests,
         100.0f * passes / n_tests);
  return 0;
}
