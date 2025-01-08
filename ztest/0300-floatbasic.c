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



int main(int argc, char **argv)
{
	float	f;

	f = 1.0;
	if ((unsigned long)f != 1)
		return	1;
	f = 2.0;
	if ((unsigned long)f != 2)
		return	2;
	f = 1024.0;
	if ((unsigned long)f != 1024)
		return	10;
	f = 32767.0;
	if ((unsigned long)f != 32767)
		return	20;
	f = 32768.0;
	if ((unsigned long)f != 32768L)
		return	21;
	f = 65535.0;
	if ((unsigned long)f != 65535L)
		return	30;
	f = 65536.0;
	if ((unsigned long)f != 65536L)
		return	31;
	f = 65537.0;
	if ((unsigned long)f != 65537L)
		return	32;
	f = 16777215.0;
	if ((unsigned long)f != 16777215L)
		return	40;
	f = 16777216.0;
	if ((unsigned long)f != 16777216L)
		return	41;
	f = 4294967295.0;
	if ((unsigned long)f != 4294967295L)
		return	42;

	f = 1.0;
	if ((unsigned int)f != 1)
		return 51;
	f = 2.0;
	if ((unsigned int)f != 2)
		return	2;
	f = 1024.0;
	if ((unsigned int)f != 1024)
		return	10;
	f = 32767.0;
	if ((unsigned int)f != 32767)
		return	20;
	f = 32768.0;
	if ((unsigned int)f != 32768)
		return	21;
	f = 65535.0;
	if ((unsigned int)f != 65535)
		return	40;
	f = 65536.0;
	if ((unsigned int)f != 65535)
		return	41;

	f = 1.0;
	if ((long)f != 1)
		return	51;
	f = 2.0;
	if ((long)f != 2)
		return	52;
	f = -1.0;
	if ((long)f != -1)
		return	53;
	f = 1024.0;
	if ((long)f != 1024)
		return	60;
	f = -1024.0;
	if ((long)f != -1024)
		return	61;
	f = 32767.0;
	if ((long)f != 32767)
		return	70;
	f = 32768.0;
	if ((long)f != 32768L)
		return	71;
	f = -32767.0;
	if ((long)f != -32767)
		return	70;
	f = -32768.0;
	if ((long)f != -32768L)
		return	71;
	f = 65535.0;
	if ((long)f != 65535L)
		return	80;
	f = 65536.0;
	if ((long)f != 65536L)
		return	81;
	f = 65537.0;
	if ((long)f != 65537L)
		return	82;
	f = -65535.0;
	if ((long)f != -65535L)
		return	83;
	f = -65536.0;
	if ((long)f != -65536L)
		return	84;
	f = -65537.0;
	if ((long)f != -65537L)
		return	85;
	f = 16777215.0;
	if ((long)f != 16777215L)
		return	90;
	f = 16777216.0;
	if ((long)f != 16777216L)
		return	91;
	f = -16777215.0;
	if ((long)f != -16777215L)
		return	92;
	f = -16777216.0;
	if ((long)f != -16777216L)
		return	93;
	f = 2147483520.0;
	if ((long)f != 2147483520)
		return	94;
	f = 2147483648.0;
	if ((long)f != 2147483648)
		return	95;
	f = -2147483520.0;
	if ((long)f != -2147483520)
		return	97;
	f = -2147483648.0;
	if ((long)f != -2147483648)
		return	98;
#if	0
	putstr("1.0f:\t");puthexf(f);putchar('\n');
	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putchar('\n');
#endif

	return 0;
}
