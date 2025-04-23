#include "common.h"

int def;
int *ptr;

void set_ptr(int *p) {
        ptr = p ? p : &def;
}

int
main(int argc, char **argv)
{
	int abc;

	set_ptr(&abc);
	if (ptr!=&abc)
	  return 1;

	set_ptr(0);
	if (ptr!=&def)
	  return 2;

	return 0;
}
