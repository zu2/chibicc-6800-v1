#include "common.h"

extern	int strcmp(char *s1, char *s2);

int main(int argc, char **argv)
{
	putstr("Hello, World\n");

	if (strcmp("Hello,","World")==0)
	  return 1;

	if (strcmp("Hello,","World")>0)
	  return 2;

	if (strcmp("Hello,","Hello,"))
	  return 3;

	if (strcmp("World","Hello,")==0)
	  return 4;

	if (strcmp("World","Hello,")<0)
	  return 5;


	return 0;
}
