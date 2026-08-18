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
	if (isinf(e)!=1)
		return 112;

	f = -1.0f;
	g = to_float(HEX_FLT_MIN_DIV4);
	e = f/g;
	if (!isinf(e))
		return 121;
	if (isinf(e)!=-1)
		return 122;

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

	return 0;
}
