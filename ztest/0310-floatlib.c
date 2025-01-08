//
//	float library
//

#include "common.h"

extern	float	fabs(float x);

int
cmpf(float f, float g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	puthexf(f);putchar(' ');
	puthexf(g);putchar('\n');
#if 0
	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
#endif
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
#if	0
	putstr("1.0f:\t");puthexf(f);putchar('\n');
	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putchar('\n');
#endif

	return 0;
}
