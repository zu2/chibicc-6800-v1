#include "common.h"
#include <stdlib.h>


int main(int argc, char **argv)
{
	char	p,q;
	int	r,s;
	long	t,u;
	float	v,w;

	if (abs(&p-&q)!=1)
	  return 1;
	if (abs(&r-&s)!=1)
	  return 2;
	if (abs(&t-&u)!=1)
	  return 3;
	if (abs(&v-&w)!=1)
	  return 4;

	return 0;
}
