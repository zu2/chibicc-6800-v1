//
//	float div: zero, Inf, NaN and subnormal operands
//

#include "common.h"

int
is_nan(float f)
{
	return	f != f;
}

float to_float(unsigned long x)
{
	return	*((float *)&x);
}

int
cmpfl(float f, unsigned long g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
	return 0;
}


int main(int argc, char **argv)
{
	float	a,b;

	/* +0 */
	a = to_float(0x00000000); b = to_float(0x00000000);
	if (!is_nan(a / b))
		return 21;
	a = to_float(0x00000000); b = to_float(0x80000000);
	if (!is_nan(a / b))
		return 22;
	a = to_float(0x00000000); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x00000000))
		return 23;
	a = to_float(0x00000000); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x80000000))
		return 24;
	a = to_float(0x00000000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 25;
	a = to_float(0x00000000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x00000000))
		return 26;
	a = to_float(0x00000000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x80000000))
		return 27;
	a = to_float(0x00000000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x00000000))
		return 28;
	a = to_float(0x00000000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x80000000))
		return 29;
	a = to_float(0x00000000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x00000000))
		return 30;
	a = to_float(0x00000000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x80000000))
		return 31;

	/* -0 */
	a = to_float(0x80000000); b = to_float(0x00000000);
	if (!is_nan(a / b))
		return 41;
	a = to_float(0x80000000); b = to_float(0x80000000);
	if (!is_nan(a / b))
		return 42;
	a = to_float(0x80000000); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x80000000))
		return 43;
	a = to_float(0x80000000); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x00000000))
		return 44;
	a = to_float(0x80000000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 45;
	a = to_float(0x80000000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x80000000))
		return 46;
	a = to_float(0x80000000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x00000000))
		return 47;
	a = to_float(0x80000000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x80000000))
		return 48;
	a = to_float(0x80000000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x00000000))
		return 49;
	a = to_float(0x80000000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x80000000))
		return 50;
	a = to_float(0x80000000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x00000000))
		return 51;

	/* +I */
	a = to_float(0x7f800000); b = to_float(0x00000000);
	if (cmpfl(a / b, 0x7f800000))
		return 61;
	a = to_float(0x7f800000); b = to_float(0x80000000);
	if (cmpfl(a / b, 0xff800000))
		return 62;
	a = to_float(0x7f800000); b = to_float(0x7f800000);
	if (!is_nan(a / b))
		return 63;
	a = to_float(0x7f800000); b = to_float(0xff800000);
	if (!is_nan(a / b))
		return 64;
	a = to_float(0x7f800000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 65;
	a = to_float(0x7f800000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x7f800000))
		return 66;
	a = to_float(0x7f800000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0xff800000))
		return 67;
	a = to_float(0x7f800000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x7f800000))
		return 68;
	a = to_float(0x7f800000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0xff800000))
		return 69;
	a = to_float(0x7f800000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x7f800000))
		return 70;
	a = to_float(0x7f800000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0xff800000))
		return 71;

	/* -I */
	a = to_float(0xff800000); b = to_float(0x00000000);
	if (cmpfl(a / b, 0xff800000))
		return 81;
	a = to_float(0xff800000); b = to_float(0x80000000);
	if (cmpfl(a / b, 0x7f800000))
		return 82;
	a = to_float(0xff800000); b = to_float(0x7f800000);
	if (!is_nan(a / b))
		return 83;
	a = to_float(0xff800000); b = to_float(0xff800000);
	if (!is_nan(a / b))
		return 84;
	a = to_float(0xff800000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 85;
	a = to_float(0xff800000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0xff800000))
		return 86;
	a = to_float(0xff800000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x7f800000))
		return 87;
	a = to_float(0xff800000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0xff800000))
		return 88;
	a = to_float(0xff800000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x7f800000))
		return 89;
	a = to_float(0xff800000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0xff800000))
		return 90;
	a = to_float(0xff800000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x7f800000))
		return 91;

	/* N */
	a = to_float(0x7fc00000); b = to_float(0x00000000);
	if (!is_nan(a / b))
		return 101;
	a = to_float(0x7fc00000); b = to_float(0x80000000);
	if (!is_nan(a / b))
		return 102;
	a = to_float(0x7fc00000); b = to_float(0x7f800000);
	if (!is_nan(a / b))
		return 103;
	a = to_float(0x7fc00000); b = to_float(0xff800000);
	if (!is_nan(a / b))
		return 104;
	a = to_float(0x7fc00000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 105;
	a = to_float(0x7fc00000); b = to_float(0x3f800000);
	if (!is_nan(a / b))
		return 106;
	a = to_float(0x7fc00000); b = to_float(0xbf800000);
	if (!is_nan(a / b))
		return 107;
	a = to_float(0x7fc00000); b = to_float(0x00000001);
	if (!is_nan(a / b))
		return 108;
	a = to_float(0x7fc00000); b = to_float(0x80000001);
	if (!is_nan(a / b))
		return 109;
	a = to_float(0x7fc00000); b = to_float(0x007fffff);
	if (!is_nan(a / b))
		return 110;
	a = to_float(0x7fc00000); b = to_float(0x807fffff);
	if (!is_nan(a / b))
		return 111;

	/* +n */
	a = to_float(0x3f800000); b = to_float(0x00000000);
	if (cmpfl(a / b, 0x7f800000))
		return 121;
	a = to_float(0x3f800000); b = to_float(0x80000000);
	if (cmpfl(a / b, 0xff800000))
		return 122;
	a = to_float(0x3f800000); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x00000000))
		return 123;
	a = to_float(0x3f800000); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x80000000))
		return 124;
	a = to_float(0x3f800000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 125;
	a = to_float(0x3f800000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x3f800000))
		return 126;
	a = to_float(0x3f800000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0xbf800000))
		return 127;
	a = to_float(0x3f800000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x7f800000))
		return 128;
	a = to_float(0x3f800000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0xff800000))
		return 129;
	a = to_float(0x3f800000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x7e800001))
		return 130;
	a = to_float(0x3f800000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0xfe800001))
		return 131;

	/* -n */
	a = to_float(0xbf800000); b = to_float(0x00000000);
	if (cmpfl(a / b, 0xff800000))
		return 141;
	a = to_float(0xbf800000); b = to_float(0x80000000);
	if (cmpfl(a / b, 0x7f800000))
		return 142;
	a = to_float(0xbf800000); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x80000000))
		return 143;
	a = to_float(0xbf800000); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x00000000))
		return 144;
	a = to_float(0xbf800000); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 145;
	a = to_float(0xbf800000); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0xbf800000))
		return 146;
	a = to_float(0xbf800000); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x3f800000))
		return 147;
	a = to_float(0xbf800000); b = to_float(0x00000001);
	if (cmpfl(a / b, 0xff800000))
		return 148;
	a = to_float(0xbf800000); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x7f800000))
		return 149;
	a = to_float(0xbf800000); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0xfe800001))
		return 150;
	a = to_float(0xbf800000); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x7e800001))
		return 151;

	/* +s */
	a = to_float(0x00000001); b = to_float(0x00000000);
	if (cmpfl(a / b, 0x7f800000))
		return 161;
	a = to_float(0x00000001); b = to_float(0x80000000);
	if (cmpfl(a / b, 0xff800000))
		return 162;
	a = to_float(0x00000001); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x00000000))
		return 163;
	a = to_float(0x00000001); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x80000000))
		return 164;
	a = to_float(0x00000001); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 165;
	a = to_float(0x00000001); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x00000001))
		return 166;
	a = to_float(0x00000001); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x80000001))
		return 167;
	a = to_float(0x00000001); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x3f800000))
		return 168;
	a = to_float(0x00000001); b = to_float(0x80000001);
	if (cmpfl(a / b, 0xbf800000))
		return 169;
	a = to_float(0x00000001); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x34000001))
		return 170;
	a = to_float(0x00000001); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0xb4000001))
		return 171;

	/* -s */
	a = to_float(0x80000001); b = to_float(0x00000000);
	if (cmpfl(a / b, 0xff800000))
		return 181;
	a = to_float(0x80000001); b = to_float(0x80000000);
	if (cmpfl(a / b, 0x7f800000))
		return 182;
	a = to_float(0x80000001); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x80000000))
		return 183;
	a = to_float(0x80000001); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x00000000))
		return 184;
	a = to_float(0x80000001); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 185;
	a = to_float(0x80000001); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x80000001))
		return 186;
	a = to_float(0x80000001); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x00000001))
		return 187;
	a = to_float(0x80000001); b = to_float(0x00000001);
	if (cmpfl(a / b, 0xbf800000))
		return 188;
	a = to_float(0x80000001); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x3f800000))
		return 189;
	a = to_float(0x80000001); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0xb4000001))
		return 190;
	a = to_float(0x80000001); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x34000001))
		return 191;

	/* +S */
	a = to_float(0x007fffff); b = to_float(0x00000000);
	if (cmpfl(a / b, 0x7f800000))
		return 201;
	a = to_float(0x007fffff); b = to_float(0x80000000);
	if (cmpfl(a / b, 0xff800000))
		return 202;
	a = to_float(0x007fffff); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x00000000))
		return 203;
	a = to_float(0x007fffff); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x80000000))
		return 204;
	a = to_float(0x007fffff); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 205;
	a = to_float(0x007fffff); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x007fffff))
		return 206;
	a = to_float(0x007fffff); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x807fffff))
		return 207;
	a = to_float(0x007fffff); b = to_float(0x00000001);
	if (cmpfl(a / b, 0x4afffffe))
		return 208;
	a = to_float(0x007fffff); b = to_float(0x80000001);
	if (cmpfl(a / b, 0xcafffffe))
		return 209;
	a = to_float(0x007fffff); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0x3f800000))
		return 210;
	a = to_float(0x007fffff); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0xbf800000))
		return 211;

	/* -S */
	a = to_float(0x807fffff); b = to_float(0x00000000);
	if (cmpfl(a / b, 0xff800000))
		return 221;
	a = to_float(0x807fffff); b = to_float(0x80000000);
	if (cmpfl(a / b, 0x7f800000))
		return 222;
	a = to_float(0x807fffff); b = to_float(0x7f800000);
	if (cmpfl(a / b, 0x80000000))
		return 223;
	a = to_float(0x807fffff); b = to_float(0xff800000);
	if (cmpfl(a / b, 0x00000000))
		return 224;
	a = to_float(0x807fffff); b = to_float(0x7fc00000);
	if (!is_nan(a / b))
		return 225;
	a = to_float(0x807fffff); b = to_float(0x3f800000);
	if (cmpfl(a / b, 0x807fffff))
		return 226;
	a = to_float(0x807fffff); b = to_float(0xbf800000);
	if (cmpfl(a / b, 0x007fffff))
		return 227;
	a = to_float(0x807fffff); b = to_float(0x00000001);
	if (cmpfl(a / b, 0xcafffffe))
		return 228;
	a = to_float(0x807fffff); b = to_float(0x80000001);
	if (cmpfl(a / b, 0x4afffffe))
		return 229;
	a = to_float(0x807fffff); b = to_float(0x007fffff);
	if (cmpfl(a / b, 0xbf800000))
		return 230;
	a = to_float(0x807fffff); b = to_float(0x807fffff);
	if (cmpfl(a / b, 0x3f800000))
		return 231;

	return 0;
}
