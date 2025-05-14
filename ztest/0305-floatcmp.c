//
//	float basic library
//

#include "common.h"
#include "math.h"
#include "float.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

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

int main(int argc, char **argv)
{
	float	f,g,h,e;

	f = 1.0;
       	g = 2.0;
	h = 0.0;

	// EQ
	putstr("0.0==0.0:\t");print(0.0==0.0);
	putstr("0.0==0.0=\t");print(h==h);
	putstr("1.0==2.0:\t");print(1.0==2.0);
	putstr("1.0==2.0=\t");print(f==g);
	putstr("2.0==1.0:\t");print(2.0==1.0);
	putstr("2.0==1.0:\t");print(g==f);
	putchar('\n');
	// NE
	putstr("0.0!=0.0:\t");print(0.0!=0.0);
	putstr("0.0!=0.0=\t");print(h!=h);
	putstr("1.0!=2.0:\t");print(1.0!=2.0);
	putstr("1.0!=2.0=\t");print(f!=g);
	putstr("2.0!=1.0:\t");print(2.0!=1.0);
	putstr("2.0!=1.0=\t");print(g!=f);
	putchar('\n');
	// LT
	putstr("0.0<0.0:\t");print(0.0<0.0);
	putstr("0.0<0.0=\t");print(h<h);
	putstr("1.0<2.0:\t");print(1.0<2.0);
	putstr("1.0<2.0=\t");print(f<g);
	putstr("2.0<1.0:\t");print(2.0<1.0);
	putstr("2.0<1.0=\t");print(g<f);
	putchar('\n');
	// GT
	putstr("0.0>0.0:\t");print(0.0>0.0);
	putstr("0.0>0.0=\t");print(h>h);
	putstr("1.0>2.0:\t");print(1.0>2.0);
	putstr("1.0>2.0=\t");print(f>g);
	putstr("2.0>1.0:\t");print(2.0>1.0);
	putstr("2.0>1.0=\t");print(g>f);
	putchar('\n');
	// LE
	putstr("0.0<=0.0:\t");print(0.0<=0.0);
	putstr("0.0<=0.0=\t");print(h<=h);
	putstr("1.0<=2.0:\t");print(1.0<=2.0);
	putstr("1.0<=2.0=\t");print(f<=g);
	putstr("2.0<=1.0:\t");print(2.0<=1.0);
	putstr("2.0<=1.0=\t");print(g<=f);
	putchar('\n');
	// GE
	putstr("0.0>=0.0:\t");print(0.0>=0.0);
	putstr("0.0>=0.0=\t");print(h>=h);
	putstr("1.0>=2.0:\t");print(1.0>=2.0);
	putstr("1.0>=2.0=\t");print(f>=g);
	putstr("2.0>=1.0:\t");print(2.0>=1.0);
	putstr("2.0>=1.0=\t");print(g>=f);
	putchar('\n');

  if (to_float(0xff800000) < to_float(0xff000000))
    return 239;


  if (-NAN < f)
    return 240;
  return 0;

  f = 1.0;
  if (f < NAN)
    return 250;
  if (f <= NAN)
    return 251;
  if (f == NAN)
    return 252;
  if (f != NAN)
    return 253;
  if (f > NAN)
    return 254;
  if (f >= NAN)
    return 255;

	return 0;
}
