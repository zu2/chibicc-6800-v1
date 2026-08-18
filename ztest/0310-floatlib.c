//
//	float library
//

#include "common.h"

extern	float	fabsf(float x);
extern	float	floorf(float x);
extern	float	ceilf(float x);
extern	float	roundf(float x);


int
cmpfl(float f, unsigned long g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (fabsf(1.0f) != 1.0f)
		return 1;
	if (fabsf(-1.0f) != 1.0f)
		return 2;
	if (fabsf(0.0) == 1.0f)
		return 3;

	if (floorf(0.0f) != 0.0f)
		return 11;

	if (floorf(0.5f) != 0.0f)
		return 12;

	if (floorf(1.0f) != 1.0f)
		return 13;

	if (floorf(1.5f) != 1.0f)
		return 14;

	if (floorf(2.5f) != 2.0f)
		return 15;

	if (floorf(8388607.5f) != 8388607.0f)
		return 16;

	if (floorf(8388608.0f) != 8388608.0f)
		return 17;


	if (floorf(-0.0f) != 0.0f)
		return 21;

	if (floorf(-0.5f) != -1.0f)
		return 22;

	if (floorf(-1.0f) != -1.0f)
		return 23;

	if (floorf(-1.5f) != -2.0f)
		return 24;

	if (floorf(-2.5f) != -3.0f)
		return 25;

	if (floorf(-8388607.5f) != -8388608.0f)
		return 26;

	if (floorf(-8388608.0f) != -8388608.0f)
		return 27;

	if (floorf(256.0f) != 256.0f)
		return 31;

	if (floorf(256.5f) != 256.0f)
		return 32;

	if (floorf(511.5f) != 511.0f)
		return 33;

	if (floorf(-256.5f) != -257.0f)
		return 34;

	if (floorf(65536.0f) != 65536.0f)
		return 35;

	if (floorf(65536.5f) != 65536.0f)
		return 36;

	if (floorf(-65536.5f) != -65537.0f)
		return 37;

	if (ceilf(0.0f) != 0.0f)
		return 51;

	if (ceilf(0.5f) != 1.0f)
		return 52;

	if (ceilf(1.0f) != 1.0f)
		return 53;

	if (ceilf(1.5f) != 2.0f)
		return 54;

	if (ceilf(2.5f) != 3.0f)
		return 55;

	if (ceilf(8388607.5f) != 8388608.0f)
		return 56;

	if (ceilf(8388608.0f) != 8388608.0f)
		return 57;

	if (ceilf(-0.0f) != -0.0f)
		return 61;

	if (ceilf(-0.5f) != -0.0f)
		return 62;

	if (ceilf(-1.0f) != -1.0f)
		return 63;

	if (ceilf(-1.5f) != -1.0f)
		return 64;

	if (ceilf(-2.5f) != -2.0f)
		return 65;

	if (ceilf(-8388607.5f) != -8388607.0f)
		return 66;

	if (ceilf(-8388608.0f) != -8388608.0f)
		return 67;

	if (ceilf(256.0f) != 256.0f)
		return 41;

	if (ceilf(256.5f) != 257.0f)
		return 42;

	if (ceilf(511.5f) != 512.0f)
		return 43;

	if (ceilf(-256.5f) != -256.0f)
		return 44;

	if (ceilf(65536.0f) != 65536.0f)
		return 45;

	if (ceilf(65536.5f) != 65537.0f)
		return 46;

	if (ceilf(-65536.5f) != -65536.0f)
		return 47;


	if (roundf(0.0f) != 0.0f)
		return 101;

	if (roundf(0.4f) != 0.0f)
		return 102;

	if (roundf(0.5f) != 1.0f)
		return 103;

	if (roundf(1.0f) != 1.0f)
		return 104;

	if (roundf(1.4f) != 1.0f)
		return 105;

	if (roundf(1.5f) != 2.0f)
		return 106;

	if (roundf(1.6f) != 2.0f)
		return 107;

	if (roundf(2.4f) != 2.0f)
		return 108;

	if (roundf(2.5f) != 3.0f)
		return 109;

	if (roundf(8388607.5f) != 8388608.0f)
		return 112;

	if (roundf(8388608.0f) != 8388608.0f)
		return 113;


	if (roundf(-0.0f) != -0.0f)
		return 71;

	if (roundf(-0.4f) != -0.0f)
		return 72;

	if (roundf(-0.5f) != -1.0f)
		return 73;

	if (roundf(-1.0f) != -1.0f)
		return 74;

	if (roundf(-1.4f) != -1.0f)
		return 75;

	if (roundf(-1.5f) != -2.0f)
		return 76;

	if (roundf(-1.6f) != -2.0f)
		return 77;

	if (roundf(-2.4f) != -2.0f)
		return 78;

	if (roundf(-2.5f) != -3.0f)
		return 79;

	if (roundf(-8388607.5f) != -8388608.0f)
		return 82;

	if (roundf(-8388608.0f) != -8388608.0f)
		return 83;

	if (roundf(256.0f) != 256.0f)
		return 91;

	if (roundf(256.4f) != 256.0f)
		return 92;

	if (roundf(256.5f) != 257.0f)
		return 93;

	if (roundf(511.4f) != 511.0f)
		return 94;

	if (roundf(-256.5f) != -257.0f)
		return 95;

	if (roundf(65536.0f) != 65536.0f)
		return 96;

	if (roundf(65536.5f) != 65537.0f)
		return 97;

	if (roundf(-65536.5f) != -65537.0f)
		return 98;

	if (roundf(0.49999997f) != 0.0f)
		return 121;

	if (roundf(-0.49999997f) != 0.0f)
		return 122;

	if (roundf(8388609.0f) != 8388609.0f)
		return 123;

	if (roundf(16777213.0f) != 16777213.0f)
		return 124;

	if (roundf(16777215.0f) != 16777215.0f)
		return 125;

	if (roundf(-16777215.0f) != -16777215.0f)
		return 126;

	if (roundf(8388607.5f) != 8388608.0f)
		return 127;

	return 0;
}

