#include "common.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	print(atoi("+0"));
	print(atoi("-0"));
	print(atoi("1"));
	print(atoi("-1"));
	print(atoi("12345"));
	print(atoi("-12345"));
	print(atoi("32767"));
	print(atoi("-32767"));
	print(atoi("32768"));
	print(atoi("-32768"));

	return 0;
}
