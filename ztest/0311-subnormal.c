//
//	float subnormal test
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

//puthexf(f);putchar('\n');
//puthexl(g);putchar('\n');
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
	float x1 = long2float(0x00800000);	// The smallest normalized number
	float x2 = long2float(0x00400000);	// The largest  subnomal   number
	float x3 = long2float(0x00000001);	// The smallest subnomal   number
	float zp = long2float(0x00000000);
	float zm = long2float(0x80000000);
  float x4 = long2float(0x00000003);  // 1.1b*2^-148
  float x5 = long2float(0x3ec00000);  // 1.1b*2^-2

// test function check
  if (cmpf(x1,x1))
    return 1;
  if (cmpf(x1,x2)==0)
    return 2;

#if	1
//	puthexf(x1/2);putchar('\n');		// 00800000 / 40000000
	if (cmpfl(x1/2,0x00400000))
		return 11;
	if (cmpfl(x1/4,  0x00200000))
		return 12;
	if (cmpfl(x1/8,  0x00100000))
		return 13;
	if (cmpfl(x1/0x00800000,  0x00000001))
		return 23;
	if (cmpfl(x1/0x01000000,  0x00000000))
		return 24;

//	puthexf(x1*0.5);putchar('\n');
//	puthexf(x1*0.25);putchar('\n');
	if (cmpfl(x1*0.5,0x00400000))
		return 31;
	if (cmpfl(x1*0.25,  0x00200000))
		return 32;
	if (cmpfl(x1*0.125,  0x00100000))
		return 33;
	if (cmpfl(x1*.00000011920928955078,  0x00000001))
		return 43;
	if (cmpfl(x1*.00000005960464477539,  0x00000000))
		return 44;


//	puthexf(x2);putchar('\n');	
//	puthexf(x1-x2);putchar('\n');
//	puthexf(x1-x2-x2);putchar('\n');
	if (x1-x2 != x2)
		return	51;
	if (x1-x2-x2 != 0.0)
		return	52;

	if (x1!=x2+x2)
		return	61;
	if (x1+x2!=x2+x2+x2)
		return	61;
#endif

  // underflow check
	// puthexf(x4*x5);putchar('\n');
	// puthexf(x3);putchar('\n');

  // 1.1b*2^-126 * 1.1b*2^-2 → 1b*2^-127
  // Exponent sum appears to underflow, (-126 + -2 = -128 < -127)
  // but mantissa multiplication carry keeps it subnormal.
	if (cmpf(x4*x5, x3))
		return 101;

	return 0;
}
