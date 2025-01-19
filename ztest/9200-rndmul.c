//
//	xorshift32
//

#include "common.h"

unsigned long rnd;

unsigned long xorshift32(void)
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
	unsigned int	j;
	unsigned long	x;

	rnd = 0x12345678;

	for (i=0; i<1000; i++){
		x = xorshift32();
		j = (x>>16) * (x & 0xffff);
	}
	if (rnd != 0xe1d909c5)
	  return 2;

	cpu_counter();
	return 0;
}

