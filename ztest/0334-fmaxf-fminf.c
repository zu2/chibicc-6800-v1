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
	if (fmaxf(1.0f, 2.0f) != 2.0f)
		return 1;
	if (fmaxf(2.0f, 1.0f) != 2.0f)
		return 2;
	if (fminf(1.0f, 2.0f) != 1.0f)
		return 3;
	if (fminf(2.0f, 1.0f) != 1.0f)
		return 4;
	if (fmaxf(2.0f, 2.0f) != 2.0f)
		return 5;
	if (fminf(2.0f, 2.0f) != 2.0f)
		return 6;

	if (fmaxf(-1.0f, 2.0f) != 2.0f)
		return 11;
	if (fmaxf(2.0f, -1.0f) != 2.0f)
		return 12;
	if (fminf(-1.0f, 2.0f) != -1.0f)
		return 13;
	if (fminf(2.0f, -1.0f) != -1.0f)
		return 14;
	if (fmaxf(-2.0f, -1.0f) != -1.0f)
		return 15;
	if (fmaxf(-1.0f, -2.0f) != -1.0f)
		return 16;
	if (fminf(-2.0f, -1.0f) != -2.0f)
		return 17;
	if (fminf(-1.0f, -2.0f) != -2.0f)
		return 18;

	if (fmaxf(0.0f, 1.0f) != 1.0f)
		return 21;
	if (fminf(0.0f, 1.0f) != 0.0f)
		return 22;
	if (fmaxf(-0.0f, -1.0f) != -0.0f)
		return 23;
	if (fminf(-0.0f, -1.0f) != -1.0f)
		return 24;
	if (fmaxf(to_float(0x00000001), to_float(0x00000002))
	 != to_float(0x00000002))
		return 25;
	if (fminf(to_float(0x00000001), to_float(0x00000002))
	 != to_float(0x00000001))
		return 26;
	if (fmaxf(FLT_MAX, -FLT_MAX) != FLT_MAX)
		return 27;
	if (fminf(FLT_MAX, -FLT_MAX) != -FLT_MAX)
		return 28;

	/* one side is Inf */
	if (fmaxf(INFINITY, 1.0f) != INFINITY)
		return 31;
	if (fmaxf(1.0f, INFINITY) != INFINITY)
		return 32;
	if (fminf(INFINITY, 1.0f) != 1.0f)
		return 33;
	if (fminf(1.0f, INFINITY) != 1.0f)
		return 34;
	if (fmaxf(-INFINITY, 1.0f) != 1.0f)
		return 35;
	if (fmaxf(1.0f, -INFINITY) != 1.0f)
		return 36;
	if (fminf(-INFINITY, 1.0f) != -INFINITY)
		return 37;
	if (fminf(1.0f, -INFINITY) != -INFINITY)
		return 38;

	/* both sides are Inf */
	if (fmaxf(INFINITY, -INFINITY) != INFINITY)
		return 41;
	if (fmaxf(-INFINITY, INFINITY) != INFINITY)
		return 42;
	if (fminf(INFINITY, -INFINITY) != -INFINITY)
		return 43;
	if (fminf(-INFINITY, INFINITY) != -INFINITY)
		return 44;
	if (fmaxf(INFINITY, INFINITY) != INFINITY)
		return 45;
	if (fminf(-INFINITY, -INFINITY) != -INFINITY)
		return 46;

	/* NaN gives way to the other side */
	if (fmaxf(NAN, 1.0f) != 1.0f)
		return 51;
	if (fmaxf(1.0f, NAN) != 1.0f)
		return 52;
	if (fminf(NAN, 1.0f) != 1.0f)
		return 53;
	if (fminf(1.0f, NAN) != 1.0f)
		return 54;
	if (fmaxf(NAN, INFINITY) != INFINITY)
		return 55;
	if (fmaxf(INFINITY, NAN) != INFINITY)
		return 56;
	if (fminf(NAN, 0.0f) != 0.0f)
		return 57;
	if (fminf(0.0f, NAN) != 0.0f)
		return 58;
	if (!isnan(fmaxf(NAN, NAN)))
		return 59;
	if (!isnan(fminf(NAN, NAN)))
		return 60;

	return 0;
}
