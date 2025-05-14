#include "common.h"
#include <string.h>

int main(int argc, char **argv)
{
	char p[100];
	p[0] = 0;

  strcpy(p,"Hello, World.");
	strcat(p," OK?\n");
	putstr(p);

	return strcmp(p, "Hello, World. OK?\n");
}
