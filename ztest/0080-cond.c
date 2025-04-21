#include "common.h"

int
main(int argc, char **argv)
{
	int a = 10;
	int b = 20;
	int c = 30;
	int d = 40;
	int e = 50;
	int f = 60;

	if (a==b && c==d)
	  return 1;

	if ((a==b) || (c==d))
	  return 2;

	if ((a==b) && (b==c) &&  (c==d))
	  return 3;

	if ((a==b) || (b==c) ||  (c==d))
	  return 3;

	if (((a==b) && (b==c)) ||  (c==d))
	  return 4;

	if (((a==b) || (b==c)) &&  (c==d))
	  return 5;

	return 0;
}
