#include "common.h"

extern char *strcpy(char *p, char *q);
extern int   strcmp(char *p, char *q);
extern char *_itoa(int n, char *buffer, int radix);

int main(int argc, char **argv)
{
	char *p;
	char q[100];

	p = _itoa(1234,q,10);
	if (strcmp(p,"1234"))
	  return 1;

	p = _itoa(-1234,q,10);
	if (strcmp(p,"-1234"))
	  return 2;

	p = _itoa(0,q,10);
	if (strcmp(p,"0"))
	  return 3;

	p = _itoa(32767,q,10);
	if (strcmp(p,"32767"))
	  return 4;

	p = _itoa(-32768,q,10);
	if (strcmp(p,"-32768"))
	  return 5;


	p = _itoa(255,q,16);
	if (strcmp(p,"ff"))
	  return 11;

	p = _itoa(32767,q,16);
	if (strcmp(p,"7fff"))
	  return 12;

	p = _itoa(-32768,q,16);
	if (strcmp(p,"8000"))
	  return 13;

	p = _itoa(0,q,16);
	if (strcmp(p,"0"))
	  return 14;

	p = _itoa(-1,q,16);
	if (strcmp(p,"ffff"))
	  return 15;

	p = _itoa(255,q,2);
	if (strcmp(p,"11111111"))
	  return 21;

	p = _itoa(32767,q,2);
	if (strcmp(p,"111111111111111"))
	  return 22;

	p = _itoa(-32768,q,2);
	if (strcmp(p,"1000000000000000"))
	  return 22;

	p = _itoa(-1,q,2);
	if (strcmp(p,"1111111111111111"))
	  return 23;

	p = _itoa(1234,q,36);
	if (strcmp(p,"ya"))
	  return 31;

	if (_itoa(0,q,-1))
	  return 201;
	if (_itoa(0,q,0))
	  return 202;
	if (_itoa(0,q,1))
	  return 203;
	if (_itoa(0,q,37))
	  return 204;

	return 0;
}
