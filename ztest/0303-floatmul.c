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
	h = -1.0;
	e = -2.0;

	// multiply 0.0
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

	if (cmpfl(f*g,0x3CF5C290))
		return 30;
	if (cmpfl(g*f,0x3CF5C290))
		return 31;

	if (cmpfl(f*h,0xBC23D70B))
		return 32;
	if (cmpfl(h*f,0xBC23D70B))
		return 33;
	if (cmpfl(f*e,0xBCF5C290))
		return 34;
	if (cmpfl(e*f,0xBCF5C290))
		return 35;

	f = 2.0;
	g = 10.0;
	h = 0.1;
	if (f*f!=4.0)
		return 40;
	if (g*g!=100.0)
		return 41;
	if (g*h!=1.0)
		return 42;
	if (g*h*f!=2.0)
		return 43;
	// round to nearest even
	f = long2float(0x3f800800);
	g = long2float(0x3f800800);
	if (cmpfl(f*g,0x3f801000))
		return 100;

	f = long2float(0x3fc00000);
	g = long2float(0x3faaaaa9);
	if (cmpfl(f*g,0x3ffffffe))
		return 101;

	f = long2float(0x3fc00000);
	g = long2float(0x3faaaaa7);
	if (cmpfl(f*g,0x3ffffffa))
		return 102;

	f = long2float(0x3f800800);
	g = long2float(0x3f800802);
	if (cmpfl(f*g,0x3f801003))
		return 103;

	f = long2float(0x3f800800);
	g = long2float(0x3f8007fe);
	if (cmpfl(f*g,0x3f800ffe))
		return 104;

	// the answer is half of the smallest subnormal
	f = long2float(0x00800000);
	g = long2float(0x33800000);
	if (cmpfl(f*g,0x00000000))
		return 105;

	f = long2float(0x00800000);
	g = long2float(0x33c00000);
	if (cmpfl(f*g,0x00000001))
		return 106;

	f = long2float(0x00c00000);
	g = long2float(0x34000000);
	if (cmpfl(f*g,0x00000002))
		return 107;

	// underflow to subnormal must be rounded, not truncated
	f = long2float(0x002d81b2);
	g = long2float(0x3eeacaad);
	if (cmpfl(f*g,0x0014de4a))
		return 110;

	f = long2float(0x3b96fc81);
	g = long2float(0x000c48d8);
	if (cmpfl(f*g,0x00000e7e))
		return 111;

	f = long2float(0x001a4c50);
	g = long2float(0x388a4657);
	if (cmpfl(f*g,0x00000072))
		return 112;

	f = long2float(0x3c4f5681);
	g = long2float(0x020d0d5a);
	if (cmpfl(f*g,0x000e47af))
		return 113;

	// exp sum is -151, but the mantissa carry brings it back into subnormal
	f = long2float(0x1a400000);
	g = long2float(0x19c00000);
	if (cmpfl(f*g,0x00000001))
		return 114;

	f = long2float(0x1a700000);
	g = long2float(0x19f00000);
	if (cmpfl(f*g,0x00000001))
		return 115;

	// exp sum from -149 down to -152, the mantissa carry decides the result
	f = long2float(0x1a000000);
	g = long2float(0x1a800000);
	if (cmpfl(f*g,0x00000001))
		return 116;

	f = long2float(0x1a000000);
	g = long2float(0x1a000000);
	if (cmpfl(f*g,0x00000000))
		return 117;

	f = long2float(0x1a000001);
	g = long2float(0x1a000000);
	if (cmpfl(f*g,0x00000001))
		return 118;

	f = long2float(0x1a7fffff);
	g = long2float(0x1a7fffff);
	if (cmpfl(f*g,0x00000002))
		return 119;

	f = long2float(0x19ffffff);
	g = long2float(0x1a000000);
	if (cmpfl(f*g,0x00000000))
		return 120;

	f = long2float(0x19ffffff);
	g = long2float(0x1a000002);
	if (cmpfl(f*g,0x00000001))
		return 121;

	f = long2float(0x19ffffff);
	g = long2float(0x1a7fffff);
	if (cmpfl(f*g,0x00000001))
		return 122;

	f = long2float(0x19ffffff);
	g = long2float(0x19ffffff);
	if (cmpfl(f*g,0x00000000))
		return 123;

	// the guard byte holds the sticky bit only in b0
	f = long2float(0x3fc00100);
	g = long2float(0x3fc08100);
	if (cmpfl(f*g,0x40106181))
		return 124;

	f = long2float(0x3fc00300);
	g = long2float(0x53c02b00);
	if (cmpfl(f*g,0x54102281))
		return 125;

	f = long2float(0x3fc00500);
	g = long2float(0x21c14d00);
	if (cmpfl(f*g,0x2210fd87))
		return 126;

	return 0;
}

