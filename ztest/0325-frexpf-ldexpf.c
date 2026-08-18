//
//	float library
//

#include <float.h>
#include <math.h>
#include <stdint.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

int main(int argc, char **argv)
{
	float m;
	int e;

	m = frexpf(1.0f, &e);
	if (m != 0.5f || e != 1 || ldexpf(m, e) != 1.0f)
		return 1;

	m = frexpf(1.5f, &e);
	if (m != 0.75f || e != 1 || ldexpf(m, e) != 1.5f)
		return 2;

	m = frexpf(-1.5f, &e);
	if (m != -0.75f || e != 1 || ldexpf(m, e) != -1.5f)
		return 3;

	m = frexpf(0.75f, &e);
	if (m != 0.75f || e != 0 || ldexpf(m, e) != 0.75f)
		return 4;

	m = frexpf(-0.75f, &e);
	if (m != -0.75f || e != 0 || ldexpf(m, e) != -0.75f)
		return 5;

	m = frexpf(2.0f, &e);
	if (m != 0.5f || e != 2 || ldexpf(m, e) != 2.0f)
		return 6;

	m = frexpf(-2.0f, &e);
	if (m != -0.5f || e != 2 || ldexpf(m, e) != -2.0f)
		return 7;

	m = frexpf(to_float(0x3FFFFFFF), &e);	// 2.0 - LSB
	if (m != to_float(0x3F7FFFFF) || e != 1
	||  ldexpf(m, e) != to_float(0x3FFFFFFF))
		return 8;


	m = frexpf(FLT_MAX, &e);
	if (m != to_float(0x3F7FFFFF) || e != 128 || ldexpf(m, e) != FLT_MAX)
		return 11;

	m = frexpf(-FLT_MAX, &e);
	if (m != to_float(0xBF7FFFFF) || e != 128 || ldexpf(m, e) != -FLT_MAX)
		return 12;

	m = frexpf(3.40282347e+38f, &e);
	if (m != to_float(0x3F7FFFFF) || e != 128
	||  ldexpf(m, e) != 3.40282347e+38f)
		return 13;

	m = frexpf(-3.40282347e+38f, &e);
	if (m != to_float(0xBF7FFFFF) || e != 128
	||  ldexpf(m, e) != -3.40282347e+38f)
		return 14;


	m = frexpf(FLT_MIN, &e);
	if (m != 0.5f || e != -125 || ldexpf(m, e) != FLT_MIN)
		return 21;

	m = frexpf(to_float(0x00800001), &e);	// FLT_MIN + LSB
	if (m != to_float(0x3F000001) || e != -125
	||  ldexpf(m, e) != to_float(0x00800001))
		return 22;

	m = frexpf(to_float(0x007FFFFF), &e);	// max subnormal
	if (m != to_float(0x3F7FFFFE) || e != -126
	||  ldexpf(m, e) != to_float(0x007FFFFF))
		return 23;

	m = frexpf(to_float(0x00000001), &e);	// min subnormal
	if (m != 0.5f || e != -148 || ldexpf(m, e) != to_float(0x00000001))
		return 24;

	m = frexpf(to_float(0x00000002), &e);	// 2nd subnormal
	if (m != 0.5f || e != -147 || ldexpf(m, e) != to_float(0x00000002))
		return 25;

	m = frexpf(to_float(0x80000001), &e);	// -min subnormal
	if (m != -0.5f || e != -148 || ldexpf(m, e) != to_float(0x80000001))
		return 26;


	m = frexpf(0.0f, &e);
	if (m != 0.0f || signbit(m) || e != 0 || ldexpf(m, e) != 0.0f)
		return 31;

	m = frexpf(-0.0f, &e);
	if (m != 0.0f || !signbit(m) || e != 0 || !signbit(ldexpf(m, e)))
		return 32;

	m = frexpf(INFINITY, &e);
	if (m != INFINITY || e != 0 || ldexpf(m, e) != INFINITY)
		return 33;

	m = frexpf(-INFINITY, &e);
	if (m != -INFINITY || e != 0 || ldexpf(m, e) != -INFINITY)
		return 34;

	m = frexpf(NAN, &e);
	if (!isnan(m) || e != 0 || !isnan(ldexpf(m, e)))
		return 35;

	m = frexpf(-NAN, &e);
	if (!isnan(m) || e != 0 || !isnan(ldexpf(m, e)))
		return 36;


	m = frexpf(FLT_EPSILON, &e);
	if (m != 0.5f || e != -22 || ldexpf(m, e) != FLT_EPSILON)
		return 41;

	m = frexpf(-FLT_EPSILON, &e);
	if (m != -0.5f || e != -22 || ldexpf(m, e) != -FLT_EPSILON)
		return 42;

	m = frexpf(1.0f + FLT_EPSILON, &e);
	if (m != to_float(0x3F000001) || e != 1
	||  ldexpf(m, e) != 1.0f + FLT_EPSILON)
		return 43;

	m = frexpf(1.0f - FLT_EPSILON, &e);
	if (m != to_float(0x3F7FFFFE) || e != 0
	||  ldexpf(m, e) != 1.0f - FLT_EPSILON)
		return 44;


	/* exact shift, 1.0f, k = 0 to 24 */
	if (ldexpf(1.0f, -126) != to_float(0x00800000))
		return 51;
	if (ldexpf(1.0f, -127) != to_float(0x00400000))
		return 52;
	if (ldexpf(1.0f, -128) != to_float(0x00200000))
		return 53;
	if (ldexpf(1.0f, -129) != to_float(0x00100000))
		return 54;
	if (ldexpf(1.0f, -130) != to_float(0x00080000))
		return 55;
	if (ldexpf(1.0f, -131) != to_float(0x00040000))
		return 56;
	if (ldexpf(1.0f, -132) != to_float(0x00020000))
		return 57;
	if (ldexpf(1.0f, -133) != to_float(0x00010000))
		return 58;
	if (ldexpf(1.0f, -134) != to_float(0x00008000))
		return 59;
	if (ldexpf(1.0f, -135) != to_float(0x00004000))
		return 60;
	if (ldexpf(1.0f, -136) != to_float(0x00002000))
		return 61;
	if (ldexpf(1.0f, -137) != to_float(0x00001000))
		return 62;
	if (ldexpf(1.0f, -138) != to_float(0x00000800))
		return 63;
	if (ldexpf(1.0f, -139) != to_float(0x00000400))
		return 64;
	if (ldexpf(1.0f, -140) != to_float(0x00000200))
		return 65;
	if (ldexpf(1.0f, -141) != to_float(0x00000100))
		return 66;
	if (ldexpf(1.0f, -142) != to_float(0x00000080))
		return 67;
	if (ldexpf(1.0f, -143) != to_float(0x00000040))
		return 68;
	if (ldexpf(1.0f, -144) != to_float(0x00000020))
		return 69;
	if (ldexpf(1.0f, -145) != to_float(0x00000010))
		return 70;
	if (ldexpf(1.0f, -146) != to_float(0x00000008))
		return 71;
	if (ldexpf(1.0f, -147) != to_float(0x00000004))
		return 72;
	if (ldexpf(1.0f, -148) != to_float(0x00000002))
		return 73;
	if (ldexpf(1.0f, -149) != to_float(0x00000001))
		return 74;
	if (ldexpf(1.0f, -150) != 0.0f)
		return 75;


	/* round up at every k, 2.0f - LSB */
	if (ldexpf(to_float(0x3FFFFFFF), -127) != to_float(0x00800000))
		return 81;
	if (ldexpf(to_float(0x3FFFFFFF), -128) != to_float(0x00400000))
		return 82;
	if (ldexpf(to_float(0x3FFFFFFF), -129) != to_float(0x00200000))
		return 83;
	if (ldexpf(to_float(0x3FFFFFFF), -130) != to_float(0x00100000))
		return 84;
	if (ldexpf(to_float(0x3FFFFFFF), -131) != to_float(0x00080000))
		return 85;
	if (ldexpf(to_float(0x3FFFFFFF), -132) != to_float(0x00040000))
		return 86;
	if (ldexpf(to_float(0x3FFFFFFF), -133) != to_float(0x00020000))
		return 87;
	if (ldexpf(to_float(0x3FFFFFFF), -134) != to_float(0x00010000))
		return 88;
	if (ldexpf(to_float(0x3FFFFFFF), -135) != to_float(0x00008000))
		return 89;
	if (ldexpf(to_float(0x3FFFFFFF), -136) != to_float(0x00004000))
		return 90;
	if (ldexpf(to_float(0x3FFFFFFF), -137) != to_float(0x00002000))
		return 91;
	if (ldexpf(to_float(0x3FFFFFFF), -138) != to_float(0x00001000))
		return 92;
	if (ldexpf(to_float(0x3FFFFFFF), -139) != to_float(0x00000800))
		return 93;
	if (ldexpf(to_float(0x3FFFFFFF), -140) != to_float(0x00000400))
		return 94;
	if (ldexpf(to_float(0x3FFFFFFF), -141) != to_float(0x00000200))
		return 95;
	if (ldexpf(to_float(0x3FFFFFFF), -142) != to_float(0x00000100))
		return 96;
	if (ldexpf(to_float(0x3FFFFFFF), -143) != to_float(0x00000080))
		return 97;
	if (ldexpf(to_float(0x3FFFFFFF), -144) != to_float(0x00000040))
		return 98;
	if (ldexpf(to_float(0x3FFFFFFF), -145) != to_float(0x00000020))
		return 99;
	if (ldexpf(to_float(0x3FFFFFFF), -146) != to_float(0x00000010))
		return 100;
	if (ldexpf(to_float(0x3FFFFFFF), -147) != to_float(0x00000008))
		return 101;
	if (ldexpf(to_float(0x3FFFFFFF), -148) != to_float(0x00000004))
		return 102;
	if (ldexpf(to_float(0x3FFFFFFF), -149) != to_float(0x00000002))
		return 103;
	if (ldexpf(to_float(0x3FFFFFFF), -150) != to_float(0x00000001))
		return 104;


	/* negative side */
	if (ldexpf(-1.0f, -127) != to_float(0x80400000))
		return 111;
	if (ldexpf(-1.5f, -127) != to_float(0x80600000))
		return 112;
	if (ldexpf(-1.0f, -149) != to_float(0x80000001))
		return 113;
	if (ldexpf(-1.5f, -150) != to_float(0x80000001))
		return 114;
	if (ldexpf(-1.5f, -151) != 0.0f)
		return 115;


	/* subnormal in, and the tie in both directions */
	if (ldexpf(to_float(0x00000001), 149) != 1.0f)
		return 121;
	if (ldexpf(to_float(0x00400000), 1) != to_float(0x00800000))
		return 122;
	if (ldexpf(to_float(0x00800000), -1) != to_float(0x00400000))
		return 123;
	if (ldexpf(to_float(0x007FFFFF), -1) != to_float(0x00400000))
		return 124;
	if (ldexpf(to_float(0x00000002), -1) != to_float(0x00000001))
		return 125;
	if (ldexpf(to_float(0x00000001), -1) != 0.0f)	// tie, down to even
		return 126;
	if (ldexpf(to_float(0x00000003), -1) != to_float(0x00000002))	// tie, up
		return 127;
	if (ldexpf(to_float(0x00000005), -1) != to_float(0x00000002))	// tie, down
		return 128;
	if (ldexpf(to_float(0x3FBFFFFF), -149) != to_float(0x00000001))
		return 129;


	/* overflow, and a 16bit exponent */
	if (ldexpf(1.0f, 127) != to_float(0x7F000000))
		return 131;
	if (ldexpf(to_float(0x3FFFFFFF), 127) != FLT_MAX)
		return 132;
	if (ldexpf(1.0f, 128) != INFINITY)
		return 133;
	if (ldexpf(-1.0f, 128) != -INFINITY)
		return 134;
	if (ldexpf(FLT_MAX, 1) != INFINITY)
		return 135;
	if (ldexpf(1.0f, 32767) != INFINITY)
		return 136;
	if (ldexpf(1.0f, -32768) != 0.0f)
		return 137;
	if (ldexpf(to_float(0x00000001), 32767) != INFINITY)
		return 138;
	if (ldexpf(FLT_MAX, -32768) != 0.0f)
		return 139;


	/* NaN, Inf and zero with a non zero exponent */
	if (ldexpf(INFINITY, -200) != INFINITY)
		return 141;
	if (ldexpf(-INFINITY, 200) != -INFINITY)
		return 142;
	if (!isnan(ldexpf(NAN, -5)))
		return 143;
	if (ldexpf(0.0f, 100) != 0.0f || signbit(ldexpf(0.0f, 100)))
		return 144;
	if (!signbit(ldexpf(-0.0f, 100)))
		return 145;

	return 0;
}
