#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "my_printf.c"

typedef struct {
  float input;
  float expected;
} LogfTestPattern;

LogfTestPattern patterns[] = {
    {0.00000000e+00f, -INFINITY},
    {1.40129846e-45f, -1.03278931e+02f},
    {9.99994610e-41f, -9.21034088e+01f},
    {9.99999935e-39f, -8.74982300e+01f},
    {1.17549435e-38f, -8.73365479e+01f},
    {9.99990003e-31f, -6.90775604e+01f},
    {1.00000000e-30f, -6.90775528e+01f},
    {9.99990013e-11f, -2.30258617e+01f},
    {1.00000001e-10f, -2.30258503e+01f},
    {9.99989975e-06f, -1.15129356e+01f},
    {9.99999975e-06f, -1.15129251e+01f},
    {4.99995000e-01f, -6.93157196e-01f},
    {5.00000000e-01f, -6.93147182e-01f},
    {9.99990000e-01f, -1.00000500e-05f},
    {-9.99990000e-01f, NAN},
    {1.00000000e+00f, 0.00000000e+00f},
    {-1.00000000e+00f, NAN},
    {1.00001000e+00f, 9.99995000e-06f},
    {1.99998000e+00f, 6.93137169e-01f},
    {2.00000000e+00f, 6.93147182e-01f},
    {2.71825456e+00f, 9.99989986e-01f},
    {2.71828175e+00f, 9.99999940e-01f},
    {3.14156133e+00f, 1.14471996e+00f},
    {3.14159274e+00f, 1.14472997e+00f},
    {9.99990000e+00f, 2.30257511e+00f},
    {1.00000000e+01f, 2.30258512e+00f},
    {9.99990000e+01f, 4.60516024e+00f},
    {1.00000000e+02f, 4.60517025e+00f},
    {1.67772150e+07f, 1.66355324e+01f},
    {1.67772160e+07f, 1.66355324e+01f},
    {5.03337024e+36f, 8.45091553e+01f},
    {-1.15030444e+37f, NAN},
    {-1.26732076e+37f, NAN},
    {-1.62067697e+37f, NAN},
    {-2.14818631e+37f, NAN},
    {-2.83623258e+37f, NAN},
    {2.91372710e+37f, 8.62650833e+01f},
    {5.26446674e+37f, 8.68566284e+01f},
    {6.35052082e+37f, 8.70441818e+01f},
    {8.33191600e+37f, 8.73157425e+01f},
    {8.37629493e+37f, 8.73210526e+01f},
    {1.05673342e+38f, 8.75534134e+01f},
    {1.17279362e+38f, 8.76576233e+01f},
    {-1.22589693e+38f, NAN},
    {1.33713205e+38f, 8.77887573e+01f},
    {-1.38725262e+38f, NAN},
    {-1.42559946e+38f, NAN},
    {1.51646313e+38f, 8.79146118e+01f},
    {1.52025523e+38f, 8.79171143e+01f},
    {-1.62567468e+38f, NAN},
    {-1.64459056e+38f, NAN},
    {-1.66157688e+38f, NAN},
    {-1.92382651e+38f, NAN},
    {-1.97106972e+38f, NAN},
    {2.03189504e+38f, 8.82071991e+01f},
    {-2.09681335e+38f, NAN},
    {-2.19885659e+38f, NAN},
    {2.22456637e+38f, 8.82977982e+01f},
    {2.30002951e+38f, 8.83311539e+01f},
    {2.33583384e+38f, 8.83466034e+01f},
    {2.33834278e+38f, 8.83476791e+01f},
    {-2.43504160e+38f, NAN},
    {2.58100437e+38f, 8.84464111e+01f},
    {2.59478829e+38f, 8.84517365e+01f},
    {2.64905935e+38f, 8.84724350e+01f},
    {-2.80705060e+38f, NAN},
    {-2.92269847e+38f, NAN},
    {-3.11948713e+38f, NAN},
    {-3.23387039e+38f, NAN},
    {3.24062565e+38f, 8.86740036e+01f},
    {3.40282002e+38f, 8.87228394e+01f},
    {3.40282245e+38f, 8.87228394e+01f},
    {3.40282347e+38f, 8.87228394e+01f},
    {INFINITY, INFINITY},
    {-INFINITY, NAN},
    {NAN, NAN},
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// Precomputed log(t) for t = 1 + i/16 (i=0..15)
static const float logtbl[16] = {
    0.00000000f, 0.06062462f, 0.11778304f, 0.17185026f,
    0.22314355f, 0.27193372f, 0.31845373f, 0.36290549f,
    0.40546511f, 0.44628710f, 0.48550782f, 0.52324814f,
    0.55961579f, 0.59470711f, 0.62860866f, 0.66139848f};

float mylogf(float x)
{
  // Handle special cases
  if (x < 0.0f || isnan(x)) {
    return NAN;
  }
  if (x == 0.0f) {
    return -INFINITY;
  }
  if (isinf(x)) {
    return INFINITY;
  }

  if (fabsf(x - 1.0f) < 0.3f) {
#if 1
    float delta = x - 1.0f;
    return delta *
             (1.0f - delta * (0.5f - delta * (1.0f / 3.0f - delta * 0.25f)));
#else
    printf("chevishev\n");
    float t = (x - 1.0f) / (x + 1.0f);
    float t2 = t * t;
    float t4 = t2 * t2;
    return 2.0f * t * (1.0f + t2 / 3.0f + t4 / 5.0f);
#endif
  }

  int e;
  float m = frexpf(x, &e);

  // Range reduction: bring m to [0.75, 1.5)
  if (m < 0.75f) {
    uint32_t u = *(uint32_t *)&m;
    u += 0x00800000; // exp_incを直接埋め込み
    m = *(float *)&u;
    e--;
  } else if (m >= 1.5f) {
    uint32_t u = *(uint32_t *)&m;
    u -= 0x00800000; // exp_incを直接埋め込み
    m = *(float *)&u;
    e++;
  }

  float f = m - 1.0f;

  // 7th-order Remez coefficients for log(1+x), x ∈ [-0.25, 0.5]
  // Coefficients from user input, Horner's method
  float poly =
      ((((((f * 7.85420644e-2f + -0.1665686656f) * f + 0.2081380434f) * f +
          -0.2510556072f) *
             f +
         0.3330458715f) *
            f +
        -0.4999642132f) *
           f +
       1.0000029016f) *
          f +
      -1.440002855e-7f;

  const float ln2 = 0.69314718f;
  return e * ln2 + poly;
}

// Calculate ULP difference (valid for same sign)
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

// Dynamic ULP tolerance based on input value
int max_ulp_tolerance(float x)
{
  if (x < 1e-30f) {
    return 4; // Subnormal range
  }
  if (x < 1e-10f) {
    return 3; // Small normal numbers
  }
  if (x > 1e10f) {
    return 2; // Large numbers
  }
  return 2; // Main accuracy range
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main(int argc, char **argv)
{
  int ng = 0;
  int total = (int)ARRAY_SIZE(patterns);

  // Print header row (inside main)
  printf("%-12s %-12s %-12s %-8s %-8s\n", "input", "expected", "got", "ULP",
         "result");

  for (size_t i = 0; i < ARRAY_SIZE(patterns); ++i) {
    float result = logf(patterns[i].input); // Use your logf implementation
    float expected = patterns[i].expected;
    int ulp = ulp_diff(result, expected);
    int max_ulp = max_ulp_tolerance(fabsf(expected));

    const char *res_msg = "PASS";
    if (isnan(expected)) {
      if (!isnan(result)) {
        res_msg = "FAIL";
        ng++;
      }
    } else if (isinf(expected)) {
      if (!isinf(result) || (signbit(expected) != signbit(result))) {
        res_msg = "FAIL";
        ng++;
      }
    } else if (ulp > max_ulp) {
      res_msg = "FAIL";
      ng++;
    }

    // Print data row
    printf("%-12e %-12e %-12e %-8d %-8s\n", patterns[i].input, expected, result,
           ulp, res_msg);
  }

  // Print test summary
  printf("\nTotal tests: %d\n", total);
  if (ng == 0) {
    printf("All tests passed.\n");
  } else {
    printf("%d tests failed.\n", ng);
  }

  return ng;
}
