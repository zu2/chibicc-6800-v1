#include "common.h"

#define FLT_MAX (3.402823466e+38)
#define FLT_MIN (1.175494351e-38)
#define FLT_EPSILON (1.192092896e-07)

extern	float	fabsf(float x);
extern	int	isnan(float x);


int is_nan(float x) {
    return x != x;
}

//
// return x*(2^exp)
//
float ldexpf(float x, int exp) {
    union {
        float f;
        unsigned long i;
    } u = { .f = x };

    unsigned long current_exp = (u.i >> 23) & 0xFF;
    if (current_exp == 0) return x;
    if (current_exp == 0xFF) return x;

    current_exp += exp;
    if (current_exp <= 0) return 0.0;
    if (current_exp >= 0xFF) return x > 0 ? (1/0.0) : (-1.0/0.0);

    u.i = (u.i & 0x807FFFFF) | (current_exp << 23);
    return u.f;
}

is_infinity(float x) {
    union {
        float f;
        unsigned long i;
    } u;
    u.f = x;

    unsigned long exponent = (u.i >> 23) & 0xFF;
    unsigned long mantissa = u.i & 0x7FFFFF;

    return (exponent == 0xFF && mantissa == 0);
}


int main(int argc, char **argv)
{
    float a = 1.0;
    float b = 1e-7;
    float c = a + b;
    if (fabsf(c - 1.0000001) >= FLT_EPSILON) {
        return 1;
    }

    float x = 1.0;
    float y = ldexpf(1.0, -23);
    if (x + y != 1.00000012) {
	return 2;
    }

    float overflow = FLT_MAX * 2.0;
    if (!is_infinity(overflow)) {
        return 3;
    }

    float underflow = FLT_MIN / 2.0;
    if (underflow != 0.0 && underflow >= FLT_MIN) {
        return 4;
    }

    float nan_value = 0.0 / 0.0;
    if (!is_nan(nan_value)) {
        return 5;
    }

    float nan_propagation = nan_value + 1.0;
    if (!is_nan(nan_propagation)) {
        return 6;
    }

    float pos_inf = 1.0 / 0.0;
    float neg_inf = -1.0 / 0.0;
    if (!is_infinity(pos_inf)){
	return 7;
    }
    if (!is_infinity(neg_inf)){
	return 8;
    }
    float inf_addition = pos_inf + neg_inf;
    if (!is_nan(inf_addition)) {
        return 9;
    }

    return 0;
}
