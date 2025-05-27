#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

#include "my_printf.c"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    float input;
    float expected;
} TestCase;

// Threshold to avoid overflow in expf(2.0f * x)
#define TANH_THRESHOLD 44.3614f

static TestCase test_cases[] = {
    { -2.00000000e+01f, -1.00000000e+00f },
    { -1.00000000e+01f, -1.00000000e+00f },
    { -1.00000000e+00f, -7.61594176e-01f },
    { -5.00000000e-01f, -4.62117165e-01f },
    { 0.00000000e+00f, 0.00000000e+00f },
    { 5.00000000e-01f, 4.62117165e-01f },
    { 1.00000000e+00f, 7.61594176e-01f },
    { 1.00000000e+01f, 1.00000000e+00f },
    { 2.00000000e+01f, 1.00000000e+00f },
    { 1.40129846e-45f, 1.40129846e-45f },
    { -1.40129846e-45f, -1.40129846e-45f },
    { 0.00000000e+00f, 0.00000000e+00f },
    { -0.00000000e+00f, -0.00000000e+00f },
    { 3.14159274e+00f, 9.96272087e-01f },
    { -3.14159274e+00f, -9.96272087e-01f },
    { 2.71828175e+00f, 9.91328895e-01f },
    { -2.71828175e+00f, -9.91328895e-01f },
    { 4.00000000e+01f, 1.00000000e+00f },
    { 4.20000000e+01f, 1.00000000e+00f },
    { 4.40000000e+01f, 1.00000000e+00f },
    { 4.43614006e+01f, 1.00000000e+00f },
    { 4.44000015e+01f, 1.00000000e+00f },
    { 4.50000000e+01f, 1.00000000e+00f },
    { 5.00000000e+01f, 1.00000000e+00f },
    { INFINITY, 1.00000000e+00f },
    { -INFINITY, -1.00000000e+00f },
    { NAN, NAN },
};

float my_tanhf(float x) {
    // Return ±1.0 for values beyond the threshold to avoid overflow
    if (fabsf(x) < 1e-7f) {
        return x;
    }
    if (x > TANH_THRESHOLD) return 1.0f;
    if (x < -TANH_THRESHOLD) return -1.0f;
    // For |x| <= threshold, use the identity tanh(x) = (exp(2x) - 1)/(exp(2x) + 1)
    float exp2x = expf(2.0f * x);
    return (exp2x - 1.0f) / (exp2x + 1.0f);
}

// Function to compute ULP difference between two floats
int32_t ulp_diff(float a, float b) {
    if (isnan(a) || isnan(b) || isinf(a) || isinf(b) || signbit(a) != signbit(b))
        return -1;
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
    int n_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int passes = 0;
    float tolerance = 10.0f * FLT_EPSILON;

    // Print header row
    printf("%14s %14s %14s %14s %4s %8s\n", "Input", "Expected", "Actual", "Diff", "Pass", "ULP");
    printf("%14s %14s %14s %14s %4s %8s\n", "-------------", "-------------", "-------------", "-------------", "----", "--------");

    for (int i = 0; i < n_tests; ++i) {
        float x = test_cases[i].input;
        float expected = test_cases[i].expected;
        float actual = tanhf(x);
//      float actual = my_tanhf(x);
        int pass;
        if (isnan(expected)) {
            pass = isnan(actual);
        } else if (isinf(expected)) {
            pass = isinf(actual) && (signbit(expected) == signbit(actual));
        } else {
            float diff = fabsf(actual - expected);
            pass = (diff <= tolerance);
        }
        passes += pass;
        float diff = fabsf(actual - expected);
        int32_t ulp = ulp_diff(actual, expected);
        printf("%14e %14e %14e %14e %4s %8d\n",
               x, expected, actual, diff, pass ? "YES" : "NO", ulp);
    }

    printf("\nPass rate: %d/%d (%.2f%%)\n", passes, n_tests, 100.0f * passes / n_tests);
    return 0;
}

