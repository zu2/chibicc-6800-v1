/*
 *	varargs
 */

#include	"common.h"
#include	<stdarg.h>

int
sub(int argc, ...)
{
	va_list	ap;
	int i;

	va_start(ap,argc);
	if (argc!=1)
	  exit(1);
	i = va_arg(ap,int);
	if (i!=2)
	  exit(91);
	i = va_arg(ap,int);
	if (i!=3)
	  exit(92);
	
	return	argc!=1;
}

int main(int argc, char *argv[])
{
  return sub(1,2,3);
}

