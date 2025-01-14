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

float long2float(unsigned long x)
{
	return	*((float *)&x);
}

int main(int argc, char **argv)
{
	float	f,g,h,e,zp,zm;
	
	zp = long2float(0x00000000);
	zm = long2float(0x80000000);

	float x = long2float(0x3FB505A8);
//	float x = long2float(0x3FB504F3);
	puthexf(x*x);putchar('\n');
#if 1
	f = 1.0;
       	g = 2.0;
	h = -1.0;
	e = -2.0;

	// multiply 0.0
	putstr("0.0*0.0:\t");puthexf(zp*zp);putchar('\n');
	if (zp*zp != 0.0)
		return 1;
	if (zp*zm != 0.0)
		return 2;
	if (zm*zp != 0.0)
		return 3;
	if (zm*zm != 0.0)
		return 4;

	// TODO: NaN check
	// write it later

	// multiply 1.0
	if (f*g != 2.0)
		return 20;
	if (g*f != 2.0)
		return 21;
	if (f*e != -2.0)
		return 22;
	if (e*f != -2.0)
		return 23;
	if (h*g != -2.0)
		return 25;
	if (g*h != -2.0)
		return 26;
	if (h*e != 2.0)
		return 27;
	if (e*h != 2.0)
		return 28;

	f = 0.1;
       	g = 0.3;
	h = -0.1;
	e = -0.3;
	putstr("0.1:\t");puthexf(f);putchar('\n');
	putstr("0.3:\t");puthexf(g);putchar('\n');
	putstr("0.1*0.3:\t");puthexf(0.03);putchar('\n');
	putstr("0.1*0.3=\t");puthexf(f*g);putchar('\n');
	putstr("0.3*0.1=\t");puthexf(g*f);putchar('\n');
	putstr("0.1*(-0.1):\t");puthexf(-0.01);putchar('\n');
	putstr("0.1*(-0.1)=\t");puthexf(f*h);putchar('\n');
	putstr("(-0.1)*0.1=\t");puthexf(h*f);putchar('\n');
	putstr("-0.03:\t");puthexf(-0.03);putchar('\n');
	putstr("0.1*(-0.3)=\t");puthexf(f*e);putchar('\n');
	putstr("0.3*(-0.1)=\t");puthexf(g*h);putchar('\n');

	f = 2.0;
	g = 10.0;
	h = 0.1;
	putstr("2.0*2.0:\t");puthexf(4.0);putchar('\n');
	putstr("2.0*2.0=\t");puthexf(f*f);putchar('\n');
	putstr("10.0*10.0:\t");puthexf(100.0);putchar('\n');
	putstr("10.0*10.0=\t");puthexf(g*g);putchar('\n');
	putstr("10.0*0.1:\t");puthexf(1.0);putchar('\n');
	putstr("10.0*0.1=\t");puthexf(g*h);putchar('\n');
	putstr("10.0*0.1*2.0:\t");puthexf(2.0);putchar('\n');
	putstr("10.0*0.1*2.0=\t");puthexf(g*h*f);putchar('\n');
#endif
	return	0;
}
