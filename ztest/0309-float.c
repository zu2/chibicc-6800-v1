//
//	float basic library
//

#include "common.h"

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
	float	f,g,h,e;
	float	xo,xn;

	xn = (xo = 10.0) + 1.0;

	if (xo!=10.0)
		return 1;
	if (xn!=11.0)
		return 2;

	if (!xo)
		return 3;

	return 0;
}
