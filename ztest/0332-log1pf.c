#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float input;
  float expected;
} Log1pfTestPattern;

Log1pfTestPattern patterns[] = {
    {0.00000000e+00f, 0.00000000e+00f},  // zero
    {-0.00000000e+00f, -0.00000000e+00f},  // negative zero
    {1.40129846e-45f, 1.40129846e-45f},  // smallest subnormal
    {1.17549421e-38f, 1.17549421e-38f},  // largest subnormal
    {1.17549435e-38f, 1.17549435e-38f},  // smallest normal
    {-1.40129846e-45f, -1.40129846e-45f},  // negative subnormal
    {-1.17549435e-38f, -1.17549435e-38f},  // negative smallest normal
    {9.99999972e-10f, 9.99999972e-10f},  // u == 1
    {-9.99999972e-10f, -9.99999972e-10f},
    {5.96046448e-08f, 5.96046448e-08f},  // 2^-24
    {1.19209290e-07f, 1.19209282e-07f},  // 2^-23
    {9.68575051e-08f, 9.68574980e-08f},
    {2.12341533e-07f, 2.12341504e-07f},
    {3.02989662e-07f, 3.02989605e-07f},
    {5.39421535e-07f, 5.39421364e-07f},
    {1.24646030e-01f, 1.17468350e-01f},
    {2.46172085e-01f, 2.20076516e-01f},
    {4.71859574e-01f, 3.86526614e-01f},
    {5.92746913e-01f, 4.65460151e-01f},
    {1.70861447e+00f, 9.96437252e-01f},
    {2.02820802e+00f, 1.10797107e+00f},
    {4.95332003e+00f, 1.78394902e+00f},
    {1.08325300e+01f, 2.47085261e+00f},
    {2.28233585e+01f, 3.17066646e+00f},
    {4.62253456e+01f, 3.85493064e+00f},
    {9.04900589e+01f, 4.51623011e+00f},
    {1.90230637e+02f, 5.25348043e+00f},
    {3.67363678e+02f, 5.90907049e+00f},
    {7.62326965e+02f, 6.63768625e+00f},
    {1.50632825e+03f, 7.31809378e+00f},
    {-9.68575051e-08f, -9.68575122e-08f},
    {-2.12341533e-07f, -2.12341561e-07f},
    {-1.16878539e-01f, -1.24292530e-01f},
    {-2.16177434e-01f, -2.43572608e-01f},
    {-2.61994064e-01f, -3.03803414e-01f},
    {-6.13038957e-01f, -9.49431241e-01f},
    {4.97526795e-01f, 4.03814942e-01f},  // worst overall
    {-2.53525555e-01f, -2.92393893e-01f},  // worst negative
    {5.00000000e-01f, 4.05465096e-01f},  // u = 1.5, m boundary
    {4.99999970e-01f, 4.05465096e-01f},
    {5.00000060e-01f, 4.05465156e-01f},
    {-2.50000000e-01f, -2.87682086e-01f},  // u = 0.75, m boundary
    {-2.49999985e-01f, -2.87682056e-01f},
    {-2.50000030e-01f, -2.87682116e-01f},
    {1.00000000e+00f, 6.93147182e-01f},  // c branch boundary
    {9.99999940e-01f, 6.93147123e-01f},
    {1.00000012e+00f, 6.93147242e-01f},
    {3.00000000e+00f, 1.38629436e+00f},  // u = 4, f = 0
    {7.00000000e+00f, 2.07944155e+00f},
    {-5.00000000e-01f, -6.93147182e-01f},  // u = 0.5
    {-7.50000000e-01f, -1.38629436e+00f},  // u = 0.25
    {-8.75000000e-01f, -2.07944155e+00f},
    {1.02400000e+03f, 6.93244791e+00f},  // c == 0 from here
    {1.02399994e+03f, 6.93244791e+00f},
    {1.67772160e+07f, 1.66355324e+01f},  // 2^24, c == 1
    {1.67772150e+07f, 1.66355324e+01f},
    {1.00000000e+06f, 1.38155117e+01f},
    {1.00000002e+20f, 4.60517006e+01f},
    {3.40282347e+38f, 8.87228394e+01f},  // FLT_MAX
    {-8.99999976e-01f, -2.30258489e+00f},
    {-9.99999881e-01f, -1.59423847e+01f},
    {-9.99999940e-01f, -1.66355324e+01f},  // nextafter(-1, 0)
    {-5.96046448e-08f, -5.96046448e-08f},
    {5.96046519e-08f, 5.96046519e-08f},
    {-5.96046519e-08f, -5.96046519e-08f},
    {8.94069601e-08f, 8.94069530e-08f},
    {-8.94069601e-08f, -8.94069672e-08f},
    {8.94069672e-08f, 8.94069601e-08f},
    {-8.94069672e-08f, -8.94069743e-08f},
    {8.94069743e-08f, 8.94069672e-08f},
    {-8.94069743e-08f, -8.94069814e-08f},
    {9.93410723e-08f, 9.93410652e-08f},
    {-9.93410723e-08f, -9.93410794e-08f},
    {7.94728550e-08f, 7.94728550e-08f},
    {-7.94728550e-08f, -7.94728550e-08f},
    {1.19209282e-07f, 1.19209275e-07f},
    {-1.19209282e-07f, -1.19209290e-07f},
    {-5.00000060e-01f, -6.93147302e-01f},
    {7.49999940e-01f, 5.59615731e-01f},
    {-7.49999940e-01f, -1.38629413e+00f},
    {7.50000000e-01f, 5.59615791e-01f},
    {7.50000060e-01f, 5.59615850e-01f},
    {-7.50000060e-01f, -1.38629460e+00f},
    {8.33333313e-01f, 6.06135786e-01f},
    {-8.33333313e-01f, -1.79175937e+00f},
    {6.66666627e-01f, 5.10825574e-01f},
    {-6.66666627e-01f, -1.09861219e+00f},
    {1.49999988e+00f, 9.16290700e-01f},
    {1.50000000e+00f, 9.16290760e-01f},
    {1.50000012e+00f, 9.16290760e-01f},
    {1.66666663e+00f, 9.80829239e-01f},
    {1.33333325e+00f, 8.47297847e-01f},
    {1.99999988e+00f, 1.09861231e+00f},
    {8.00000000e+00f, 2.19722462e+00f},
    {8.00000095e+00f, 2.19722462e+00f},
    {1.19999990e+01f, 2.56494927e+00f},
    {1.20000000e+01f, 2.56494927e+00f},
    {1.20000010e+01f, 2.56494951e+00f},
    {1.33333330e+01f, 2.66258788e+00f},
    {1.06666660e+01f, 2.45673561e+00f},
    {1.59999990e+01f, 2.83321333e+00f},
    {5.28607905e-01f, 4.24357444e-01f},  // 1+x is an exact tie
    {2.00000024e+00f, 1.09861243e+00f},  // 1+x exact, low bit set
    {4.00000048e+00f, 1.60943806e+00f},
    {-1.00000000e+00f, -INFINITY},
    {-1.00000012e+00f, NAN},
    {-2.00000000e+00f, NAN},
    {-3.40282347e+38f, NAN},
    {INFINITY, INFINITY},
    {-INFINITY, NAN},
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
  return 1;
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
    float result = log1pf(patterns[i].input);
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
