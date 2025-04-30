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

	sc = ~0xff;
	if (sc)		return 41;

	ui = ~0xffff;
	if (ui)		return 81;

	si = ~0xffff;
	if (si)		return 121;

	ul = ~0xffffffffL;
	if (ul)		return 161;

	sl = ~0xffffffffL;
	if (sl)		return 201;

	return 0;
}
