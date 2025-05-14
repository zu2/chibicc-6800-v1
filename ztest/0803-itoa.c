#include "common.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	char *p;
	char q[100];

	p = itoa(-32501,q,10);
	putstr(p); putchar('\n');

	return strcmp(p,"-32501");
}
