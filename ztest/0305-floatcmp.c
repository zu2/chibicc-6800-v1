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

	f = 1.0;
       	g = 2.0;
	h = 0.0;

	// EQ
	putstr("0.0==0.0:\t");print(0.0==0.0);
	putstr("0.0==0.0=\t");print(h==h);
	putstr("1.0==2.0:\t");print(1.0==2.0);
	putstr("1.0==2.0=\t");print(f==g);
	putstr("2.0==1.0:\t");print(2.0==1.0);
	putstr("2.0==1.0:\t");print(g==f);
	putchar('\n');
	// NE
	putstr("0.0!=0.0:\t");print(0.0!=0.0);
	putstr("0.0!=0.0=\t");print(h!=h);
	putstr("1.0!=2.0:\t");print(1.0!=2.0);
	putstr("1.0!=2.0=\t");print(f!=g);
	putstr("2.0!=1.0:\t");print(2.0!=1.0);
	putstr("2.0!=1.0=\t");print(g!=f);
	putchar('\n');
	// LT
	putstr("0.0<0.0:\t");print(0.0<0.0);
	putstr("0.0<0.0=\t");print(h<h);
	putstr("1.0<2.0:\t");print(1.0<2.0);
	putstr("1.0<2.0=\t");print(f<g);
	putstr("2.0<1.0:\t");print(2.0<1.0);
	putstr("2.0<1.0=\t");print(g<f);
	putchar('\n');
	// GT
	putstr("0.0>0.0:\t");print(0.0>0.0);
	putstr("0.0>0.0=\t");print(h>h);
	putstr("1.0>2.0:\t");print(1.0>2.0);
	putstr("1.0>2.0=\t");print(f>g);
	putstr("2.0>1.0:\t");print(2.0>1.0);
	putstr("2.0>1.0=\t");print(g>f);
	putchar('\n');
	// LE
	putstr("0.0<=0.0:\t");print(0.0<=0.0);
	putstr("0.0<=0.0=\t");print(h<=h);
	putstr("1.0<=2.0:\t");print(1.0<=2.0);
	putstr("1.0<=2.0=\t");print(f<=g);
	putstr("2.0<=1.0:\t");print(2.0<=1.0);
	putstr("2.0<=1.0=\t");print(g<=f);
	putchar('\n');
	// GE
	putstr("0.0>=0.0:\t");print(0.0>=0.0);
	putstr("0.0>=0.0=\t");print(h>=h);
	putstr("1.0>=2.0:\t");print(1.0>=2.0);
	putstr("1.0>=2.0=\t");print(f>=g);
	putstr("2.0>=1.0:\t");print(2.0>=1.0);
	putstr("2.0>=1.0=\t");print(g>=f);
	putchar('\n');
	return 0;
}
