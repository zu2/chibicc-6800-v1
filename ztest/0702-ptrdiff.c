#include "common.h"


int main(int argc, char **argv)
{
	char	p,q;
	int	r,s;
	long	t,u;
	float	v,w;

	if (&p-&q!=1)
	  return 1;
	if (&r-&s!=1)
	  return 2;
	if (&t-&u!=1)
	  return 3;
	if (&v-&w!=1)
	  return 4;

	return 0;
}
