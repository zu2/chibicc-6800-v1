//
//	float basic library
//

#include "common.h"

int main(int argc, char **argv)
{
	float	f;

	f = 1.0;
	if ((unsigned long)f != 1UL)
		return	1;
	f = 2.0;
	if ((unsigned long)f != 2UL)
		return	2;
	f = 1024.0;
	if ((unsigned long)f != 1024UL)
		return	10;
	f = 32767.0;
	if ((unsigned long)f != 32767UL)
		return	20;
	f = 32768.0;
	if ((unsigned long)f != 32768UL)
		return	21;
	f = 65535.0;
	if ((unsigned long)f != 65535UL)
		return	30;
	f = 65536.0;
	if ((unsigned long)f != 65536UL)
		return	31;
	f = 65537.0;
	if ((unsigned long)f != 65537UL)
		return	32;
	f = 16777215.0;
	if ((unsigned long)f != 16777215UL)
		return	40;
	f = 16777216.0;
	if ((unsigned long)f != 16777216UL)
		return	41;
  f = 2147483647.0;     // 2147483647.0 is rounded to 2147483648.0 in float.
	if ((unsigned long)f != 2147483648UL)
		return	41;
	f = 4294967295.0;     // 4294967295.0 is rounded to 4294967296.0 in float.
	if ((unsigned long)f != 0L)    // 4294967296.0 → 0UL
		return	48;
	f = 4294967296.0;
	if ((unsigned long)f != 4294967296UL)   // Undefined Behavior
		return	49;

	f = 1.0;
	if ((long)f != 1)
		return	51;
	f = 2.0;
	if ((long)f != 2)
		return	52;
	f = -1.0;
	if ((long)f != -1)
		return	53;
	f = 1024.0;
	if ((long)f != 1024)
		return	60;
	f = -1024.0;
	if ((long)f != -1024)
		return	61;
	f = 32767.0;
	if ((long)f != 32767)
		return	70;
	f = 32768.0;
	if ((long)f != 32768L)
		return	71;
	f = -32767.0;
	if ((long)f != -32767)
		return	70;
	f = -32768.0;
	if ((long)f != -32768L)
		return	71;
	f = 65535.0;
	if ((long)f != 65535L)
		return	80;
	f = 65536.0;
	if ((long)f != 65536L)
		return	81;
	f = 65537.0;
	if ((long)f != 65537L)
		return	82;
	f = -65535.0;
	if ((long)f != -65535L)
		return	83;
	f = -65536.0;
	if ((long)f != -65536L)
		return	84;
	f = -65537.0;
	if ((long)f != -65537L)
		return	85;
	f = 16777215.0;
	if ((long)f != 16777215L)
		return	90;
	f = 16777216.0;
	if ((long)f != 16777216L)
		return	91;
	f = -16777215.0;
	if ((long)f != -16777215L)
		return	92;
	f = -16777216.0;
	if ((long)f != -16777216L)
		return	93;
	f = 2147483520.0;
	if ((long)f != 2147483520)
		return	94;
	f = 2147483648.0;
	if ((long)f != 2147483647)   // Undefined behavior
		return	95;
	f = -2147483520.0;
	if ((long)f != -2147483520)
		return	97;
	f = -2147483648.0;
	if ((long)f != -2147483648)
		return	98;

	f = 0;
	if (f != 0.0)
		return	100;
	f = 1;
	if (f != 1.0)
		return	101;
	f = 2;
	if (f != 2.0)
		return	102;
	f = 1024;
	if (f != 1024.0)
		return	110;
	f = -1024;
	if (f != -1024.0)
		return	111;
	f = 32767;
	if (f != 32767.0)
		return	120;
	f = -32767;
	if (f != -32767.0)
		return	121;
	f = -32768.0;
	if (f != -32768.0)
		return	122;

	f = 1L;
	if ((long)f != 1)
		return	151;
	f = 2L;
	if ((long)f != 2)
		return	152;
	f = -1L;
	if ((long)f != -1)
		return	153;
	f = 1024L;
	if ((long)f != 1024)
		return	160;
	f = -1024L;
	if ((long)f != -1024)
		return	161;
	f = 32767L;
	if ((long)f != 32767)
		return	170;
	f = 32768L;
	if ((long)f != 32768L)
		return	171;
	f = -32767L;
	if ((long)f != -32767)
		return	172;
	f = -32768L;
	if ((long)f != -32768L)
		return	173;
	f = 65535L;
	if ((long)f != 65535L)
		return	180;
	f = 65536L;
	if ((long)f != 65536L)
		return	181;
	f = 65537L;
	if ((long)f != 65537L)
		return	182;
	f = -65535L;
	if ((long)f != -65535L)
		return	183;
	f = -65536L;
	if ((long)f != -65536L)
		return	184;
	f = -65537L;
	if ((long)f != -65537L)
		return	185;
	f = 16777215L;
	if ((long)f != 16777215L)
		return	190;
	f = 16777216L;
	if ((long)f != 16777216L)
		return	191;
	f = -16777215L;
	if ((long)f != -16777215L)
		return	192;
	f = -16777216L;
	if ((long)f != -16777216L)
		return	193;
	f = 2147483520L;
	if ((long)f != 2147483520)
		return	194;
	f = 2147483648L;
	if ((long)f != 2147483647L)     // undefined behavior
		return	195;
	f = -2147483520L;
	if ((long)f != -2147483520)
		return	197;
	f = -2147483648L;
	if ((long)f != -2147483648)
		return	198;

	return 0;
}
