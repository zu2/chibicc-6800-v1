//
//	float basic library
//

#include "common.h"
#include "floatcheck.h"


int main(int argc, char **argv)
{
	float	f,g,h,e,pzero,mzero,pinf,minf;

	pzero = to_float(pZERO);
	mzero = to_float(mZERO);
	pinf  = to_float(pINFINITY);
	minf  = to_float(mINFINITY);

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
	g = to_float(pINFINITY);
	e = f/g;

	if (e!=0.0)
		return 171;

	if (cmpfl(e,pZERO))
		return 172;

	if (!cmpfl(e,mZERO))
		return 173;

	f = 1.0f;
	g = to_float(mINFINITY);
	e = f/g;

	if (e!=0.0)
		return 181;

	if (!cmpfl(e,pZERO))
		return 182;

	if (cmpfl(e,mZERO))
		return 183;
	
	return 0;
}
