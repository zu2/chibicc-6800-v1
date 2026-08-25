//
//	float basic library
//

#include "common.h"

#define HEX_FLT_TRUE_MIN   (0x00000001)
#define HEX_FLT_TRUE_MIN2  (0x00000002)
#define HEX_FLT_MIN        (0x00800000)
#define HEX_FLT_MIN_DIV2   (0x00400000)
#define HEX_FLT_MIN_DIV4   (0x00200000)
#define HEX_FLT_MIN_DIV8   (0x00100000)
#define pZERO		   (0x00000000)
#define mZERO		   (0x80000000)

int
is_nan(float f)
{
	return	f != f;
}

float to_float(unsigned long x)
{
	return	*((float *)&x);
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


int main(int argc, char **argv)
{
	float	f,g,h,e,pzero,mzero,pinf,minf;

	pzero = to_float(pZERO);
	mzero = to_float(mZERO);
	pinf  = __builtin_inff();
	minf  = -__builtin_inff();

	f = 1.0f;
       	g = 2.0f;
	h = 4.0f;
	e = -0.5f;

	if (pzero/f!=0.0f)
		return 1;
	if (mzero/f!=-0.0f)
		return 2;

	if (pzero/e!=0.0f)
		return 3;
	if (mzero/e!=0.0f)
		return 4;

	f = 1.0f;
       	g = 2.0f;
	h = 4.0f;
	e = 0.5f;

	if (f/g!=e)
		return 10;

	if (g/f!=g)
		return 11;

	if (h/f!=h)
		return 12;

	if (h/e!=8.0f)
		return 13;
//	putchar('\n');

	f = 1.0f;
       	g = -2.0f;
	h = -4.0f;
	e = -0.5f;
	if (f/g!=e)
		return 20;
	if (g/f!=g)
		return 21;
	if (h/f!=h)
		return 22;
	if (h/e!=8.0f)
		return 23;

	f = 1.0f;
       	g = 0.3f;
	h = 0.003f;
	e = 9001.5f;
	if (f/g!=3.33333333333333)
		return 31;

	f = 1.0f;
	g = to_float(HEX_FLT_MIN_DIV2);
	e = f/g;
	if (isinf(e))
		return 101;
	if (cmpfl(e,0x7F000000))
		return 102;

	f = 1.0f;
	g = to_float(HEX_FLT_MIN_DIV4);
	e = f/g;
	if (!isinf(e))
		return 111;

	f = -1.0f;
	g = to_float(HEX_FLT_MIN_DIV4);
	e = f/g;
	if (!isinf(e))
		return 121;

	f = to_float(HEX_FLT_MIN_DIV2);
	g = 2.0f;
	e = f/g;
	if (e != to_float(HEX_FLT_MIN_DIV4))
		return 131;
	e = e/g;
	if (e != to_float(HEX_FLT_MIN_DIV8))
		return 132;

	f = to_float(HEX_FLT_TRUE_MIN2);
	g = to_float(HEX_FLT_TRUE_MIN);
	e = f/g;
	if (e!=2.0f)
		return 141;


	f = to_float(HEX_FLT_MIN);
	g = 0.0;
	e = f/g;
	if (!isinf(e))
		return 151;

	f = to_float(HEX_FLT_TRUE_MIN);
	g = 0.0;
	e = f/g;
	if (!isinf(e))
		return 152;

	f = pzero;
	g = pzero;
	e = f/g;
	if (!isnan(e))
		return 161;

	f = mzero;
	g = mzero;
	e = f/g;
	if (!isnan(e))
		return 162;

	f = 1.0f;
	g = __builtin_inff();
	e = f/g;

	if (e!=0.0)
		return 171;

	if (cmpfl(e,pZERO))
		return 172;

	if (!cmpfl(e,mZERO))
		return 173;

	f = 1.0f;
	g = -__builtin_inff();
	e = f/g;

	if (e!=0.0)
		return 181;

	if (!cmpfl(e,pZERO))
		return 182;

	if (cmpfl(e,mZERO))
		return 183;

	// round check. more test needed.
	f = 2.5f;
	g = 3.0f;
	e = f/g;
	if (e!=0.8333333f)
		return 201;

	f = 2.9999999f;
	g = 3.0f;
	e = f/g;
	if (e != 0.99999999f)
		return 202;
	
	// round to nearest even
	f = 1.0f;
	g = 3.0f;
	if (cmpfl(f/g,0x3eaaaaab))
		return 210;

	f = 2.0f;
	g = 3.0f;
	if (cmpfl(f/g,0x3f2aaaab))
		return 211;

	f = 1.0f;
	g = 7.0f;
	if (cmpfl(f/g,0x3e124925))
		return 212;

	f = 1.0f;
	g = 25.0f;
	if (cmpfl(f/g,0x3d23d70a))
		return 213;

	f = 3.0f;
	g = 19.0f;
	if (cmpfl(f/g,0x3e21af28))
		return 214;

	// the answer is half of the smallest subnormal
	f = to_float(HEX_FLT_MIN);
	g = to_float(0x4b800000);
	if (cmpfl(f/g,0x00000000))
		return 215;

	f = to_float(0x00c00000);
	g = to_float(0x4b000000);
	if (cmpfl(f/g,0x00000002))
		return 216;

	f = to_float(0x7f7fffff);
	g = 0.5f;
	if (f/g != __builtin_inff())
		return 217;

	// exp difference is -150, the quotient still reaches the smallest subnormal
	f = to_float(0x30c00000);
	g = to_float(0x7b800000);
	if (cmpfl(f/g,0x00000001))
		return 218;

	f = to_float(0x30880000);
	g = to_float(0x7b800000);
	if (cmpfl(f/g,0x00000001))
		return 219;

	f = to_float(0x30800000);
	g = to_float(0x7b800000);
	if (cmpfl(f/g,0x00000000))
		return 220;

	f = to_float(0x30780000);
	g = to_float(0x7b800000);
	if (cmpfl(f/g,0x00000000))
		return 221;

	// ties in the subnormal range must go to the even neighbour
	f = to_float(0x04a00000);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000002))
		return 222;

	f = to_float(0x05100000);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000004))
		return 223;

	f = to_float(0x04e00000);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000004))
		return 224;

	f = to_float(0x04a01000);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000003))
		return 225;

	f = to_float(0x049ff000);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000002))
		return 226;

	// the bits shifted out while denormalizing are the only sticky bits
	f = to_float(0x05880800);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000009))
		return 227;

	f = to_float(0x07010200);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000041))
		return 228;

	f = to_float(0x09001080);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00000401))
		return 229;

	f = to_float(0x0b000120);
	g = to_float(0x4e800000);
	if (cmpfl(f/g,0x00004001))
		return 230;

	// Inf / zero is an Inf, and zero / Inf is a zero, both with the sign rule
	f = __builtin_inff();
	g = to_float(pZERO);
	if (cmpfl(f/g,0x7f800000))
		return 231;

	g = to_float(mZERO);
	if (cmpfl(f/g,0xff800000))
		return 232;

	f = -__builtin_inff();
	g = to_float(pZERO);
	if (cmpfl(f/g,0xff800000))
		return 233;

	g = to_float(mZERO);
	if (cmpfl(f/g,0x7f800000))
		return 234;

	f = to_float(pZERO);
	g = __builtin_inff();
	if (cmpfl(f/g,0x00000000))
		return 235;

	g = -__builtin_inff();
	if (cmpfl(f/g,0x80000000))
		return 236;

	f = to_float(mZERO);
	g = __builtin_inff();
	if (cmpfl(f/g,0x80000000))
		return 237;

	g = -__builtin_inff();
	if (cmpfl(f/g,0x00000000))
		return 238;

	// a NaN operand or Inf / Inf gives a NaN
	f = __builtin_nanf("");
	g = 1.0f;
	if (!is_nan(f/g))
		return 240;
	if (!is_nan(g/f))
		return 241;
	if (!is_nan(f/f))
		return 242;

	f = to_float(0xffc00000);
	if (!is_nan(f/g))
		return 243;

	f = to_float(0x7fa00000);
	if (!is_nan(f/g))
		return 244;

	f = __builtin_inff();
	if (!is_nan(f/f))
		return 245;

	g = -__builtin_inff();
	if (!is_nan(f/g))
		return 246;
	if (!is_nan(g/g))
		return 247;

	// a divide by zero keeps the sign of both operands
	f = 1.0f;
	g = to_float(0x00000000);
	if (cmpfl(f/g,0x7f800000))
		return 248;

	g = to_float(0x80000000);
	if (cmpfl(f/g,0xff800000))
		return 249;

	f = -1.0f;
	g = to_float(0x00000000);
	if (cmpfl(f/g,0xff800000))
		return 250;

	g = to_float(0x80000000);
	if (cmpfl(f/g,0x7f800000))
		return 251;

	// x / Inf is a zero with the sign rule
	f = 1.0f;
	g = -__builtin_inff();
	if (cmpfl(f/g,0x80000000))
		return 252;

	f = to_float(0x7f7fffff);
	g = to_float(0x00800000);
	if (cmpfl(f/g,0x7f800000))
		return 253;

	// underflow keeps the sign
	f = to_float(0x80000001);
	g = 2.0f;
	if (cmpfl(f/g,0x80000000))
		return 254;

	// exp difference is 128, but the left shift brings the exponent back to 127
	f = to_float(0x3f800000);
	g = to_float(0x002aaaab);
	if (cmpfl(f/g,0x7f3fffff))
		return 190;

	f = to_float(0x3f800001);
	g = to_float(0x003f8001);
	if (cmpfl(f/g,0x7f010203))
		return 191;

	// exp difference is 129, so the left shift still leaves 128
	f = to_float(0x3f800000);
	g = to_float(0x001fffff);
	if (cmpfl(f/g,0x7f800000))
		return 192;

	// the round up carries into the hidden bit, so the subnormal turns normal
	f = to_float(0x00ffffff);
	g = to_float(0x3fffffff);
	if (cmpfl(f/g,0x00800000))
		return 193;

	// the smallest normal falls to the largest subnormal
	f = to_float(0x00800000);
	g = to_float(0x3f800001);
	if (cmpfl(f/g,0x007fffff))
		return 194;

	// both operands are subnormal
	f = to_float(0x00000001);
	g = to_float(0x00000001);
	if (cmpfl(f/g,0x3f800000))
		return 195;

	return 0;
}
