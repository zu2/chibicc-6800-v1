//
//	float library
//

#include "common.h"

extern	float	sqrtf(float x);

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
	float	f;

	if (sqrtf(4.0) != 2.0)
		return 11;

	if (sqrtf(81.0) != 9.0)
		return 12;

	if (cmpfl(sqrtf(2.0),0x3FB504F3))
		return 13;

	if (cmpfl(sqrtf(1.05),0x3F832935))
		return 14;

	if (cmpfl(sqrtf(1.0e-38),0x1FEC1E4A))
		return 14;

	return 0;
}
