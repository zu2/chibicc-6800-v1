#include "common.h"

//
// Checking evaluation order
//
int
main(int argc, char **argv)
{
	int  a[10];
	int  i;

	for (i=0; i<10; i++){
	   a[i] = i+100;
	}

	i = 0;
	a[i] = ++i;

	for (i=0; i<10; i++){
	  print(a[i]);
	}
// TODO: check C standardcheck C standard
#if 0
	if (a[0]!=100)
	  return 1;
	if (a[1]!=1)
	  return 2;
#endif

	return 0;
}
