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

#if	0
	f = 1.0;
       	g = 2.0;
	h = -1.0;
	e = -2.0;

	putstr("1.0:\t");puthexf(1.0);putchar('\n');
	putstr("2.0:\t");puthexf(2.0);putchar('\n');
	putstr("3.0:\t");puthexf(3.0);putchar('\n');
	putstr("-1.0:\t");puthexf(-1.0);putchar('\n');
	putstr("-2.0:\t");puthexf(-2.0);putchar('\n');
	putstr("f:\t");puthexf(f);putchar('\n');
	putstr("g:\t");puthexf(g);putchar('\n');
	putstr("h:\t");puthexf(h);putchar('\n');
	putstr("1.0-2.0=\t");puthexf(f-g);putchar('\n');
	putstr("2.0-1.0=\t");puthexf(g-f);putchar('\n');
	putstr("1.0-(-1.0)=\t");puthexf(f-h);putchar('\n');
	putstr("(-1.0)-1.0=\t");puthexf(h-f);putchar('\n');
	putstr("-1.0:\t");puthexf(-1.0);putchar('\n');
	putstr("1.0-(-2.0)=\t");puthexf(f-e);putchar('\n');
	putstr("2.0-(-1.0)=\t");puthexf(g-h);putchar('\n');
#endif
	f = 10.0;
	g = 11.0;
	putstr("10.0-11.0:");puthexf(-1.0);putchar('\n');
	putstr("10.0-11.0=");puthexf(f-g);putchar('\n');
#if	0
	f = 0.1;
       	g = 0.2;
	h = -0.1;
	e = -0.2;
	putstr("0.1:\t");puthexf(0.1);putchar('\n');
	putstr("0.2:\t");puthexf(0.2);putchar('\n');
	putstr("0.3:\t");puthexf(0.3);putchar('\n');
	putstr("-0.1:\t");puthexf(-0.1);putchar('\n');
	putstr("f:\t");puthexf(f);putchar('\n');
	putstr("g:\t");puthexf(g);putchar('\n');
	putstr("h:\t");puthexf(h);putchar('\n');
	putstr("0.1-0.2:\t");puthexf(0.1-0.2);putchar('\n');
	putstr("0.1-0.2=\t");puthexf(f-g);putchar('\n');
	putstr("0.2-0.1:\t");puthexf(0.2-0.1);putchar('\n');
	putstr("0.2-0.1=\t");puthexf(g-f);putchar('\n');
	putstr("0.1-(-0.1)=\t");puthexf(f-h);putchar('\n');
	putstr("(-0.1)-0.1=\t");puthexf(h-f);putchar('\n');
	putstr("-0.1:\t");puthexf(-0.1);putchar('\n');
	putstr("0.1-(-0.2):\t");puthexf(0.1-(-0.2));putchar('\n');
	putstr("0.2-(-0.1):\t");puthexf(0.2-(-0.1));putchar('\n');
	putstr("0.1-(-0.2)=\t");puthexf(f-e);putchar('\n');
	putstr("0.2-(-0.1)=\t");puthexf(g-h);putchar('\n');
//	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putchar('\n');
#endif
}
