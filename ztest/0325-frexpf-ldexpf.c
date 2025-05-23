#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "my_printf.c"

#define EPS 1e-6f

int float_eq(float a, float b)
{
  if (isnan(a) && isnan(b)) {
    return 1;
  }
  if (isinf(a) && isinf(b) && (signbit(a) == signbit(b))) {
    return 1;
  }
  if (a == 0.0f && b == 0.0f && (signbit(a) == signbit(b))) {
    return 1;
  }
  return fabsf(a - b) < EPS;
}

typedef struct {
  float input;
  float expected_m;
  int expected_e;
  const char *desc;
} TestCase;

float max_subnormal(void)
{
  union {
    uint32_t u;
    float f;
  } v;
  v.u = 0x007FFFFF;
  return v.f;
}

float flt_min_next(void)
{
  union {
    uint32_t u;
    float f;
  } v;
  v.f = FLT_MIN;
  v.u += 1;
  return v.f;
}

int main(void)
{
  TestCase tests[] = {{1.0f, 0.5f, 1, "1.0 normal"},
                      {1.99999988f, 0.99999994f, 1, "max mantissa"},
                      {FLT_MAX, 0.99999994f, 128, "FLT_MAX"},
                      {-FLT_MAX, -0.99999994f, 128, "-FLT_MAX"},
                      {FLT_MIN, 0.5f, -125, "FLT_MIN"},
                      {flt_min_next(), 0.50000006f, -125, "FLT_MIN + 1ulp"},
                      {max_subnormal(), 0.99999988f, -126, "max subnormal"},
                      {1.40129846e-45f, 0.5f, -148, "min subnormal"},
                      {2.80259693e-45f, 0.5f, -147, "2nd subnormal"},
                      {-1.40129846e-45f, -0.5f, -148, "-min subnormal"},
                      {0.0f, 0.0f, 0, "zero"},
                      {-0.0f, -0.0f, 0, "-zero"},
                      {INFINITY, INFINITY, 0, "infinity"},
                      {-INFINITY, -INFINITY, 0, "-infinity"},
                      {NAN, NAN, 0, "nan"},
                      {-NAN, -NAN, 0, "-nan"},
                      {1.5f, 0.75f, 1, "1.5"},
                      {-1.5f, -0.75f, 1, "-1.5"},
                      {0.75f, 0.75f, 0, "0.75"},
                      {-0.75f, -0.75f, 0, "-0.75"},
                      {2.0f, 0.5f, 2, "2.0"},
                      {-2.0f, -0.5f, 2, "-2.0"},
                      {3.40282347e+38f, 0.99999994f, 128, "max finite"},
                      {-3.40282347e+38f, -0.99999994f, 128, "-max finite"},
                      {FLT_EPSILON, 0.5f, -22, "FLT_EPSILON"},
                      {-FLT_EPSILON, -0.5f, -22, "-FLT_EPSILON"},
                      {1.0f + FLT_EPSILON, 0.50000006f, 1, "1.0 + EPS"},
                      {1.0f - FLT_EPSILON, 0.99999994f, 0, "1.0 - EPS"}};
  int n = sizeof(tests) / sizeof(tests[0]);
  int all_pass = 1;
  printf("%-18s %.8s %.8s %4s %.8s %s %s\n", "desc", "input", "m", "e", "ldexp",
         "frexp", "result");
  for (int i = 0; i < n; ++i) {
    float m;
    int e;
    m = frexpf(tests[i].input, &e);
    float y = ldexpf(m, e);
    int ok_ld = float_eq(y, tests[i].input);
    int ok_m = float_eq(m, tests[i].expected_m);
    int ok_e = (e == tests[i].expected_e);
    int ok = ok_ld && ok_m && ok_e;
    printf("%-18s %.8e %.8e %4d %.8e %s %s\n", tests[i].desc, tests[i].input, m,
           e, y, (ok_m && ok_e) ? "OK" : "NG", ok_ld ? "OK" : "NG");
    if (!ok) {
      all_pass = 0;
    }
  }
  printf("\nAll tests %s\n", all_pass ? "PASS" : "FAIL");
  return all_pass ? 0 : 1;
}
