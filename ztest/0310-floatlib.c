//
//	float library
//

#include "common.h"

extern	float	fabsf(float x);
extern	float	floorf(float x);
extern	float	ceilf(float x);
extern	float	roundf(float x);
extern	int	isnan(float x);

#define to_float(x) (*(const float*)&(uint32_t){x})


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
	if (fabsf(1.0f) != 1.0f)
		return 1;
	if (fabsf(-1.0f) != 1.0f)
		return 2;
	if (fabsf(0.0) == 1.0f)
		return 3;

	if (floorf(0.0f) != 0.0f)
		return 11;

	if (floorf(0.5f) != 0.0f)
		return 12;

	if (floorf(1.0f) != 1.0f)
		return 13;

	if (floorf(1.5f) != 1.0f)
		return 14;

	if (floorf(2.5f) != 2.0f)
		return 15;

	if (floorf(8388607.5f) != 8388607.0f)
		return 16;

	if (floorf(8388608.0f) != 8388608.0f)
		return 17;


	if (floorf(-0.0f) != 0.0f)
		return 21;

	if (floorf(-0.5f) != -1.0f)
		return 22;

	if (floorf(-1.0f) != -1.0f)
		return 23;

	if (floorf(-1.5f) != -2.0f)
		return 24;

	if (floorf(-2.5f) != -3.0f)
		return 25;

	if (floorf(-8388607.5f) != -8388608.0f)
		return 26;

	if (floorf(-8388608.0f) != -8388608.0f)
		return 27;

	if (floorf(256.0f) != 256.0f)
		return 31;

	if (floorf(256.5f) != 256.0f)
		return 32;

	if (floorf(511.5f) != 511.0f)
		return 33;

	if (floorf(-256.5f) != -257.0f)
		return 34;

	if (floorf(65536.0f) != 65536.0f)
		return 35;

	if (floorf(65536.5f) != 65536.0f)
		return 36;

	if (floorf(-65536.5f) != -65537.0f)
		return 37;

	if (ceilf(0.0f) != 0.0f)
		return 51;

	if (ceilf(0.5f) != 1.0f)
		return 52;

	if (ceilf(1.0f) != 1.0f)
		return 53;

	if (ceilf(1.5f) != 2.0f)
		return 54;

	if (ceilf(2.5f) != 3.0f)
		return 55;

	if (ceilf(8388607.5f) != 8388608.0f)
		return 56;

	if (ceilf(8388608.0f) != 8388608.0f)
		return 57;

	if (ceilf(-0.0f) != -0.0f)
		return 61;

	if (ceilf(-0.5f) != -0.0f)
		return 62;

	if (ceilf(-1.0f) != -1.0f)
		return 63;

	if (ceilf(-1.5f) != -1.0f)
		return 64;

	if (ceilf(-2.5f) != -2.0f)
		return 65;

	if (ceilf(-8388607.5f) != -8388607.0f)
		return 66;

	if (ceilf(-8388608.0f) != -8388608.0f)
		return 67;

	if (ceilf(256.0f) != 256.0f)
		return 41;

	if (ceilf(256.5f) != 257.0f)
		return 42;

	if (ceilf(511.5f) != 512.0f)
		return 43;

	if (ceilf(-256.5f) != -256.0f)
		return 44;

	if (ceilf(65536.0f) != 65536.0f)
		return 45;

	if (ceilf(65536.5f) != 65537.0f)
		return 46;

	if (ceilf(-65536.5f) != -65536.0f)
		return 47;


	if (roundf(0.0f) != 0.0f)
		return 101;

	if (roundf(0.4f) != 0.0f)
		return 102;

	if (roundf(0.5f) != 1.0f)
		return 103;

	if (roundf(1.0f) != 1.0f)
		return 104;

	if (roundf(1.4f) != 1.0f)
		return 105;

	if (roundf(1.5f) != 2.0f)
		return 106;

	if (roundf(1.6f) != 2.0f)
		return 107;

	if (roundf(2.4f) != 2.0f)
		return 108;

	if (roundf(2.5f) != 3.0f)
		return 109;

	if (roundf(8388607.5f) != 8388608.0f)
		return 112;

	if (roundf(8388608.0f) != 8388608.0f)
		return 113;


	if (roundf(-0.0f) != -0.0f)
		return 71;

	if (roundf(-0.4f) != -0.0f)
		return 72;

	if (roundf(-0.5f) != -1.0f)
		return 73;

	if (roundf(-1.0f) != -1.0f)
		return 74;

	if (roundf(-1.4f) != -1.0f)
		return 75;

	if (roundf(-1.5f) != -2.0f)
		return 76;

	if (roundf(-1.6f) != -2.0f)
		return 77;

	if (roundf(-2.4f) != -2.0f)
		return 78;

	if (roundf(-2.5f) != -3.0f)
		return 79;

	if (roundf(-8388607.5f) != -8388608.0f)
		return 82;

	if (roundf(-8388608.0f) != -8388608.0f)
		return 83;

	if (roundf(256.0f) != 256.0f)
		return 91;

	if (roundf(256.4f) != 256.0f)
		return 92;

	if (roundf(256.5f) != 257.0f)
		return 93;

	if (roundf(511.4f) != 511.0f)
		return 94;

	if (roundf(-256.5f) != -257.0f)
		return 95;

	if (roundf(65536.0f) != 65536.0f)
		return 96;

	if (roundf(65536.5f) != 65537.0f)
		return 97;

	if (roundf(-65536.5f) != -65537.0f)
		return 98;

	if (roundf(0.49999997f) != 0.0f)
		return 121;

	if (roundf(-0.49999997f) != 0.0f)
		return 122;

	if (roundf(8388609.0f) != 8388609.0f)
		return 123;

	if (roundf(16777213.0f) != 16777213.0f)
		return 124;

	if (roundf(16777215.0f) != 16777215.0f)
		return 125;

	if (roundf(-16777215.0f) != -16777215.0f)
		return 126;

	if (roundf(8388607.5f) != 8388608.0f)
		return 127;


	if (cmpfl(roundf(to_float(0xFF800000)), 0xFF800000UL))
		return 201;

	if (cmpfl(roundf(to_float(0x7F800000)), 0x7F800000UL))
		return 202;

	if (!isnan(roundf(to_float(0xFFC00000))))
		return 203;

	if (!isnan(roundf(to_float(0x7FC00000))))
		return 204;

	if (!isnan(roundf(to_float(0x7F800001))))
		return 205;

	if (!isnan(roundf(to_float(0xFF800001))))
		return 206;

	if (cmpfl(roundf(to_float(0x00000000)), 0x00000000UL))
		return 207;

	if (cmpfl(roundf(to_float(0x80000000)), 0x80000000UL))
		return 208;

	if (cmpfl(roundf(to_float(0x00000001)), 0x00000000UL))
		return 209;

	if (cmpfl(roundf(to_float(0x80000001)), 0x80000000UL))
		return 210;

	if (cmpfl(roundf(to_float(0x007FFFFF)), 0x00000000UL))
		return 211;

	if (cmpfl(roundf(to_float(0x807FFFFF)), 0x80000000UL))
		return 212;

	if (cmpfl(roundf(to_float(0x3EFFFFFF)), 0x00000000UL))
		return 213;

	if (cmpfl(roundf(to_float(0xBEFFFFFF)), 0x80000000UL))
		return 214;

	if (cmpfl(roundf(to_float(0x3F000001)), 0x3F800000UL))
		return 215;

	if (cmpfl(roundf(to_float(0xBF000001)), 0xBF800000UL))
		return 216;

	if (cmpfl(roundf(to_float(0x40200000)), 0x40400000UL))
		return 217;

	if (cmpfl(roundf(to_float(0xC0200000)), 0xC0400000UL))
		return 218;

	if (cmpfl(roundf(to_float(0x00800000)), 0x00000000UL))
		return 219;

	if (cmpfl(roundf(to_float(0x80800000)), 0x80000000UL))
		return 220;

	if (cmpfl(roundf(to_float(0x3F000000)), 0x3F800000UL))
		return 221;

	if (cmpfl(roundf(to_float(0xBF000000)), 0xBF800000UL))
		return 222;

	if (cmpfl(roundf(to_float(0x3F7FFFFF)), 0x3F800000UL))
		return 223;

	if (cmpfl(roundf(to_float(0xBF7FFFFF)), 0xBF800000UL))
		return 224;

	if (cmpfl(roundf(to_float(0x3F800000)), 0x3F800000UL))
		return 225;

	if (cmpfl(roundf(to_float(0xBF800000)), 0xBF800000UL))
		return 226;

	if (cmpfl(roundf(to_float(0x3F800001)), 0x3F800000UL))
		return 227;

	if (cmpfl(roundf(to_float(0xBF800001)), 0xBF800000UL))
		return 228;

	if (cmpfl(roundf(to_float(0x3FC00000)), 0x40000000UL))
		return 229;

	if (cmpfl(roundf(to_float(0xBFC00000)), 0xC0000000UL))
		return 230;

	if (cmpfl(roundf(to_float(0x3FFFFFFF)), 0x40000000UL))
		return 231;

	if (cmpfl(roundf(to_float(0xBFFFFFFF)), 0xC0000000UL))
		return 232;

	if (cmpfl(roundf(to_float(0x4AFFFFFF)), 0x4B000000UL))
		return 233;

	if (cmpfl(roundf(to_float(0xCAFFFFFF)), 0xCB000000UL))
		return 234;

	if (cmpfl(roundf(to_float(0x4B000000)), 0x4B000000UL))
		return 235;

	if (cmpfl(roundf(to_float(0xCB000000)), 0xCB000000UL))
		return 236;

	if (cmpfl(roundf(to_float(0x4B000001)), 0x4B000001UL))
		return 237;

	if (cmpfl(roundf(to_float(0xCB000001)), 0xCB000001UL))
		return 238;

	if (cmpfl(roundf(to_float(0x7F7FFFFF)), 0x7F7FFFFFUL))
		return 239;

	if (cmpfl(roundf(to_float(0xFF7FFFFF)), 0xFF7FFFFFUL))
		return 240;


	if (cmpfl(roundf(to_float(0x3F800000)), 0x3F800000UL))
		return 128;

	if (cmpfl(roundf(to_float(0xBF800000)), 0xBF800000UL))
		return 129;

	if (cmpfl(roundf(to_float(0x3F800001)), 0x3F800000UL))
		return 130;

	if (cmpfl(roundf(to_float(0xBF800001)), 0xBF800000UL))
		return 131;

	if (cmpfl(roundf(to_float(0x3FC00000)), 0x40000000UL))
		return 132;

	if (cmpfl(roundf(to_float(0xBFC00000)), 0xC0000000UL))
		return 133;

	if (cmpfl(roundf(to_float(0x3FFFFFFF)), 0x40000000UL))
		return 134;

	if (cmpfl(roundf(to_float(0xBFFFFFFF)), 0xC0000000UL))
		return 135;

	if (cmpfl(roundf(to_float(0x43000000)), 0x43000000UL))
		return 136;

	if (cmpfl(roundf(to_float(0xC3000000)), 0xC3000000UL))
		return 137;

	if (cmpfl(roundf(to_float(0x43000001)), 0x43000000UL))
		return 138;

	if (cmpfl(roundf(to_float(0xC3000001)), 0xC3000000UL))
		return 139;

	if (cmpfl(roundf(to_float(0x43400000)), 0x43400000UL))
		return 140;

	if (cmpfl(roundf(to_float(0xC3400000)), 0xC3400000UL))
		return 141;

	if (cmpfl(roundf(to_float(0x437FFFFF)), 0x43800000UL))
		return 142;

	if (cmpfl(roundf(to_float(0xC37FFFFF)), 0xC3800000UL))
		return 143;

	if (cmpfl(roundf(to_float(0x43800000)), 0x43800000UL))
		return 144;

	if (cmpfl(roundf(to_float(0xC3800000)), 0xC3800000UL))
		return 145;

	if (cmpfl(roundf(to_float(0x43800001)), 0x43800000UL))
		return 146;

	if (cmpfl(roundf(to_float(0xC3800001)), 0xC3800000UL))
		return 147;

	if (cmpfl(roundf(to_float(0x43C00000)), 0x43C00000UL))
		return 148;

	if (cmpfl(roundf(to_float(0xC3C00000)), 0xC3C00000UL))
		return 149;

	if (cmpfl(roundf(to_float(0x43FFFFFF)), 0x44000000UL))
		return 150;

	if (cmpfl(roundf(to_float(0xC3FFFFFF)), 0xC4000000UL))
		return 151;

	if (cmpfl(roundf(to_float(0x47000000)), 0x47000000UL))
		return 152;

	if (cmpfl(roundf(to_float(0xC7000000)), 0xC7000000UL))
		return 153;

	if (cmpfl(roundf(to_float(0x47000001)), 0x47000000UL))
		return 154;

	if (cmpfl(roundf(to_float(0xC7000001)), 0xC7000000UL))
		return 155;

	if (cmpfl(roundf(to_float(0x47400000)), 0x47400000UL))
		return 156;

	if (cmpfl(roundf(to_float(0xC7400000)), 0xC7400000UL))
		return 157;

	if (cmpfl(roundf(to_float(0x477FFFFF)), 0x47800000UL))
		return 158;

	if (cmpfl(roundf(to_float(0xC77FFFFF)), 0xC7800000UL))
		return 159;

	if (cmpfl(roundf(to_float(0x47800000)), 0x47800000UL))
		return 160;

	if (cmpfl(roundf(to_float(0xC7800000)), 0xC7800000UL))
		return 161;

	if (cmpfl(roundf(to_float(0x47800001)), 0x47800000UL))
		return 162;

	if (cmpfl(roundf(to_float(0xC7800001)), 0xC7800000UL))
		return 163;

	if (cmpfl(roundf(to_float(0x47C00000)), 0x47C00000UL))
		return 164;

	if (cmpfl(roundf(to_float(0xC7C00000)), 0xC7C00000UL))
		return 165;

	if (cmpfl(roundf(to_float(0x47FFFFFF)), 0x48000000UL))
		return 166;

	if (cmpfl(roundf(to_float(0xC7FFFFFF)), 0xC8000000UL))
		return 167;

	if (cmpfl(roundf(to_float(0x4A800000)), 0x4A800000UL))
		return 168;

	if (cmpfl(roundf(to_float(0xCA800000)), 0xCA800000UL))
		return 169;

	if (cmpfl(roundf(to_float(0x4A800001)), 0x4A800002UL))
		return 170;

	if (cmpfl(roundf(to_float(0xCA800001)), 0xCA800002UL))
		return 171;

	if (cmpfl(roundf(to_float(0x4AC00000)), 0x4AC00000UL))
		return 172;

	if (cmpfl(roundf(to_float(0xCAC00000)), 0xCAC00000UL))
		return 173;

	if (cmpfl(roundf(to_float(0x4AFFFFFF)), 0x4B000000UL))
		return 174;

	if (cmpfl(roundf(to_float(0xCAFFFFFF)), 0xCB000000UL))
		return 175;

	return 0;
}

