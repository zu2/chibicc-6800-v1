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

  // Current tests assume undefined LHS/RHS evaluation order
  if (a[0]!=100)
    return 1;
  if (a[1]!=1)
    return 2;
  if (a[9]!=109)
    return 9;

	return 0;
}
