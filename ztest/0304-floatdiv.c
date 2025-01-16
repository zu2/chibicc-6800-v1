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

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
	return 0;
}

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

float long2float(unsigned long x)
{
	return	*((float *)&x);
}

int main(int argc, char **argv)
{
	float	f,g,h,e,zp,zm;

	zp = long2float(0x00000000);
	zm = long2float(0x80000000);
	f = 1.0;
       	g = 2.0;
	h = 4.0;
	e = -0.5;

	if (zp/f!=0.0)
		return 1;
	if (zm/f!=-0.0)
		return 2;
//	putstr("+0.0/-0.5:\t");puthexf(0);putchar('\n');
//	putstr("+0.0/-0.5=\t");puthexf(zp/e);putchar('\n');
	if (zp/e!=0.0)
		return 3;
	if (zm/e!=0.0)
		return 4;

	// TODO: NaN check
	// write it later

	f = 1.0;
       	g = 2.0;
	h = 4.0;
	e = 0.5;

//	putstr("1.0/2.0:\t");puthexf(0.5);putchar('\n');
//	putstr("1.0:\t");puthexf(1.0);putchar('\n');
//	putstr("2.0:\t");puthexf(2.0);putchar('\n');
//	putstr("1.0/2.0=\t");puthexf(f/g);putchar('\n');
	if (f/g!=e)
		return 10;
//	putstr("2.0/1.0:\t");puthexf(2.0);putchar('\n');
//	putstr("2.0/1.0=\t");puthexf(g/f);putchar('\n');
	if (g/f!=g)
		return 11;
//	putstr("4.0/1.0:\t");puthexf(4.0);putchar('\n');
//	putstr("4.0/1.0=\t");puthexf(h/f);putchar('\n');
	if (h/f!=h)
		return 12;
//	putstr("4.0/0.5:\t");puthexf(8.0);putchar('\n');
//	putstr("4.0/0.5=\t");puthexf(h/e);putchar('\n');
	if (h/e!=8.0)
		return 13;
//	putchar('\n');

	f = 1.0;
       	g = -2.0;
	h = -4.0;
	e = -0.5;
//	putstr("1.0/-2.0:\t");puthexf(-0.5);putchar('\n');
//	putstr("1.0/-2.0=\t");puthexf(f/g);putchar('\n');
	if (f/g!=e)
		return 20;
//	putstr("-2.0/1.0:\t");puthexf(-2.0);putchar('\n');
//	putstr("-2.0/1.0=\t");puthexf(g/f);putchar('\n');
	if (g/f!=g)
		return 21;
//	putstr("-4.0/1.0:\t");puthexf(-4.0);putchar('\n');
//	putstr("-4.0/1.0=\t");puthexf(h/f);putchar('\n');
	if (h/f!=h)
		return 22;
//	putstr("-4.0/-0.5:\t");puthexf(8.0);putchar('\n');
//	putstr("-4.0/-0.5=\t");puthexf(h/e);putchar('\n');
	if (h/e!=8.0)
		return 23;
//	putchar('\n');

	f = 1.0;
       	g = 0.3;
	h = 0.003;
	e = 9001.5;
//	putstr("1.0/0.3:\t");puthexf(3.33333333333333);putchar('\n');
//	putstr("1.0/0.3=\t");puthexf(f/g);putchar('\n');
	if (f/g!=3.33333333333333)
		return 31;
//	putstr("1.0/0.003:\t");puthexf(333.333333333333);putchar('\n');
//	putstr("1.0/0.003=\t");puthexf(f/h);putchar('\n');
//	putstr("0.3/0.003:\t");puthexf(100.0);putchar('\n');
//	putstr("0.3/0.003=\t");puthexf(g/h);putchar('\n');
//	putstr("0.003/9001.5:\t");puthexf(3.33277787035494e-07);putchar('\n');
//	putstr("0.003/9001.5=\t");puthexf(h/e);putchar('\n');
//	putchar('\n');

	return 0;
}
