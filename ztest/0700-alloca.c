#include "common.h"

int sub(x)
{
	unsigned char	i;
	unsigned char	sum = 0;
	unsigned char	*p = alloca(x);

	for (i=0; i<10; i++){
		p[i] = i;
	}
	for (i=0; i<10; i++){
		sum += p[i];
	}
	return sum;
}

int sub2(x)
{
  unsigned int **bp = 0x00;

  unsigned char *p = alloca(x);
  unsigned char *q = alloca(x);

  return 0;
}

int main(int argc, char **argv)
{
	if (sub(10)!=45)
    return 1;

  if (sub2(10)) {
    return 2;
  }

  return 0;
}
