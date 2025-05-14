#include <math.h>
#include <stdint.h>

// Convert int32_t bit pattern to float
static inline float pIntAsFloat(int32_t pI) { // e: int, f: float
    union { int32_t e; float f; } u;
    u.e = pI;
    return u.f;
}

// High-accuracy expf implementation using range reduction, polynomial, and bit manipulation
float expf(float pX) { // e: input, f: result
    // --- Range reduction: pX = pK * pLN2 + pF, pF in [-pLN2/2, pLN2/2] ---
    // pK = round(pX / pLN2)
    const float pINV_LN2 = 1.4426950408889634f;   // e: 1/ln(2), f: 1.4426950408889634
    const float pLN2_HI  = 6.93145752e-1f;        // e: high part of ln(2), f: 0.693145752
    const float pLN2_LO  = 1.42860677e-6f;        // e: low part of ln(2), f: 0.00000142860677
    float pKf = pX * pINV_LN2 + 12582912.0f;      // e: magic bias for rounding, f: 1.2582912e7
    int pK = (int)(pKf - 12582912.0f);
    float pF = pX - pK * pLN2_HI - pK * pLN2_LO;  // e: reduced argument, f: pF

    // --- Polynomial approximation for exp(pF) on small interval ---
    float pR = 1.37805939e-3f;    // e: coeff, f: 0.00137805939
    pR = pR * pF + 8.37312452e-3f;    // e: coeff, f: 0.00837312452
    pR = pR * pF + 4.16695364e-2f;    // e: coeff, f: 0.0416695364
    pR = pR * pF + 1.66664720e-1f;    // e: coeff, f: 0.166664720
    pR = pR * pF + 4.99999851e-1f;    // e: coeff, f: 0.499999851
    pR = pR * pF + 1.0f;              // e: coeff, f: 1.0
    pR = pR * pF + 1.0f;              // e: coeff, f: 1.0

    // --- Scale by 2^pK using float bit manipulation ---
    // 2^pK = reinterpret_bits(0x7f800000 + (pK << 23))
    int32_t pExponent = (pK + 127) << 23; // e: exponent bits, f: (pK+127)<<23
    float pScale = pIntAsFloat(pExponent); // e: 2^pK as float, f: pScale

    float pResult = pR * pScale; // e: final result, f: pResult

    // --- Handle special cases (overflow, underflow, NaN, Inf) ---
    if (isnan(pX)) return NAN;
    if (pX > 88.722839f) return INFINITY;    // e: overflow threshold, f: 88.722839
    if (pX < -103.972084f) return 0.0f;      // e: underflow threshold, f: -103.972084

    return pResult;
}
