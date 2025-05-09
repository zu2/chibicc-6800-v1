//
//	xorshift32
//

#include "common.h"
#include <stdlib.h>

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
	int	i,j;

	rnd = 0x12345678;

	for (i=0; i<1000; i++){
		xorshift32();
		if (i==500 && rnd!=0x846802a6)
			return 1;
	}
	if (rnd != 0xe1d909c5)
	  return 2;

	putstr("libc/rand\n");
	cpu_counter();
	for (i=0; i<10; i++) {
	  for (j=0; j<10000; j++) {
	    rand();
	  }
	}
	cpu_counter();

	putstr("xorshift32 C version\n");
	cpu_counter();
	for (i=0; i<10; i++) {
	  for (j=0; j<10000; j++) {
            xorshift32();
	  }
	}
	cpu_counter();

	cpu_counter();
	return 0;
}

