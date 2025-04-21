#include "common.h"

int
main(int argc, char **argv)
{
	int  a[10];
	int  b[10];
	int  c;
	float d = 1.0;

	a[0] = b[0] = 999;

	a[(int)(d-1)] = b[(int)(d-1)] = 555;

	if (a[0]!=555)
	  return 1;

	if (b[0]!=555)
	  return 2;

	return 0;
}
