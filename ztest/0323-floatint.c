//
//	float convert to/from int
//

#include "common.h"


int main(int argc, char **argv)
{
	float	f;

	f = 1.0;
	if ((unsigned int)f != 1)
		return	1;
	f = 2.0;
	if ((unsigned int)f != 2)
		return	2;
	f = 1024.0;
	if ((unsigned int)f != 1024)
		return	10;
	f = 32767.0;
	if ((unsigned int)f != 32767)
		return	20;
	f = 32768.0;
	if ((unsigned int)f != 32768U)
		return	21;
	f = 65535.0;
	if ((unsigned int)f != 65535U)
		return	30;

	f = 1.0;
	if ((int)f != 1)
		return	51;
	f = 2.0;
	if ((int)f != 2)
		return	52;
	f = -1.0;
	if ((int)f != -1)
		return	53;
	f = 1024.0;
	if ((int)f != 1024)
		return	60;
	f = -1024.0;
	if ((int)f != -1024)
		return	61;
	f = 32767.0;
	if ((int)f != 32767)
		return	70;

	f = -32767.0;
	if ((int)f != -32767)
		return	70;
	f = -32768.0;
	if ((int)f != -32768)
		return	71;

	f = 0;
	if (f != 0.0)
		return	100;
	f = 1;
	if (f != 1.0)
		return	101;
	f = 2;
	if (f != 2.0)
		return	102;
	f = 1024U;
	if (f != 1024.0)
		return	110;
	f = 32767U;
	if (f != 32767.0)
		return	120;
	f = 32768U;
	if (f != 32768.0)
		return	122;
	f = 65535U;
	if (f != 65535.0)
		return	123;

	f = 1;
	if ((int)f != 1)
		return	151;
	f = 2;
	if ((int)f != 2)
		return	152;
	f = -1;
	if ((int)f != -1)
		return	153;
	f = 1024L;
	if ((int)f != 1024)
		return	160;
	f = -1024;
	if ((int)f != -1024)
		return	161;
	f = 32767;
	if ((int)f != 32767)
		return	170;
	f = -32767;
	if ((int)f != -32767)
		return	172;
	f = -32768;
	if ((int)f != -32768)
		return	173;

	return 0;
}
