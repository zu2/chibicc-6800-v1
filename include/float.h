//
//	float.h for chibicc 6800
//
#include "__use_float_for_double.h"

#ifndef FLOAT_H
#define FLOAT_H

#define FLT_RADIX        2
#define FLT_MANT_DIG     24
#define FLT_DIG          6
#define FLT_MIN_EXP      (-125)
#define FLT_MIN_10_EXP   (-37)
#define FLT_MAX_EXP      128
#define FLT_MAX_10_EXP   38

#define FLT_MAX          0x1.fffffep127f   // ≒ 3.4028235e+38
#define FLT_MIN          0x1.0p-126f       // ≒ 1.1754944e-38
#define FLT_TRUE_MIN     0x1.0p-149f       // 最小サブノーマル値 ≒ 1.4012985e-45
#define FLT_EPSILON      0x1.0p-23f        // ≒ 1.1920929e-7

#define FLT_HAS_SUBNORM  1
#define FLT_ROUNDS       1
#define FLT_EVAL_METHOD  0

#define FLT_INFINITY     (1.0f/0.0f)
#define FLT_NAN          (0.0f/0.0f)

#endif // FLOAT_H
