#include "common.h"

int sum(int *p)
{
	unsigned char	s = 0;

	for (int i=0; i<10; i++){
		s += p[i];
	}
	return s;
}

int sub(int x)
{
	unsigned int	p[x];

	for (int i=0; i<10; i++){
		p[i] = i;
	}
  return sum(p);
}

int main(int argc, char **argv)
{
	return sub(10)!=45;
}
