#include "common.h"

int main(void) {
	int a = 1, b = 2;
	_Bool r;


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

	if (!(r==0))
	  return 13;
	if (!(r!=1))
	  return 14;

	return 0;
}
