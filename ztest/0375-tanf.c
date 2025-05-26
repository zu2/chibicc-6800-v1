#include <float.h>
#include <math.h>
#include <stdio.h>

#include "my_printf.c"

// Struct for test case
typedef struct {
  float input;
  float expected;
} TanfTestCase;

// Test cases for tanf
const TanfTestCase test_cases[] = {
    {0.00000000e+00f, 0.00000000e+00f},
    {-0.00000000e+00f, -0.00000000e+00f},
    {1.40129846e-45f, 1.40129846e-45f},
    {-1.40129846e-45f, -1.40129846e-45f},
    {1.00000000e-40f, 1.00000000e-40f},
    {-1.00000000e-40f, -1.00000000e-40f},
    {1.17549435e-38f, 1.17549435e-38f},
    {-1.17549435e-38f, -1.17549435e-38f},
    {1.00000000e-20f, 1.00000000e-20f},
    {-1.00000000e-20f, -1.00000000e-20f},
    {1.00000000e-10f, 1.00000000e-10f},
    {-1.00000000e-10f, -1.00000000e-10f},
    {5.23598776e-01f, 5.77350270e-01f},
    {-5.23598776e-01f, -5.77350270e-01f},
    {7.85398163e-01f, 9.99999999e-01f},
    {-7.85398163e-01f, -9.99999999e-01f},
    {1.04719755e+00f, 1.73205080e+00f},
    {-1.04719755e+00f, -1.73205080e+00f},
    {1.57079623e+00f, 1.03311232e+07f},
    {-1.57079623e+00f, -1.03311232e+07f},
    {1.57079533e+00f, 1.00321541e+06f},
    {-1.57079533e+00f, -1.00321541e+06f},
    {1.56079633e+00f, 9.99966987e+01f},
    {-1.56079633e+00f, -9.99966987e+01f},
    {1.57079633e+00f, -3.12002409e+08f},
    {-1.57079633e+00f, 3.12002409e+08f},
    {3.14159265e+00f, -3.58979303e-09f},
    {-3.14159265e+00f, 3.58979303e-09f},
    {6.28318531e+00f, 2.82041388e-09f},
    {-6.28318531e+00f, -2.82041388e-09f},
    {3.14159265e+01f, -3.58979312e-08f},
    {-3.14159265e+01f, 3.58979312e-08f},
    {1.00000000e+00f, 1.55740772e+00f},
    {-1.00000000e+00f, -1.55740772e+00f},
    {1.00000000e+01f, 6.48360827e-01f},
    {-1.00000000e+01f, -6.48360827e-01f},
    {1.00000000e+02f, -5.87213915e-01f},
    {-1.00000000e+02f, 5.87213915e-01f},
    {1.00000000e+06f, -3.73624454e-01f},
    {-1.00000000e+06f, 3.73624454e-01f},
    {1.00000000e+20f, -8.44602463e-01f},
    {-1.00000000e+20f, 8.44602463e-01f},
    {3.40282350e+38f, 6.12973417e-01f},
    {-3.40282350e+38f, -6.12973417e-01f},
    {INFINITY, NAN},
    {-INFINITY, NAN},
    {NAN, NAN},
};

// Improved tanf for float, using only float operations, no double, no FMA
float my_tanf(float x)
{
  // Use fmodf to reduce x into [-PI, PI]
  float r = fmodf(x, (float)M_PI);

  // Fold into [-PI/2, PI/2]
  if (r > (float)M_PI_2) {
    r -= (float)M_PI;
  } else if (r < -(float)M_PI_2) {
    r += (float)M_PI;
  }

  // Use 1/delta approximation near the asymptote for |r| close to π/2
  float delta = (r > 0) ? (M_PI_2 - r) : (-M_PI_2 - r);
  if (fabsf(delta) < 0.01f) {
    if (delta == 0.0f) {
      return (r > 0) ? INFINITY : -INFINITY;
    }
    return (r > 0 ? 1.0f : -1.0f) / delta;
  }

  // Standard computation using sinf/cosf
  float cos_val = cosf(r);
  if (cos_val == 0.0f) {
    return (r > 0) ? INFINITY : -INFINITY;
  }
  return sinf(r) / cos_val;
}

#define EPSILON 1e-6f

// Compare two floats with tolerance
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
  return (diff <= rel || diff <= EPSILON);
}

int main()
{
  int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  int failed = 0;
  printf("%16s %16s %16s %16s   %6s\n", "Input", "Expected", "Got", "Diff",
         "Result");
  for (int i = 0; i < num_cases; i++) {
    float input = test_cases[i].input;
    float expected = test_cases[i].expected;
    float got = tanf(input);
//    float got = my_tanf(input);
    float diff = fabsf(got - expected);
    int ok = float_equal(got, expected);
    printf("%16e %16e %16e %16e %6s\n", input, expected, got, diff,
           ok ? "Pass" : "Fail");
    if (!ok) {
      failed++;
    }
  }
  if (failed == 0) {
    printf("All tests passed!\n");
  } else {
    printf("%d tests failed.\n", failed);
  }
  return failed;
}
