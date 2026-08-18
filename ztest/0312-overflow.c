//
//	float overflow test
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
	float x1 = long2float(0x00800000);	// The smallest normalized number
	float x2 = long2float(0x00400000);	// The largest  subnomal   number
	float x3 = long2float(0x00000001);	// The smallest subnomal   number
	float zp = long2float(0x00000000);
	float zm = long2float(0x80000000);
	float pInf = long2float(0x7F800000);
	float mInf = long2float(0xFF800000);
	float one = long2float(0x3F800000);
	float two = long2float(0x40000000);

	float y1 = long2float(0x7F7FFFFF);	// The largest nomalized number

	if (pInf!=pInf)
		return	1;
	if (mInf!=mInf)
		return	2;
	if (mInf!=-pInf)
		return	3;
	if ((y1*2.0)!=pInf)
		return	21;
//	puthexf(y1);putchar('\n');
//	puthexf(y1*1.1);putchar('\n');
	if ((y1*1.1)!=pInf)
		return	22;


	if (y1*-2.0!=mInf)
		return	22;
	if (cmpfl(x1/2,0x00400000))
		return 1;
	if (cmpfl(x1/4,  0x00200000))
		return 2;
	if (cmpfl(x1/8,  0x00100000))
		return 3;
	if (cmpfl(x1/0x00800000,  0x00000001))
		return 23;
	if (cmpfl(x1/0x01000000,  0x00000000))
		return 24;

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

	if (x1-x2 != x2)
		return	51;
	if (x1-x2-x2 != 0.0)
		return	52;

	if (x1!=x2+x2)
		return	61;
	if (x1+x2!=x2+x2+x2)
		return	61;

	if (cmpfl(mInf+one, 0xFF800000))
		return	71;
	if (cmpfl(one+mInf, 0xFF800000))
		return	72;
	if (cmpfl(pInf+one, 0x7F800000))
		return	73;
	if (cmpfl(one+pInf, 0x7F800000))
		return	74;
	if (cmpfl(mInf+two, 0xFF800000))
		return	75;
	if (cmpfl(two+mInf, 0xFF800000))
		return	76;
	if (cmpfl(pInf+(-two), 0x7F800000))
		return	77;
	if (cmpfl((-two)+pInf, 0x7F800000))
		return	78;
	if (cmpfl(pInf+zm, 0x7F800000))
		return	79;
	if (cmpfl(zm+pInf, 0x7F800000))
		return	80;
	if (cmpfl(mInf+zp, 0xFF800000))
		return	81;
	if (cmpfl(zp+mInf, 0xFF800000))
		return	82;

	return 0;
}
