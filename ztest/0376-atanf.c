#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "my_printf.c"

static float atanf_taylor(float x)
{
  float x2 = x * x;
  return x - (x2 * x) / 3.0f + (x2 * x2 * x) / 5.0f -
         (x2 * x2 * x2 * x) / 7.0f + (x2 * x2 * x2 * x2 * x) / 9.0f;
}

static float atanf_frac(float x)
{
  float r = 0.0f;
  float x2 = x * x;

  r = (121.0f * x2) / (23.0f + r); // 11
  r = (100.0f * x2) / (21.0f + r); // 10
  r = (81.0f * x2) / (19.0f + r);  //  9
  r = (64.0f * x2) / (17.0f + r);  //  8
  r = (49.0f * x2) / (15.0f + r);  //  7
  r = (36.0f * x2) / (13.0f + r);  //  6
  r = (25.0f * x2) / (11.0f + r);  //  5
  r = (16.0f * x2) / (9.0f + r);   //  4
  r = (9.0f * x2) / (7.0f + r);    //  3
  r = (4.0f * x2) / (5.0f + r);    //  2
  r = (x2) / (3.0f + r);           //  1

  return (x / (1.0f + r)); // 0
}

float myatanf(float x)
{
  if (isnan(x)) {
    return NAN;
  }
  if (isinf(x)) {
    return copysignf(M_PI_2,x);
  }
  if (x < 0) {
    return -myatanf(fabsf(x));
  }else if (fabsf(x) < 0.5f) {
    return atanf_taylor(x);
  } else if (fabsf(x) <= 1.0f) {
    return atanf_frac(fabsf(x));
  } else {
    float inv_x = 1.0f / fabsf(x);
    return M_PI_2 - myatanf(inv_x);
  }
}
struct TestCase {
  float input;
  float expected;
};

// tan(π/6) ≈ 0.57735 → atan(0.57735) ≈ π/6 ≈ 0.523599
// tan(π/4) = 1.0 → atan(1.0) = π/4 ≈ 0.785398
// tan(π/3) ≈ 1.73205 → atan(1.73205) ≈ π/3 ≈ 1.0472
// tan(π/2) → infinity, already tested as INFINITY

struct TestCase test_cases[] = {
    {0.0000000f, 0.0000000f},      {1.0000000f, 0.7853982f},
    {-1.0000000f, -0.7853982f},    {0.5000000f, 0.4636476f},
    {-0.5000000f, -0.4636476f},    {2.0000000f, 1.1071487f},
    {-2.0000000f, -1.1071487f},    {0.0010000f, 0.0010000f},
    {-0.0010000f, -0.0010000f},    {1000.0000000f, 1.5697963f},
    {-1000.0000000f, -1.5697963f}, {INFINITY, 1.5707963f},
    {-INFINITY, -1.5707963f},      {NAN, NAN},
    {0.5773500f, 0.5235986f},      {1.0000000f, 0.7853982f},
    {1.7320500f, 1.0471973f},
};

#define EPSILON 1e-6f

void print_header()
{
  printf("%-16s %-16s %-16s %-16s %s\n", "input", "expected", "result", "diff",
         "status");
  printf("---------------------------------------------------------------------"
         "-----------\n");
}

void print_result(float input, float expected, float result, float diff,
                  bool is_nan, bool is_pass)
{
  if (is_nan) {
    printf("%-16e %-16s %-16e %-16s %s\n", input, "NaN", result, "-",
           is_pass ? "PASS" : "FAIL");
  } else {
    printf("%-16e %-16e %-16e %-16e %s\n", input, expected, result, diff,
           is_pass ? "PASS" : "FAIL");
  }
}

bool test_atanf()
{
  bool all_passed = true;
  print_header();
  for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    float result = atanf(test_cases[i].input);
    //float result = myatanf(test_cases[i].input);
    float diff = fabsf(result - test_cases[i].expected);
    bool is_nan = isnan(test_cases[i].expected);
    bool is_pass;

    if (is_nan) {
      is_pass = isnan(result);
    } else {
      is_pass = (diff <= EPSILON);
    }

    if (!is_pass) {
      all_passed = false;
    }

    print_result(test_cases[i].input, test_cases[i].expected, result, diff,
                 is_nan, is_pass);
  }
  return all_passed;
}

int main()
{
  if (test_atanf()) {
    printf("\nAll test cases passed.\n");
  } else {
    printf("\nSome test cases failed.\n");
  }
  return 0;
}
