//
//	xorshift32
//

#include "common.h"

unsigned long rnd;

unsigned long xorshift32()
{
	unsigned long t;
	t = rnd << 13;
	rnd = rnd ^ t;
	t = rnd >> 17;
	rnd = rnd ^ t;
	t = rnd << 5;
	rnd = rnd ^ t;

	return rnd;
}

int main(int argc, char **argv)
{
	int	i;

	rnd = 0x12345678;
	puthexl(rnd);putchar('\n');

	for (i=0; i<1000; i++){
		puthexl(xorshift32());putchar('\n');
		print(rnd>>16);
		print(rnd&0xffff);
	}
	return 0;
}
