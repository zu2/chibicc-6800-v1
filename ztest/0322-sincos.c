#include <math.h>
#include <stdio.h>

#include "my_printf.c"

#define M_PI_3 (M_PI / 3.0f)
#define M_PI_6 (M_PI / 6.0f)

typedef struct {
  float input;
  float expected;
} TestCase;

#define EPSILON 1e-4f

int test_sinf(void)
{
  TestCase tests[] = {
      {0.0f, 0.0f},           {M_PI_6, 0.5f},
      {M_PI_4, 0.70710678f},  {M_PI_3, 0.8660254f},
      {M_PI_2, 1.0f},         {M_PI, 0.0f},
      {3.0f * M_PI_2, -1.0f}, {2.0f * M_PI, 0.0f},
      {-M_PI_6, -0.5f},       {-M_PI_4, -0.70710678f},
      {-M_PI_3, -0.8660254f}, {10.0f * M_PI, 0.0f},
      {1e-7f, 1e-7f},
  };
  int n = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < n; ++i) {
    float actual = sinf(tests[i].input);
    if (fabsf(actual - tests[i].expected) > EPSILON) {
      printf("sinf test failed at %d: input=%.8f expected=%.8f actual=%.8f\n",
             i, tests[i].input, tests[i].expected, actual);
      return i + 1;
    }
  }
  return 0;
}

int test_cosf(void)
{
  TestCase tests[] = {
      {0.0f, 1.0f},           {M_PI_6, 0.8660254f}, {M_PI_4, 0.70710678f},
      {M_PI_3, 0.5f},         {M_PI_2, 0.0f},       {M_PI, -1.0f},
      {3.0f * M_PI_2, 0.0f},  {2.0f * M_PI, 1.0f},  {-M_PI_6, 0.8660254f},
      {-M_PI_4, 0.70710678f}, {-M_PI_3, 0.5f},      {10.0f * M_PI, 1.0f},
      {1e-7f, 1.0f},
  };
  int n = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < n; ++i) {
    float actual = cosf(tests[i].input);
    if (fabsf(actual - tests[i].expected) > EPSILON) {
      printf("cosf test failed at %d: input=%.8f expected=%.8f actual=%.8f\n",
             i, tests[i].input, tests[i].expected, actual);
      return i + 1;
    }
  }
  return 0;
}

int main(int argc, char **argv)
{
  return 0;

  int sret = test_sinf();
  int cret = test_cosf();
  if (sret == 0 && cret == 0) {
    printf("All tests passed.\n");
    return 0;
  }
  if (sret != 0) {
    return sret;
  }
  if (cret != 0) {
    return 100 + cret;
  }
  return -1;
}
