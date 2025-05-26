#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "my_printf.c"

// Struct for test case
typedef struct {
  float input;
  float expected;
} ExpfTestCase;

// Subnormal values
#define SUBNORMAL1 1.40129846e-45f // Smallest positive subnormal float
#define SUBNORMAL2 1.0e-40f        // Example subnormal

// Edge values for float expf
#define EXPF_OVERFLOW_EDGE 88.7228317261f // Max x where expf(x) == FLT_MAX
#define EXPF_OVERFLOW_EDGE_OVER                                                \
  88.722839f // Slightly over the edge (will overflow)
#define EXPF_UNDERFLOW_EDGE                                                    \
  -103.9720764160f // Min x where expf(x) == FLT_TRUE_MIN
#define EXPF_UNDERFLOW_EDGE_UNDER                                              \
  -103.972084f // Slightly under the edge (will underflow)

// Test cases (comprehensive)
const ExpfTestCase test_cases[] = {
    {8.87228317e+01f, 3.40279854e+38f},
    {8.87228390e+01f, 3.40282329e+38f},
    {-1.03972076e+02f, 7.00649700e-46f},
    {-1.03972084e+02f, 7.00644386e-46f},
    {1.40129846e-45f, 1.00000000e+00f},
    {-1.40129846e-45f, 1.00000000e+00f},
    {1.00000000e-40f, 1.00000000e+00f},
    {-1.00000000e-40f, 1.00000000e+00f},
    {5.00000000e-01f, 1.64872127e+00f},
    {-5.00000000e-01f, 6.06530660e-01f},
    {2.50000000e-01f, 1.28402542e+00f},
    {-2.50000000e-01f, 7.78800783e-01f},
    {7.50000000e-01f, 2.11700002e+00f},
    {-7.50000000e-01f, 4.72366553e-01f},
    {1.50000000e+00f, 4.48168907e+00f},
    {-1.50000000e+00f, 2.23130160e-01f},
    {2.50000000e+00f, 1.21824940e+01f},
    {-2.50000000e+00f, 8.20849986e-02f},
    {3.50000000e+00f, 3.31154520e+01f},
    {-3.50000000e+00f, 3.01973834e-02f},
    {4.50000000e+00f, 9.00171313e+01f},
    {-4.50000000e+00f, 1.11089965e-02f},
    {4.50000000e+00f, 9.00171313e+01f},
    {6.27000000e+00f, 5.28477378e+02f},
    {8.73000000e+00f, 6.18572811e+03f},
    {1.21600000e+01f, 1.90994517e+05f},
    {1.69300000e+01f, 2.25219287e+07f},
    {2.35800000e+01f, 1.74045935e+10f},
    {3.28400000e+01f, 1.82907193e+14f},
    {4.57300000e+01f, 7.24914284e+19f},
    {6.36900000e+01f, 4.57315111e+27f},
    {8.80000000e+01f, 1.65163625e+38f},
    {8.87000000e+01f, 3.32598698e+38f},
    {1.00000000e-01f, 1.10517092e+00f},
    {2.00000000e-01f, 1.22140276e+00f},
    {3.00000000e-01f, 1.34985881e+00f},
    {4.00000000e-01f, 1.49182470e+00f},
    {6.00000000e-01f, 1.82211880e+00f},
    {7.00000000e-01f, 2.01375271e+00f},
    {8.00000000e-01f, 2.22554093e+00f},
    {9.00000000e-01f, 2.45960311e+00f},
    {-1.00000000e-01f, 9.04837418e-01f},
    {-2.00000000e-01f, 8.18730753e-01f},
    {-3.00000000e-01f, 7.40818221e-01f},
    {-4.00000000e-01f, 6.70320046e-01f},
    {-6.00000000e-01f, 5.48811636e-01f},
    {-7.00000000e-01f, 4.96585304e-01f},
    {-8.00000000e-01f, 4.49328964e-01f},
    {-9.00000000e-01f, 4.06569660e-01f},
    {3.14159265e+00f, 2.31406926e+01f},
    {2.71828183e+00f, 1.51542622e+01f},
    {1.44269504e+00f, 4.23208611e+00f},
    {4.34294482e-01f, 1.54387344e+00f},
    {-3.14159265e+00f, 4.32139183e-02f},
    {-2.71828183e+00f, 6.59880358e-02f},
    {INFINITY, INFINITY},
    {-INFINITY, 0.0f},
    {NAN, NAN},
};

#define EPSILON 1e-6f

// Compare two floats, including NaN and Inf
int float_equal(float a, float b)
{
  if (isnan(a) && isnan(b)) {
    return 1;
  }
  if (isinf(a) && isinf(b) && (a > 0) == (b > 0)) {
    return 1;
  }
  float diff = fabsf(a - b);
  float rel = EPSILON * fabsf(b);
  if (diff <= rel || diff <= EPSILON) {
    return 1;
  }
  return 0;
}

// Calculate ULP difference between two floats
int ulp_diff(float a, float b)
{
  unsigned int a_bits, b_bits;
  memcpy(&a_bits, &a, sizeof(float));
  memcpy(&b_bits, &b, sizeof(float));
  // Convert to signed int representation
  if (a_bits & 0x80000000) {
    a_bits = 0x80000000 - (a_bits & 0x7FFFFFFF);
  }
  if (b_bits & 0x80000000) {
    b_bits = 0x80000000 - (b_bits & 0x7FFFFFFF);
  }
  return (a_bits > b_bits) ? (a_bits - b_bits) : (b_bits - a_bits);
}

// 9th order Taylor expansion coefficients (precomputed)
#define C0 1.00000000e+00f
#define C1 1.00000000e+00f
#define C2 5.00000000e-01f
#define C3 1.66666667e-01f  // 1/6
#define C4 4.16666667e-02f  // 1/24
#define C5 8.33333333e-03f  // 1/120
#define C6 1.38888889e-03f  // 1/720
#define C7 1.98412698e-04f  // 1/5040
#define C8 2.48015873e-05f  // 1/40320
#define C9 2.75573192e-06f  // 1/362880

// expf implementation using 7th order Taylor expansion (Horner's method)
float myexpf(float x)
{
  // Handle special cases
  if (isnan(x)) {
    return x;
  }
  if (x == 0.0f) {
    return 1.0f;
  }
  if (x == INFINITY) {
    return INFINITY;
  }
  if (x == -INFINITY) {
    return 0.0f;
  }
  if (x > EXPF_OVERFLOW_EDGE_OVER) {
    return INFINITY;
  }
  if (x < EXPF_UNDERFLOW_EDGE_UNDER) {
    return 0.0f;
  }
#if 0
  if (x > 4.0f) {
    float y = x / 4.0f;
    float exp_y = myexpf(y);
    exp_y = exp_y * exp_y;
    return exp_y * exp_y;
  }
#endif
  // Range reduction using fmodf
  int n = (int)(x / M_LN2);
  float r = fmodf(x, M_LN2);
  if (r > 0.5f * M_LN2) {
    r -= M_LN2;
    n += 1;
  } else if (r < -0.5f * M_LN2) {
    r += M_LN2;
    n -= 1;
  }

  // 7th order Taylor expansion using Horner's method
//  float exp_r = C0 + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));
//  float exp_r = C0 + r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * (C7 + r * (C8 + r * C9))))))));
float exp_r = ((((((((C9 * r + C8) * r + C7) * r + C6) * r + C5) * r + C4) * r + C3) * r + C2) * r  + C1) * r + C0;


  return ldexpf(exp_r, n);
}

// Test function for expf
int test_expf(float (*my_expf)(float))
{
  int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  int failed = 0;
  printf("%13s %13s %13s %13s %6s %8s\n", "input", "expected", "got", "diff",
         "result", "ULP");
  for (int i = 0; i < num_cases; ++i) {
    float input = test_cases[i].input;
    float expected = test_cases[i].expected;
    float result = expf(input);
//  float result = my_expf(input);

    int ok = float_equal(result, expected);

    float diff = (isnan(expected) && isnan(result)) ||
                         (isinf(expected) && isinf(result) &&
                          (expected > 0) == (result > 0))
                     ? 0.0f
                     : fabsf(result - expected);

    int ulp = ok ? 0 : ulp_diff(result, expected);

    printf("%13.6e %13.6e %13.6e %13.6e %6s %8d\n", input, expected, result,
           diff, ok ? "Pass" : "Fail", ulp);

    if (!ok) {
      failed++;
    }
  }
  if (!failed) {
    printf("All tests passed! %d case\n", num_cases);
  } else {
    printf("failed %d/%d (%.2f%%)\n", failed, num_cases,
           failed * 100.0 / num_cases);
  }

  return failed;
}

int main(void)
{
  // Run test
  return test_expf(myexpf);
}
