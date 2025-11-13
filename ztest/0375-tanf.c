#include <float.h>
#include <math.h>
#include <stdio.h>
#include "common.h"
#include "my_printf.c"

// Struct for test case
typedef struct {
  float input;
  float expected;
} TanfTestCase;

// Test cases for tanf
const TanfTestCase test_cases[] = {
    {0.00000000e+00f, 0.00000000e+00f}, /* 1 */
    {-0.00000000e+00f, -0.00000000e+00f}, /* 2 */
    {1.40129846e-45f, 1.40129846e-45f}, /* 3 */
    {-1.40129846e-45f, -1.40129846e-45f}, /* 4 */
    {1.00000000e-40f, 1.00000000e-40f}, /* 5 */
    {-1.00000000e-40f, -1.00000000e-40f}, /* 6 */
    {1.17549435e-38f, 1.17549435e-38f}, /* 7 */
    {-1.17549435e-38f, -1.17549435e-38f}, /* 8 */
    {1.00000000e-20f, 1.00000000e-20f}, /* 9 */
    {-1.00000000e-20f, -1.00000000e-20f}, /* 10 */
    {1.00000000e-10f, 1.00000000e-10f}, /* 11 */
    {-1.00000000e-10f, -1.00000000e-10f}, /* 12 */
    {1.10000000e+00f, 1.96475966e+00f}, /* 13 */
    {1.20000000e+00f, 2.57215162e+00f}, /* 14 */
    {1.40000000e+00f, 5.79788372e+00f}, /* 15 */
    {1.80000000e+00f, -4.28626167e+00f}, /* 16 */
    {2.00000000e+00f, -2.18503986e+00f}, /* 17 */
    {2.10000000e+00f, -1.70984654e+00f}, /* 18 */
    {2.20000000e+00f, -1.37382306e+00f}, /* 19 */
    {2.40000000e+00f, -9.16014290e-01f}, /* 20 */
    {2.80000000e+00f, -3.55529832e-01f}, /* 21 */
    {3.00000000e+00f, -1.42546543e-01f}, /* 22 */
    {3.10000000e+00f, -4.16166546e-02f}, /* 23 */
    {3.20000000e+00f, 5.84738545e-02f}, /* 24 */
    {3.40000000e+00f, 2.64316901e-01f}, /* 25 */
    {3.80000000e+00f, 7.73556091e-01f}, /* 26 */
    {4.00000000e+00f, 1.15782128e+00f}, /* 27 */
    {1.01000000e+01f, 8.00789303e-01f}, /* 28 */
    {1.02000000e+01f, 9.79852084e-01f}, /* 29 */
    {1.04000000e+01f, 1.47566791e+00f}, /* 30 */
    {1.08000000e+01f, 5.04778831e+00f}, /* 31 */
    {2.00000000e+01f, 2.23716094e+00f}, /* 32 */
    {2.01000000e+01f, 3.01404197e+00f}, /* 33 */
    {2.02000000e+01f, 4.46449871e+00f}, /* 34 */
    {2.04000000e+01f, 4.91278363e+01f}, /* 35 */
    {2.08000000e+01f, -2.50623843e+00f}, /* 36 */
    {3.00000000e+01f, -6.40533120e+00f}, /* 37 */
    {3.01000000e+01f, -3.83824530e+00f}, /* 38 */
    {3.02000000e+01f, -2.69863989e+00f}, /* 39 */
    {3.04000000e+01f, -1.61335687e+00f}, /* 40 */
    {3.08000000e+01f, -7.07777234e-01f}, /* 41 */
    {4.00000000e+01f, -1.11721493e+00f}, /* 42 */
    {5.23598776e-01f, 5.77350270e-01f}, /* 43 */
    {-5.23598776e-01f, -5.77350270e-01f}, /* 44 */
    {7.85398163e-01f, 9.99999999e-01f}, /* 45 */
    {-7.85398163e-01f, -9.99999999e-01f}, /* 46 */
    {1.04719755e+00f, 1.73205080e+00f}, /* 47 */
    {-1.04719755e+00f, -1.73205080e+00f}, /* 48 */
    {1.57079623e+00f, 1.03311232e+07f}, /* 49 */
    {-1.57079623e+00f, -1.03311232e+07f}, /* 50 */
    {1.57079533e+00f, 1.00321541e+06f}, /* 51 */
    {-1.57079533e+00f, -1.00321541e+06f}, /* 52 */
    {1.56079633e+00f, 9.99966987e+01f}, /* 53 */
    {-1.56079633e+00f, -9.99966987e+01f}, /* 54 */
    {1.57079633e+00f, -3.12002409e+08f}, /* 55 */
    {-1.57079633e+00f, 3.12002409e+08f}, /* 56 */
    {3.14159265e+00f, -3.58979303e-09f}, /* 57 */
    {-3.14159265e+00f, 3.58979303e-09f}, /* 58 */
    {6.28318531e+00f, 2.82041388e-09f}, /* 59 */
    {-6.28318531e+00f, -2.82041388e-09f}, /* 60 */
    {3.14159265e+01f, -3.58979312e-08f}, /* 61 */
    {-3.14159265e+01f, 3.58979312e-08f}, /* 62 */
    {1.00000000e+00f, 1.55740772e+00f}, /* 63 */
    {-1.00000000e+00f, -1.55740772e+00f}, /* 64 */
    {1.00000000e+01f, 6.48360827e-01f}, /* 65 */
    {-1.00000000e+01f, -6.48360827e-01f}, /* 66 */
    {1.00000000e+02f, -5.87213915e-01f}, /* 67 */
    {-1.00000000e+02f, 5.87213915e-01f}, /* 68 */
    {1.00000000e+06f, -3.73624454e-01f}, /* 69 */
    {-1.00000000e+06f, 3.73624454e-01f}, /* 70 */
    {1.00000000e+20f, -8.44602463e-01f}, /* 71 */
    {-1.00000000e+20f, 8.44602463e-01f}, /* 72 */
    {3.40282350e+38f, 6.12973417e-01f}, /* 73 */
    {-3.40282350e+38f, -6.12973417e-01f}, /* 74 */
    {INFINITY, NAN},
    {-INFINITY, NAN},
    {NAN, NAN},
};


float mytanf(float x)
{
  // Step 0: Use the property of odd function (tanf)
  if (fabsf(x)<0.00006) {
    return x;
  }
  if (x < 0.0) {
    return -mytanf(fabsf(x));
  }
  x = fmodf(x, M_TWOPI);
  if (fmodf(x,M_TWOPI)>M_TWOPI) {
    printf("*** fmodf error\n");
    return 255;
  }
  if (x > M_PI) {
    return -mytanf(M_TWOPI - x);
  }
  if (x > M_PI_2) {
    return -mytanf(M_PI - x);
  }

  float delta = M_PI_2 - x;
  if (delta < 0.01f) {
    if (delta == 0.0f) {
      return copysignf(INFINITY, delta);
    }
    return 1.0f / delta;
  }

  if (x > M_PI_4) {
    return 1.0f / mytanf(M_PI_2 - x);
  }
  if (x > M_PI_8) {
    // tan(x) = (2*tan(x/2)) / (1 - tan(x/2)^2) の公式を使う
    float t = mytanf(x / 2.0f);
    return (2.0f * t) / (1.0f - t * t);
  }

  // return sinf(x)/cosf(x);
  float x2 = x * x;
  return x * (0.99999948f +
              x2 * (0.33337722f + x2 * (0.13232848f + x2 * 0.06236088f)));
}

#define EPSILON 1e-6f

// Compare two floats with tolerance
int float_equal(float a, float b,float ratio)
{
  if (isnan(a) && isnan(b)) {
    return 1;
  }
  if (isinf(a) && isinf(b) && (a > 0) == (b > 0)) {
    return 1;
  }
  float diff = fabsf(a - b);
  float rel = EPSILON * fabsf(b);
  return (diff <= rel || diff <= EPSILON || ratio<=rel);
}

int main()
{
  int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  int failed = 0;
  printf("%12s %12s %12s %12s   %6s\n", "Input", "Expected", "Got", "Diff",
         "Result");
  for (int i = 0; i < num_cases; i++) {
    float input = test_cases[i].input;
    float expected = test_cases[i].expected;
//  float got = mytanf(input);
    float got = tanf(input);
    float diff = fabsf(got - expected);
    int ok = float_equal(got, expected, fabsf(diff/expected));
    printf("%12e %12e %12e %12e %6s %d\n", input, expected, got, diff,
           ok ? "Pass" : "Fail",i+1);
    if (!ok) {
      failed++;
    }
  }
  if (failed == 0) {
    printf("All tests passed!\n");
  } else {
    printf("%d tests failed.\n", failed);
  }
  return (failed>16)?failed:0;
}
