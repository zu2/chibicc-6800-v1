#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_printf.c"

typedef struct {
    float base;
    float exponent;
    float expected;
} PowfTestCase;

PowfTestCase powf_test_cases[] = {
    {2.0f, 3.0f, 8.0f},
    {10.0f, -1.0f, 0.1f},
    {5.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {-2.0f, 2.0f, 4.0f},
    {-2.0f, 0.5f, NAN},
    {2.0f, 1000.0f, INFINITY},
    {1.0f, 100.0f, 1.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, -1.0f, INFINITY},
    {1.0f, NAN, NAN},
    {NAN, 1.0f, NAN},
    {INFINITY, 1.0f, INFINITY},
    {1.0f, INFINITY, 1.0f},
    {0.5f, INFINITY, 0.0f},
    {1.5f, INFINITY, INFINITY},
    {1.40129846e-45f, 1.0f, 1.40129846e-45f},
    {3.40282347e+38f, 1.0f, 3.40282347e+38f},
    {1.0f, 3.40282347e+38f, 1.0f},
    {2.0f, 3.40282347e+38f, INFINITY},
    {0.5f, 3.40282347e+38f, 0.0f},
    {2.0f, -3.40282347e+38f, 0.0f},
    {0.5f, -3.40282347e+38f, INFINITY},
};


int32_t ulp_diff(float a, float b) {
    if (isnan(a) || isnan(b) || isinf(a) || isinf(b) || signbit(a) != signbit(b)) {
        return -1;
    }
    int32_t ia, ib;
    memcpy(&ia, &a, sizeof(float));
    memcpy(&ib, &b, sizeof(float));
    if (a < 0) {
        ia = 0x80000000 - ia;
        ib = 0x80000000 - ib;
    }
    return abs(ia - ib);
}

int main() {
    int n_tests = sizeof(powf_test_cases) / sizeof(powf_test_cases[0]);
    int passes = 0;
    float tolerance = 10.0f * FLT_EPSILON;

    printf("%14s %14s %14s %14s %14s %4s %8s\n",
           "Base", "Exponent", "Expected", "Actual", "Diff", "Pass", "ULP");
    printf("%14s %14s %14s %14s %14s %4s %8s\n",
           "-------------", "-------------", "-------------", "-------------", "-------------", "----", "--------");

    for (int i = 0; i < n_tests; ++i) {
        float base = powf_test_cases[i].base;
        float exponent = powf_test_cases[i].exponent;
        float expected = powf_test_cases[i].expected;
        float actual = powf(base, exponent);
        int pass;
        if (isnan(expected)) {
            pass = isnan(actual);
        } else if (isinf(expected)) {
            pass = isinf(actual);
        } else {
            float diff = fabsf(actual - expected);
            pass = (diff <= tolerance * fabsf(expected)) || (diff <= FLT_MIN);
        }
        passes += pass;
        float diff = (isnan(expected) || isnan(actual) || isinf(expected) || isinf(actual))
                   ? 0.0f : fabsf(actual - expected);
        int32_t ulp = ulp_diff(actual, expected);
        printf("%14e %14e %14e %14e %14e %4s %8d\n",
               base, exponent, expected, actual, diff, pass ? "YES" : "NO", ulp);
    }

    printf("\nPass rate: %d/%d (%.2f%%)\n", passes, n_tests, 100.0f * passes / n_tests);
    return 0;
}

