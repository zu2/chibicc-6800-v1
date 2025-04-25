#include "common.h"

long tarai(long x, long y, long z)
{
	if (x>y){
		return tarai(
			tarai(x-1,y,z),
			tarai(y-1,z,x),
			tarai(z-1,x,y));
	}
	return y;
}

int main(int argc, char **argv)
{
//	if(tarai(13,7,0)!=13)		// call 91924989 times
//		return 1;
	if(tarai(12,6,0)!=12)		// call 12604861 times
		return 1;
//	if(tarai(8,4,0)!=8)		// call 12605 times
//		return 1;

	cpu_counter();
	return 0;
}
