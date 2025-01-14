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
	

	float x1 = long2float(0xC0AD1746);
	float x2 = long2float(0x41300000);
	puthexl(0x40B2E8BA);putchar('\n');
	puthexf(x1+x2);putchar('\n');

	zp = long2float(0x00000000);
	zm = long2float(0x80000000);

	f = 1.0;
       	g = 2.0;
	h = -1.0;
	e = -2.0;

//      putstr("+0.0:");puthexf(zp);putchar('\n');
//      putstr("-0.0:");puthexf(zm);putchar('\n');

	// zero check
	if (zp != 0.0)
		return 1;
	if (zp+zp != 0.0)
		return 2;
	if (zp+zm != 0.0)
		return 3;
	if (zm+zp != 0.0)
		return 4;
	if (zm+zm != zm)
		return 5;

	// NaN check. write it later

	if (f+f != 2.0)
		return 41;
	if (f+g != 3.0)
		return 42;
	if (g+f != 3.0)
		return 43;

	if (f+h != 0.0)
		return 50;
	if (h+f != 0.0)
		return 51;
	if (f+e != -1.0)
		return 52;
	if (g+h != 1.0)
		return 53;

	f = 11.0;
	g = -10.0;
	h = 1.0;

	if (f+g != 1.0)
		return 61;
	if (g+f != 1.0)
		return 62;
	if (f+h != 12.0)
		return 63;
	if (g+h != -9.0)
		return 64;

	f = 0.1;
       	g = 0.2;
	h = -0.1;
	e = -0.2;
	if (f+g != 0.3)
		return 71;
	if (g+f != 0.3)
		return 72;
	if (f+h != 0.0)
		return 73;
	if (h+f != 0.0)
		return 74;
	if (f+e != -0.1)
		return 75;
	if (g+h != 0.1)
		return 76;
//	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putchar('\n');

	return 0;
}
