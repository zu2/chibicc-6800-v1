#include "common.h"

int isxdigit(ch)
{
	return	(ch>='0' && ch<='9')
	||	(ch>='A' && ch<='F')
	||	(ch>='a' && ch<='f');
}

int main(int argc, char **argv)
{
	char *p = "Hello, World\n";

	while (*p){
		if (isxdigit(*p))
			putchar(*p);
		p++;
	}
	putchar('\n');

	return isxdigit('@');
}
