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
	h = 4.0;
	e = 0.5;

	putstr("1.0/2.0:\t");puthexf(0.5);putchar('\n');
	putstr("1.0/2.0=\t");puthexf(f/g);putchar('\n');
	putstr("2.0/1.0:\t");puthexf(2.0);putchar('\n');
	putstr("2.0/1.0=\t");puthexf(g/f);putchar('\n');
	putstr("4.0/1.0:\t");puthexf(4.0);putchar('\n');
	putstr("4.0/1.0=\t");puthexf(h/f);putchar('\n');
	putstr("4.0/0.5:\t");puthexf(8.0);putchar('\n');
	putstr("4.0/0.5=\t");puthexf(h/e);putchar('\n');
	putchar('\n');

	f = 1.0;
       	g = -2.0;
	h = -4.0;
	e = -0.5;
	putstr("1.0/-2.0:\t");puthexf(-0.5);putchar('\n');
	putstr("1.0/-2.0=\t");puthexf(f/g);putchar('\n');
	putstr("-2.0/1.0:\t");puthexf(-2.0);putchar('\n');
	putstr("-2.0/1.0=\t");puthexf(g/f);putchar('\n');
	putstr("-4.0/1.0:\t");puthexf(-4.0);putchar('\n');
	putstr("-4.0/1.0=\t");puthexf(h/f);putchar('\n');
	putstr("-4.0/-0.5:\t");puthexf(8.0);putchar('\n');
	putstr("-4.0/-0.5=\t");puthexf(h/e);putchar('\n');
	putchar('\n');
#endif

	f = 1.0;
       	g = 0.3;
	h = 0.003;
	e = 9001.5;
	putstr("1.0/0.3:\t");puthexf(3.33333333333333);putchar('\n');
	putstr("1.0/0.3=\t");puthexf(f/g);putchar('\n');
	putstr("1.0/0.003:\t");puthexf(333.333333333333);putchar('\n');
	putstr("1.0/0.003=\t");puthexf(f/h);putchar('\n');
	putstr("0.3/0.003:\t");puthexf(100.0);putchar('\n');
	putstr("0.3/0.003=\t");puthexf(g/h);putchar('\n');
	putstr("0.003/9001.5:\t");puthexf(3.33277787035494e-07);putchar('\n');
	putstr("0.003/9001.5=\t");puthexf(h/e);putchar('\n');
	putchar('\n');
}
