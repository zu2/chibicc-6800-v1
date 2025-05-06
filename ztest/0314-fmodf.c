//
//	float basic library
//

#include "common.h"
#include "floatcheck.h"


#if defined(__clang__) || defined(__GNUC__)
#else
#if 0
float fmodf(float x, float y){
 return x-truncf(x/y)*y;
}
#else
extern	float	fmodf(float x, float y);
extern	float	fmaxf(float x, float y);
extern	float	fminf(float x, float y);
#define	FLT_EPSILON	(0x1p-23f)
#endif
#endif

float my_fmodf(float x, float y)
{
	return x-truncf(x/y)*y;
}

int float_equals(float a, float b, float abs_tol, float rel_tol)
{
    float diff = fabsf(a - b);
    if (diff <= abs_tol) return 1; // 絶対誤差で合格

    float max_ab = fmaxf(fabsf(a), fabsf(b));
    if (diff <= max_ab * rel_tol) return 1; // 相対誤差で合格

    return 0; // どちらも満たさない
}


int main(int argc, char **argv)
{
	float	f,g,h,e,pzero,mzero,pinf,minf;

#if 0
	pzero = to_float(pZERO);
	mzero = to_float(mZERO);
	pinf  = to_float(pINFINITY);
	minf  = to_float(mINFINITY);

	f = 1.0f;
       	g = 2.0f;
	h = 4.0f;
	e = -0.5f;

	if (fmodf(pzero,f)!=0.0f)
		return 1;
	if (fmodf(mzero,f)!=-0.0f)
		return 2;

	if (fmodf(pzero,e)!=0.0f)
		return 3;
	if (fmodf(mzero,e)!=0.0f)
		return 4;

	f = 1.0f;
       	g = 2.0f;
	h = 4.0f;
	e = 0.5f;

	if (fmodf(f,g)!=f)		// f>g, then f
		return 10;

	puthexf(fmodf(g,f));putstr(" fmod(2.0f,1.0f)\n");
	puthexf(0.0f);putstr(" 0.0f\n");
	if (fmodf(g,f)!=0.0f)
		return 11;

	if (fmodf(h,f)!=0.0f)
		return 12;

	if (fmodf(h,e)!=0.0f)
		return 13;

	f = 1.0f;
       	g = -2.0f;
	h = -4.0f;
	e = -0.5f;
	if (fmodf(f,g)!=1.0f)
		return 20;
	if (fmodf(g,f)!=-0.0f)
		return 21;
	if (fmodf(h,f)!=-0.0f)
		return 22;
	if (fmodf(h,e)!=-0.0f)
		return 23;

#endif

	f = 1.0f;
       	g = 0.3f;
	h = 0.003f;
	e = 9001.5f;
	puthexf((float)fmodf(f,g));putstr(" fmod(1.0f,0.3f)\n");
#if 0
	puthexf(0.1f);putstr(" 0.1f\n");
//	if (fmodf(f,g)!=0.1f)
//		return 31;
#endif

	f = 2.0f;
       	g = 0.3f;
	puthexf((float)fmodf(f,g));putstr(" fmod(2.0f,0.3f)\n");
#if 0
	puthexf(0.2f);putstr(" 0.2f\n");
#endif

	f = 5.3f;
       	g = 0.1f;
	puthexf((float)fmodf(f,g));putstr(" fmod(5.3f,0.1f)\n");
	puthexf(my_fmodf(f,g));putstr(" my_fmodf(5.3f,0.1f)\n");
	puthexf(fabsf(fmodf(f,g)-my_fmodf(f,g))/my_fmodf(f,g));putstr(" error\n");
#if 0
	puthexf(0.2f);putstr(" 0.2f\n");
#endif

	f = 200.0f;
	g = 0.3f;
	puthexf((float)fmodf(f,g));putstr(" fmod(200.0f,0.3f)\n");
#if 0
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

#endif
	return 0;
}
