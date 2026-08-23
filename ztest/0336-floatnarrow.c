//
//	narrowing a cast from float must not drop the middle cast
//

#include "common.h"

float	g;
int	r;

int main(int argc, char **argv)
{
	g = 300.0f;
	if ((char)(long)g != 44)
		return	1;
	if ((char)(int)g != 44)
		return	2;
	if ((char)(short)g != 44)
		return	3;
	if ((char)(unsigned long)g != 44)
		return	4;
	if ((unsigned char)(long)g != 44)
		return	5;
	if ((unsigned char)(int)g != 44)
		return	6;

	if ((char)((long)g & 0xffL) != 44)
		return	10;
	if ((char)((long)g | 0L) != 44)
		return	11;
	if ((char)((long)g ^ 0L) != 44)
		return	12;
	if ((char)((int)g & 0xff) != 44)
		return	13;
	if ((char)((int)g | 0) != 44)
		return	14;
	if ((char)((int)g ^ 0) != 44)
		return	15;

	r = 1;
	if ((char)(r ? (long)g : 0L) != 44)
		return	20;
	r = 0;
	if ((char)(r ? 0L : (long)g) != 44)
		return	21;

	g = -300.0f;
	if ((char)(long)g != -44)
		return	30;
	if ((char)(int)g != -44)
		return	31;
	if ((char)((long)g & 0xffL) != -44)
		return	32;

	// 65536 and above needs 32 bits. a 16 bit int saturates to 32767
	g = 70000.0f;
	if (((long)g & 1L) != 0L)
		return	40;
	g = 70001.0f;
	if (((long)g & 1L) != 1L)
		return	41;

	return	0;
}
