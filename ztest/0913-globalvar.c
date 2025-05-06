//#include "common.h"

unsigned char uc = 1;
  signed char sc = 2;
unsigned int  ui = 3;
  signed int  si = 4;
unsigned long ul = 5;
  signed long sl = 6;

unsigned char ua1[1];
unsigned char ua2[1];

int abs(int x)
{
  return x<0? -x: x;
}

int
main(int argc, char **argv)
{
	ua1[0] = 7;
	ua2[0] = 8;

	if (sc-uc!=1)
	  return 1;

	if (si-ui!=1)
	  return 51;

	if (sl-ul!=1)
	  return 101;

	if (ua2[0]-ua1[0]!=1)
	  return 151;

	if (abs(ua2-ua1)!=1)
	  return 152;

	if (abs(&ua2[0]-&ua1[0])!=1)
	  return 153;

	return 0;
}
