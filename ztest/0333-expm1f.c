#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float input;
  float expected;
} Expm1fTestPattern;

Expm1fTestPattern patterns[] = {
    {0.00000000e+00f, 0.00000000e+00f},  // zero
    {-0.00000000e+00f, -0.00000000e+00f},  // negative zero
    {1.40129846e-45f, 1.40129846e-45f},  // smallest subnormal
    {1.17549421e-38f, 1.17549421e-38f},  // largest subnormal
    {1.17549435e-38f, 1.17549435e-38f},  // smallest normal
    {-1.40129846e-45f, -1.40129846e-45f},  // negative subnormal
    {-1.17549435e-38f, -1.17549435e-38f},  // negative smallest normal
    {9.31322575e-10f, 9.31322575e-10f},
    {-9.31322575e-10f, -9.31322575e-10f},
    {5.96046448e-08f, 5.96046448e-08f},
    {-5.96046448e-08f, -5.96046448e-08f},
    {9.53674316e-07f, 9.53674771e-07f},
    {-9.53674316e-07f, -9.53673862e-07f},
    {2.44140625e-04f, 2.44170427e-04f},
    {-2.44140625e-04f, -2.44110823e-04f},
    {1.00000005e-03f, 1.00050017e-03f},
    {-1.00000005e-03f, -9.99500160e-04f},
    {9.99999978e-03f, 1.00501664e-02f},
    {-9.99999978e-03f, -9.95016564e-03f},
    {5.00000007e-02f, 5.12710959e-02f},
    {-5.00000007e-02f, -4.87705767e-02f},
    {1.00000001e-01f, 1.05170920e-01f},
    {-1.00000001e-01f, -9.51625854e-02f},
    {2.00000003e-01f, 2.21402764e-01f},
    {-2.00000003e-01f, -1.81269243e-01f},
    {3.00000012e-01f, 3.49858820e-01f},
    {-3.00000012e-01f, -2.59181798e-01f},
    {3.40000004e-01f, 4.04947609e-01f},
    {-3.40000004e-01f, -2.88229674e-01f},
    {3.46500009e-01f, 4.14109498e-01f},
    {-3.46500009e-01f, -2.92841196e-01f},
    {3.46573591e-01f, 4.14213568e-01f},
    {-3.46573591e-01f, -2.92893231e-01f},
    {3.46573532e-01f, 4.14213479e-01f},
    {-3.46573532e-01f, -2.92893171e-01f},
    {3.46573561e-01f, 4.14213508e-01f},
    {-3.46573561e-01f, -2.92893201e-01f},
    {3.46573621e-01f, 4.14213598e-01f},
    {-3.46573621e-01f, -2.92893231e-01f},
    {3.46573651e-01f, 4.14213657e-01f},
    {-3.46573651e-01f, -2.92893261e-01f},
    {3.46783996e-01f, 4.14511144e-01f},  // worst positive
    {-3.47296119e-01f, -2.93403953e-01f},  // worst negative
    {1.69844551e+01f, 2.37823700e+07f},  // worst large
    {-2.00906372e+00f, -8.65885794e-01f},  // worst n<=0
    {3.60000014e-01f, 4.33329433e-01f},
    {-3.60000014e-01f, -3.02323669e-01f},
    {4.00000006e-01f, 4.91824716e-01f},
    {-4.00000006e-01f, -3.29679966e-01f},
    {5.00000000e-01f, 6.48721278e-01f},
    {-5.00000000e-01f, -3.93469334e-01f},
    {6.00000024e-01f, 8.22118819e-01f},
    {-6.00000024e-01f, -4.51188385e-01f},
    {6.89999998e-01f, 9.93715525e-01f},
    {-6.89999998e-01f, -4.98423934e-01f},
    {6.93147182e-01f, 1.00000000e+00f},
    {-6.93147182e-01f, -5.00000000e-01f},
    {6.99999988e-01f, 1.01375270e+00f},
    {-6.99999988e-01f, -5.03414690e-01f},
    {8.00000012e-01f, 1.22554100e+00f},
    {-8.00000012e-01f, -5.50671041e-01f},
    {1.00000000e+00f, 1.71828187e+00f},
    {-1.00000000e+00f, -6.32120550e-01f},
    {1.50000000e+00f, 3.48168898e+00f},
    {-1.50000000e+00f, -7.76869833e-01f},
    {2.00000000e+00f, 6.38905621e+00f},
    {-2.00000000e+00f, -8.64664733e-01f},
    {3.00000000e+00f, 1.90855370e+01f},
    {-3.00000000e+00f, -9.50212955e-01f},
    {5.00000000e+00f, 1.47413162e+02f},
    {-5.00000000e+00f, -9.93262053e-01f},
    {1.00000000e+01f, 2.20254648e+04f},
    {-1.00000000e+01f, -9.99954581e-01f},
    {1.60000000e+01f, 8.88611000e+06f},
    {-1.60000000e+01f, -9.99999881e-01f},
    {1.70000000e+01f, 2.41549520e+07f},
    {-1.70000000e+01f, -9.99999940e-01f},
    {2.00000000e+01f, 4.85165184e+08f},
    {-2.00000000e+01f, -1.00000000e+00f},
    {3.00000000e+01f, 1.06864742e+13f},
    {-3.00000000e+01f, -1.00000000e+00f},
    {5.00000000e+01f, 5.18470546e+21f},
    {-5.00000000e+01f, -1.00000000e+00f},
    {8.00000000e+01f, 5.54062248e+34f},
    {-8.00000000e+01f, -1.00000000e+00f},
    {1.59423847e+01f, 8.38860350e+06f},  // n boundary
    {1.66355324e+01f, 1.67772160e+07f},  // n boundary
    {1.69821053e+01f, 2.37265500e+07f},  // n boundary
    {1.73286800e+01f, 3.35544480e+07f},  // n boundary
    {1.80218258e+01f, 6.71088040e+07f},  // n boundary
    {-1.73286839e+01f, -1.00000000e+00f},
    {-1.73286819e+01f, -1.00000000e+00f},
    {-1.73286800e+01f, -1.00000000e+00f},  // -1 threshold
    {-1.73286781e+01f, -9.99999940e-01f},
    {-1.73286762e+01f, -9.99999940e-01f},
    {-1.65000000e+01f, -9.99999940e-01f},
    {-1.80000000e+01f, -1.00000000e+00f},
    {-4.00000000e+01f, -1.00000000e+00f},
    {-9.00000000e+01f, -1.00000000e+00f},
    {8.87228241e+01f, 3.40277256e+38f},
    {8.87228317e+01f, 3.40279852e+38f},
    {8.87228394e+01f, INFINITY},  // overflow threshold
    {8.87228470e+01f, INFINITY},
    {8.87228546e+01f, INFINITY},
    {8.80000000e+01f, 1.65163627e+38f},
    {8.86999969e+01f, 3.32597686e+38f},
    {8.90000000e+01f, INFINITY},
    {3.40282347e+38f, INFINITY},  // FLT_MAX
    {INFINITY, INFINITY},
    {-INFINITY, -1.00000000e+00f},
    {NAN, NAN},
};

int ulp_diff(float a, float b)
{
  union {
    float f;
    int32_t i;
  } ua = {a}, ub = {b};
  if (ua.i < 0) {
    ua.i = 0x80000000 - ua.i;
  }
  if (ub.i < 0) {
    ub.i = 0x80000000 - ub.i;
  }
  return abs(ua.i - ub.i);
}

int max_ulp_tolerance(float x)
{
  (void)x;
  return 2;
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main(int argc, char **argv)
{
  int n_diff = 0;
  int n_fail = 0;
  int total = (int)ARRAY_SIZE(patterns);

  printf("%-12s %-12s %-12s %-8s %-8s\n", "input", "expected", "got", "ULP",
         "result");

  for (size_t i = 0; i < ARRAY_SIZE(patterns); ++i) {
    float result = expm1f(patterns[i].input);
    float expected = patterns[i].expected;
    int ulp = ulp_diff(result, expected);
    int max_ulp = max_ulp_tolerance(fabsf(expected));

    const char *res_msg = "PASS";
    if (expected == 0.0f) {
      if (result != 0.0f || signbit(expected) != signbit(result)) {
        res_msg = "FAIL";
        n_fail++;
      }
    } else if (isnan(expected)) {
      if (!isnan(result)) {
        res_msg = "FAIL";
        n_fail++;
      }
    } else if (isinf(expected)) {
      if (!isinf(result) || (signbit(expected) != signbit(result))) {
        res_msg = "FAIL";
        n_fail++;
      }
    } else if (ulp > max_ulp) {
      res_msg = "DIFF";
      n_diff++;
    }

    printf("%-12e %-12e %-12e %-8d %-8s\n", patterns[i].input, expected, result,
           ulp, res_msg);
  }

  printf("\nTotal tests: %d\n", total);
  if (n_diff == 0 && n_fail == 0) {
    printf("All tests passed.\n");
  } else {
    printf("%d tests differ, %d tests failed.\n", n_diff, n_fail);
  }

  return n_fail != 0 || n_diff != 0;
}
