//
//	float library
//

#include "common.h"

extern	float	fabs(float x);
extern	float	floor(float x);
extern	float	ceil(float x);


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
	if (fabs(1.0f) != 1.0f)
		return 1;
	if (fabs(-1.0f) != 1.0f)
		return 2;
	if (fabs(0.0) == 1.0f)
		return 3;

	if (floor(0.0f) != 0.0f)
		return 11;

	if (floor(0.5f) != 0.0f)
		return 12;

	if (floor(1.0f) != 1.0f)
		return 13;

	if (floor(1.5f) != 1.0f)
		return 14;

	if (floor(2.5f) != 2.0f)
		return 15;

	if (floor(8388607.5f) != 8388607.0f)
		return 16;

	if (floor(8388608.0f) != 8388608.0f)
		return 17;

	if (-0.0f != 0.0f)
		return 20;
	if (floor(-0.0f) != 0.0f)
		return 21;

	if (floor(-0.5f) != -1.0f)
		return 22;

	if (floor(-1.0f) != -1.0f)
		return 23;

	if (floor(-1.5f) != -2.0f)
		return 24;

	if (floor(-2.5f) != -3.0f)
		return 25;

	if (floor(-8388607.5f) != -8388608.0f)
		return 26;

	if (floor(-8388608.0f) != -8388608.0f)
		return 27;

	if (ceil(0.0f) != 0.0f)
		return 51;

	if (ceil(0.5f) != 1.0f)
		return 52;

	if (ceil(1.0f) != 1.0f)
		return 53;

	if (ceil(1.5f) != 2.0f)
		return 54;

	if (ceil(2.5f) != 3.0f)
		return 55;

	if (ceil(8388607.5f) != 8388608.0f)
		return 56;

	if (ceil(8388608.0f) != 8388608.0f)
		return 57;

	if (-0.0f != 0.0f)
		return 60;
	if (ceil(-0.0f) != 0.0f)
		return 61;

	if (ceil(-0.5f) != 0.0f)
		return 62;

	if (ceil(-1.0f) != -1.0f)
		return 63;

	if (ceil(-1.5f) != -1.0f)
		return 64;

	if (ceil(-2.5f) != -2.0f)
		return 65;

	if (ceil(-8388607.5f) != -8388607.0f)
		return 66;

	if (ceil(-8388608.0f) != -8388608.0f)
		return 67;

	return 0;
}
