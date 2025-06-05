#include "common.h"

int
main(int argc, char **argv)
{
	int  a[10];
	int  b[10];
	int  c = 1;
	long  d = 1;

	a[0] = b[0] = 999;

	a[d-1] = b[d-1] = 555;

	if (a[0]!=555)
	  return 1;

	if (b[0]!=555)
	  return 2;

	a[0] = b[0] = 0;

	if (a[0]!=0)
	  return 11;

	if (b[0]!=0)
	  return 12;

	return 0;
}
