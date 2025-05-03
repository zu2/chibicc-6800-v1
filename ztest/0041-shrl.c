#include "common.h"

int main(int argc, char **argv)
{
	signed   char sc=1;
	unsigned char uc=1;
	signed   int  si=1;
	unsigned int  ui=1;
	signed   long sl=1;
	unsigned long ul=1;

	sc <<= 6;
	if (sc!=64)
	  return 1;

	sc >>= 6;
	if (sc!=1)
	  return 2;

	sc <<= 0;
	if (sc!=1)
	  return 3;

	sc >>= 0;
	if (sc!=1)
	  return 4;

	sc <<= 2;
	if (sc!=4)
	  return 3;

	sc >>= 2;
	if (sc!=1)
	  return 4;

	uc <<= 7;
	if (uc!=128)
	  return 11;

	uc >>= 7;
	if (uc!=1)
	  return 12;

	uc <<= 0;
	if (uc!=1)
	  return 13;

	uc >>= 0;
	if (sc!=1)
	  return 14;

	uc <<= 2;
	if (uc!=4)
	  return 13;

	uc >>= 2;
	if (sc!=1)
	  return 14;

	si <<= 14;
	if (si!=16384)
	  return 21;

	si >>= 14;
	if (si!=1)
	  return 22;

	si <<= 0;
	if (si!=1)
	  return 23;

	si >>= 0;
	if (si!=1)
	  return 24;

	ui <<= 14;
	if (ui!=16384)
	  return 31;

	ui >>= 14;
	if (ui!=1)
	  return 32;

	ui <<= 0;
	if (ui!=1)
	  return 33;

	ui >>= 0;
	if (ui!=1)
	  return 34;

	sl <<= 14;
	if (sl!=16384)
	  return 41;

	sl >>= 14;
	if (sl!=1)
	  return 42;

	sl <<= 0;
	if (sl!=1)
	  return 43;

	sl >>= 0;
	if (sl!=1)
	  return 44;

	ul <<= 15;
	if (ul!=32768)
	  return 51;

	ul >>= 15;
	if (ul!=1)
	  return 52;

	ul <<= 0;
	if (ul!=1)
	  return 53;

	ul >>= 0;
	if (ul!=1)
	  return 54;

	return 0;
}
