#include <stdio.h>
#include <string.h>
#include <ctype.h>

int isxdigit_spec(ch)
{
	return	(ch>='0' && ch<='9')
	||	(ch>='A' && ch<='F')
	||	(ch>='a' && ch<='f');
}

int main(int argc, char **argv)
{
	char s0[] = "Hello, World\n";
  char s1[20];
  char s2[20];
  char *p,*q;

	for (p=s0,q=s1; *p; p++ ){
		if (isxdigit_spec(*p)) {
      *q = *p;
      q++;
    }
	}
  *q = '\0';
	for (p=s0,q=s2; *p; p++ ){
		if (isxdigit(*p)) {
      *q = *p;
      q++;
    }
	}
  *q = '\0';

	return strcmp(s1,s2);
}
