#include "common.h"

int	count = 0;

int rsub()
{
	return ++count;
}

int lsub()
{
	return ++count;
}

int
main(int argc, char **argv)
{
	int  a[10];
	int  b[10];
	int d = 1;
	int x,y;

	a[0] = b[0] = 999;

	a[d-1] = b[d-1] = 555;

	if (a[0]!=555)
	  return 1;

	if (b[0]!=555)
	  return 2;

	x = 0;
	y = 0;
	x += y += 1;

	if (x!=1)
          print(x);
	if (x!=1)
	  return 11;
	if (y!=1)
	  return 12;

	a[d-1] += b[d-1] += 1;

	if (a[0]!=556+555)
	  return 31;

	if (b[0]!=556)
	  return 32;

	a[lsub()] = rsub();

	print(a[0]);
	print(a[1]);	// ← if 2, error
	print(a[2]);	// ← must 1


	return 0;
}
