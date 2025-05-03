#include "common.h"

int func(void) {
	static int a[] = { 1 };
	return *a;
}

int main(int argc, char **argv)
{
	print(func());

	return func()!=1;
}
