//
//	float library
//

#include "common.h"

extern	float	fabs(float x);

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

	f = 1.0;
	if (f != fabs(1.0))
		return 1;
	if (f != fabs(-1.0))
		return 2;
	if (f == fabs(0.0))
		return 3;

	return 0;
}
