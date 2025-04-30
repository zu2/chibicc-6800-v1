#include "common.h"

int main(int argc, char **argv)
{
	unsigned char	uc;
	signed   char	sc;
	unsigned int	ui;
	signed	 int	si;
	unsigned long	ul;
	signed	 long	sl;


//	if (-0x10000)	return 1;	long??
	if (~0xffff)	return 2;

	uc = ~0xff;
	if (uc)		return 1;

	uc = -0x100;
	if (uc)		return 2;

	sc = ~0xff;
	if (sc)		return 41;

	sc = -0x100;
	if (sc)		return 42;

	ui = ~0xffff;
	if (ui)		return 81;

	ui = -0x10000;
	if (ui)		return 82;

	si = ~0xffff;
	if (si)		return 121;

	si = -0x10000;
	if (si)		return 122;

	ul = ~0xffffffffL;
	if (ul)		return 161;

	ul = -0x100000000L;
	if (ul)		return 162;

	sl = ~0xffffffffL;
	if (sl)		return 201;

	sl = -0x100000000L;
	if (sl)		return 202;

	return 0;
}
