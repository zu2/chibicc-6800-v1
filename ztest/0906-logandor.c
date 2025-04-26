#include "common.h"

int main(void) {
	int a = 1, b = 2;
	int r;


	r = (a==b) || !(a==b);
	if (r!=1)
	  return 1;

	r = !(a==b) || (a==b);
	if (r!=1)
	  return 2;

	r = (a==b) && !(a==b);
	if (r!=0)
	  return 11;

	r = !(a==b) && (a==b);
	if (r!=0)
	  return 12;

	return 0;
}
