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
    {1.00000000e-30f, -6.90775528e+01f},
    {1.00000001e-10f, -2.30258503e+01f},
    {9.99999975e-06f, -1.15129251e+01f},
    {5.00000000e-01f, -6.93147182e-01f},
    {1.00000000e+00f, 0.00000000e+00f},
    {-1.00000000e+00f, NAN},
    {2.00000000e+00f, 6.93147182e-01f},
    {2.71828175e+00f, 9.99999940e-01f},
    {3.14159274e+00f, 1.14472997e+00f},
    {1.00000000e+01f, 2.30258512e+00f},
    {1.00000000e+02f, 4.60517025e+00f},
    {1.67772150e+07f, 1.66355324e+01f},
    {1.67772160e+07f, 1.66355324e+01f},
    {1.53198326e+37f, 8.56222153e+01f},
    {2.32965202e+37f, 8.60413666e+01f},
    {5.33217095e+37f, 8.68694077e+01f},
    {5.56459367e+37f, 8.69120712e+01f},
    {6.92956231e+37f, 8.71314468e+01f},
    {7.52015262e+37f, 8.72132339e+01f},
    {8.99743003e+37f, 8.73925858e+01f},
    {1.15610972e+38f, 8.76432953e+01f},
    {1.18310895e+38f, 8.76663818e+01f},
    {1.33261485e+38f, 8.77853775e+01f},
    {1.53836175e+38f, 8.79289551e+01f},
    {1.66733698e+38f, 8.80094604e+01f},
    {2.23546756e+38f, 8.83026810e+01f},
    {2.39784244e+38f, 8.83728027e+01f},
    {2.40009034e+38f, 8.83737411e+01f},
    {-2.43933133e+38f, NAN},
    {-2.44193316e+38f, NAN},
    {2.69431245e+38f, 8.84893799e+01f},
    {2.70176603e+38f, 8.84921417e+01f},
    {2.96905290e+38f, 8.85864792e+01f},
    {-3.06800469e+38f, NAN},
    {3.27756458e+38f, 8.86853333e+01f},
    {3.27797733e+38f, 8.86854630e+01f},
    {-3.37511526e+38f, NAN},
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
    0.55961579f, 0.59470711f, 0.62860866f, 0.66139848f
};

// Pade [2/2] for log(1+δ) on δ∈[-1/32,1/32]
static inline float log1p_pade(float d) {
    const float a0 = 1.0f;
    const float a1 = -0.5f;
    const float a2 = 0.3333333f;
    const float b0 = 1.0f;
    const float b1 = -0.25f;
    const float b2 = 0.2f;
    float d2 = d * d;
    float num = a0 + a1 * d + a2 * d2;
    float den = b0 + b1 * d + b2 * d2;
    return d * num / den;
}

float mylogf(float x) {
    if (x <= 0.0f) return NAN;
    int exp;
    float m = frexpf(x, &exp); // x = m * 2^exp, m in [0.5,1)
    m *= 2.0f;
    exp -= 1;
    // 16分割
    int idx = (int)((m - 1.0f) * 16.0f + 0.5f);
    if (idx < 0) idx = 0; if (idx > 15) idx = 15;
    float t = 1.0f + idx / 16.0f;
    float delta = (m - t) / t;
    float log_m = logtbl[idx] + log1p_pade(delta);
    return log_m + exp * 0.69314718f;
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
  return 1; // Main accuracy range
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main(int argc,char **argv)
{
  int ng = 0;
  int total = (int)ARRAY_SIZE(patterns);
  for (size_t i = 0; i < ARRAY_SIZE(patterns); ++i) {
#if 0
    float result = mylogf(patterns[i].input);
#else
    float result = logf(patterns[i].input);
#endif
    float expected = patterns[i].expected;
    int ulp = ulp_diff(result, expected);

    int max_ulp = max_ulp_tolerance(fabsf(expected));

    if (isnan(expected)) {
      if (!isnan(result)) {
        printf("FAIL: input=%e, expected=NaN, got=%e\n", patterns[i].input,
               result);
        ng++;
      } else {
        printf("PASS: input=%e, expected=NaN, got=NaN\n", patterns[i].input);
      }
    } else if (isinf(expected)) {
      if (!isinf(result) || (signbit(expected) != signbit(result))) {
        printf("FAIL: input=%e, expected=%sInf, got=%e\n", patterns[i].input,
               signbit(expected) ? "-" : "+", result);
        ng++;
      } else {
        printf("PASS: input=%e, expected=%sInf, got=%sInf\n", patterns[i].input,
               signbit(expected) ? "-" : "+", signbit(result) ? "-" : "+");
      }
    } else if (ulp > max_ulp) {
      printf("FAIL: input=%e, expected=%e, got=%e, ULP=%d >%d\n",
             patterns[i].input, expected, result, ulp, max_ulp);
      ng++;
    } else {
      printf("PASS: input=%e, expected=%e, got=%e, ULP=%d\n", patterns[i].input,
             expected, result, ulp);
    }
  }
  printf("Total tests: %d\n", total);
  if (ng == 0) {
    printf("All tests passed.\n");
  } else {
    printf("%d tests failed.\n", ng);
  }
  return ng;
}
