#include <math.h>
#include <stdint.h>

// Fast approximation of logf (natural logarithm, float)
// Range reduction and quadratic polynomial approximation
float logf(float x) {
    // Special cases
    if (x < 0.0f) return NAN;        // log(negative) is NaN
    if (x == 0.0f) return -INFINITY; // log(0) is -Inf
    if (isinf(x)) return INFINITY;   // log(Inf) is Inf
    if (isnan(x)) return NAN;        // log(NaN) is NaN

    // Polynomial coefficients for log2 approximation
    const float pA = -0.6296735f;    // quadratic coefficient
    const float pB =  1.466967f;     // linear coefficient

    union { float f; uint32_t i; } ux1, ux2;
    ux1.f = x;

    // Extract exponent (bits 23-30)
    int exp = (ux1.i & 0x7F800000) >> 23;
    // Actual exponent is exp - 127
    int greater = ux1.i & 0x00400000; // true if significand > 1.5

    float signif, fexp, lg2;

    if (greater) {
        // significand >= 1.5, normalize to [0.75, 1.5)
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f000000;
        signif = ux2.f;
        fexp = exp - 126; // 126 instead of 127 due to normalization
        signif = signif - 1.0f;
        lg2 = fexp + pA * signif * signif + pB * signif;
    } else {
        // normalize to [0.75, 1.5)
        ux2.i = (ux1.i & 0x007FFFFF) | 0x3f800000;
        signif = ux2.f;
        fexp = exp - 127;
        signif = signif - 1.0f;
        lg2 = fexp + pA * signif * signif + pB * signif;
    }

    // Convert log2(x) to ln(x)
    return lg2 * M_LN2;
}
