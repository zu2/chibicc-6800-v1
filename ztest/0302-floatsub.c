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
	h = -1.0;
	e = -2.0;

//      putstr("+0.0:");puthexf(zp);putchar('\n');
//      putstr("-0.0:");puthexf(zm);putchar('\n');

	// zero check
	if (zp != 0.0)
		return 1;
	if (zp-zp != 0.0)
		return 2;
	if (zp-zm != 0.0)
		return 3;
	if (zm-zp != zm)
		return 4;
	if (zm-zm != 0.0)
		return 5;

	// NaN check. write it later

	if (f-f != 0.0)
		return 41;
	if (f-g != -1.0)
		return 42;
	if (g-f != 1.0)
		return 43;

	if (f-h != 2.0)
		return 50;
	if (h-f != -2.0)
		return 51;
	if (f-e != 3.0)
		return 52;
	if (g-h != 3.0)
		return 53;

	f = 11.0;
	g = -10.0;
	h = 1.0;

	if (f-g != 21.0)
		return 61;
	if (g-f != -21.0)
		return 62;
	if (f-h != 10.0)
		return 63;
	if (g-h != -11.0)
		return 64;

	f = 0.1;
       	g = 0.2;
	h = -0.1;
	e = -0.2;
	if (f-g != -0.1)
		return 71;
	if (g-f != 0.1)
		return 72;
	if (f-h != 0.2)
		return 73;
	if (h-f != -0.2)
		return 74;
	if (f-e != 0.3)
		return 75;
	if (g-h != 0.3)
		return 76;
//	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putchar('\n');

	// round to nearest even
	f = long2float(0x3f800000);
	g = long2float(0x33000000);
	if (cmpfl(f-g,0x3f800000))
		return 100;

	f = long2float(0x3f800000);
	g = long2float(0x33010000);
	if (cmpfl(f-g,0x3f7fffff))
		return 101;

	f = long2float(0x3f800000);
	g = long2float(0x32fe0000);
	if (cmpfl(f-g,0x3f800000))
		return 102;

	// below 2.0 the step size gets half as big
	f = long2float(0x40000000);
	g = long2float(0x33800000);
	if (cmpfl(f-g,0x40000000))
		return 103;

	f = long2float(0x3f800000);
	g = long2float(0x2b800000);
	if (cmpfl(f-g,0x3f800000))
		return 104;

	f = long2float(0x3f7fffff);
	g = long2float(0x33000000);
	if (cmpfl(f-g,0x3f7ffffe))
		return 105;

	f = long2float(0x3f800000);
	g = long2float(0x3f800000);
	if (cmpfl(f-g,0x00000000))
		return 106;

	// the sticky bits of the subtrahend must lower the result
	f = long2float(0x4204cca8);
	g = long2float(0x360b0051);
	if (cmpfl(f-g,0x4204cca7))
		return 107;

	f = long2float(0x38018018);
	g = long2float(0x34736688);
	if (cmpfl(f-g,0x38008cb1))
		return 108;

	f = long2float(0x492ca494);
	g = long2float(0x46ba1ed2);
	if (cmpfl(f-g,0x4926d39d))
		return 109;

	f = long2float(0x3a201d99);
	g = long2float(0x33b63388);
	if (cmpfl(f-g,0x3a2017e7))
		return 110;

	f = long2float(0x4f41b896);
	g = long2float(0x48122501);
	if (cmpfl(f-g,0x4f41b64d))
		return 111;

	f = long2float(0x4ad41377);
	g = long2float(0x4032a09c);
	if (cmpfl(f-g,0x4ad41371))
		return 112;

	f = long2float(0x4212b902);
	g = long2float(0x36092552);
	if (cmpfl(f-g,0x4212b901))
		return 113;

	f = long2float(0x0644dc43);
	g = long2float(0x0229d39b);
	if (cmpfl(f-g,0x0644326f))
		return 114;

	// G=1, R=0, S=1 at shift 4, 8, 13, 20 and 22
	f = long2float(0x3fc00000);
	g = long2float(0x3d800009);
	if (cmpfl(f-g,0x3fb7ffff))
		return 120;

	f = long2float(0x3fc00000);
	g = long2float(0x3b800081);
	if (cmpfl(f-g,0x3fbf7fff))
		return 121;

	f = long2float(0x3fc00000);
	g = long2float(0x39001001);
	if (cmpfl(f-g,0x3fbffbff))
		return 122;

	f = long2float(0x3fc00000);
	g = long2float(0x35880001);
	if (cmpfl(f-g,0x3fbffff7))
		return 123;

	f = long2float(0x3fc00000);
	g = long2float(0x34a00001);
	if (cmpfl(f-g,0x3fbffffd))
		return 124;

	return 0;
}
