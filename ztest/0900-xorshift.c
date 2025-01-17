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

	for (i=0; i<1000; i++){
		xorshift32();
		if (i==500 && rnd!=0x846802a6)
			return 1;
	}
	if (rnd != 0xe1d909c5)
	  return 2;

	cpu_counter();
	return 0;
}

