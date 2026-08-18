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

int
is_nan(float f)
{
	return	f != f;
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

	// round to nearest even
	f = long2float(0x3f800000);
	g = long2float(0x33800000);
	if (cmpfl(f+g,0x3f800000))
		return 100;

	f = long2float(0x3f800001);
	g = long2float(0x33800000);
	if (cmpfl(f+g,0x3f800002))
		return 101;

	f = long2float(0x3f800000);
	g = long2float(0x33810000);
	if (cmpfl(f+g,0x3f800001))
		return 102;

	f = long2float(0x3f800000);
	g = long2float(0x337e0000);
	if (cmpfl(f+g,0x3f800000))
		return 103;

	// the tie carries into the exponent
	f = long2float(0x3fffffff);
	g = long2float(0x33800000);
	if (cmpfl(f+g,0x40000000))
		return 104;

	f = long2float(0x3f800000);
	g = long2float(0x2b800000);
	if (cmpfl(f+g,0x3f800000))
		return 105;

	f = long2float(0x7f7fffff);
	g = long2float(0x73000000);
	if (f+g != __builtin_inff())
		return 106;

	f = long2float(0x7f7fffff);
	g = long2float(0x72fffffe);
	if (cmpfl(f+g,0x7f7fffff))
		return 107;

	// the sticky bits of the addend must raise the result
	f = long2float(0x3df7141e);
	g = long2float(0x3938913e);
	if (cmpfl(f+g,0x3df77067))
		return 108;

	f = long2float(0x4a77549d);
	g = long2float(0x47a902b1);
	if (cmpfl(f+g,0x4a7c9cb3))
		return 109;

	f = long2float(0x2a8835b5);
	g = long2float(0x26f9999a);
	if (cmpfl(f+g,0x2a892f4f))
		return 110;

	f = long2float(0x4a77549d);
	g = long2float(0x47a902b1);
	if (cmpfl(f+g,0x4a7c9cb3))
		return 111;

	f = long2float(0x46289a7d);
	g = long2float(0x3f0de70d);
	if (cmpfl(f+g,0x46289cb5))
		return 112;

	f = long2float(0x33cd3348);
	g = long2float(0x29a8518a);
	if (cmpfl(f+g,0x33cd3353))
		return 113;

	f = long2float(0x3cf0dbe6);
	g = long2float(0x32124b24);
	if (cmpfl(f+g,0x3cf0dbeb))
		return 114;

	f = long2float(0x036b821c);
	g = long2float(0x00076052);
	if (cmpfl(f+g,0x036bbd1f))
		return 115;

	// G=1, R=0, S=1 at shift 4, 8, 13, 20 and 22
	f = long2float(0x3f800000);
	g = long2float(0x3d800009);
	if (cmpfl(f+g,0x3f880001))
		return 120;

	f = long2float(0x3f800000);
	g = long2float(0x3b800081);
	if (cmpfl(f+g,0x3f808001))
		return 121;

	f = long2float(0x3f800000);
	g = long2float(0x39001001);
	if (cmpfl(f+g,0x3f800401))
		return 122;

	f = long2float(0x3f800000);
	g = long2float(0x35880001);
	if (cmpfl(f+g,0x3f800009))
		return 123;

	f = long2float(0x3f800000);
	g = long2float(0x34a00001);
	if (cmpfl(f+g,0x3f800003))
		return 124;

	// a NaN operand or Inf + (-Inf) gives a NaN
	f = __builtin_nanf("");
	g = 1.0;
	if (!is_nan(f+g))
		return 130;
	if (!is_nan(g+f))
		return 131;
	if (!is_nan(f+f))
		return 132;

	f = long2float(0xffc00000);
	if (!is_nan(f+g))
		return 133;

	f = long2float(0x7fa00000);
	if (!is_nan(f+g))
		return 134;

	f = __builtin_inff();
	g = -__builtin_inff();
	if (!is_nan(f+g))
		return 135;
	if (!is_nan(g+f))
		return 136;

	f = __builtin_nanf("");
	if (f == f)
		return 137;
	if (f < 1.0)
		return 138;
	if (f > 1.0)
		return 139;

	// Inf keeps its sign, and an overflow reaches Inf
	f = __builtin_inff();
	g = 1.0;
	if (cmpfl(f+g,0x7f800000))
		return 140;

	f = -__builtin_inff();
	if (cmpfl(f+g,0xff800000))
		return 141;
	if (cmpfl(f+f,0xff800000))
		return 142;

	f = long2float(0x7f7fffff);
	if (cmpfl(f+f,0x7f800000))
		return 143;

	f = long2float(0xff7fffff);
	if (cmpfl(f+f,0xff800000))
		return 144;

	// only -0.0 + -0.0 keeps the sign
	f = long2float(0x80000000);
	g = long2float(0x00000000);
	if (cmpfl(f+g,0x00000000))
		return 145;
	if (cmpfl(g+f,0x00000000))
		return 146;
	if (cmpfl(f+f,0x80000000))
		return 147;

	// subnormal addition is exact
	f = long2float(0x00000001);
	if (cmpfl(f+f,0x00000002))
		return 148;

	return 0;
}
