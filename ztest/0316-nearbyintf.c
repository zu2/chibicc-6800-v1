//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})
#define to_bits(x)  (*(const uint32_t*)&(float){x})

int
cmpfl(float f, unsigned long g)
{
	return to_bits(f) != (uint32_t)g;
}

int main(int argc, char **argv)
{
  if (!isnan(nearbyintf(NAN)))
    return 1;

  if (nearbyintf(INFINITY) != INFINITY)
    return 2;

  if (nearbyintf(0.0f) != 0.0f)
    return 3;

  if (nearbyintf(FLT_TRUE_MIN) != 0.0f)
    return 4;




	if (nearbyintf(0.0) != 0.0)
		return 11;

	if (nearbyintf(1.0) != 1.0)
		return 12;

	if (nearbyintf(2.0) != 2.0)
		return 13;

	if (nearbyintf(3.0) != 3.0)
		return 14;

	if (nearbyintf(4.0) != 4.0)
		return 15;

	if (nearbyintf(8.0) != 8.0)
		return 16;


	if (nearbyintf(2.3) != 2.0)
		return 21;

	if (nearbyintf(2.4) != 2.0)
		return 22;

	if (nearbyintf(2.5) != 2.0)
		return 23;

	if (nearbyintf(2.6) != 3.0)
		return 24;

	if (nearbyintf(2.7) != 3.0)
		return 25;

  if (nearbyintf(to_float(0x401FFFFF))!=2.0)  // 2.5 - LSB=1
    return 26;

  if (nearbyintf(to_float(0x40200001))!=3.0)  // 2.5 + LSB=1
    return 27;


	if (nearbyintf(3.3) != 3.0)
		return 31;

	if (nearbyintf(3.4) != 3.0)
		return 32;

	if (nearbyintf(3.5) != 4.0)
		return 33;

	if (nearbyintf(3.6) != 4.0)
		return 34;

	if (nearbyintf(3.7) != 4.0)
		return 35;

  if (nearbyintf(to_float(0x405FFFFF))!=3.0)  // 3.5 - LSB=1
    return 36;

  if (nearbyintf(to_float(0x40600001))!=4.0)  // 3.5 + LSB=1
    return 37;


  if (nearbyintf(to_float(0x3eFFFFFF)) != 0.0f) // 0.49999..
    return 41;

  if (nearbyintf(0.5) != 0.0f)
    return 42;

  if (nearbyintf(to_float(0x3f000001)) != 1.0f)  // 0.500000...1
    return 43;


  if (nearbyintf(8388606.5f) != 8388606.0f)
    return 71;

  if (nearbyintf(8388607.0f) != 8388607.0f)
    return 72;

  if (nearbyintf(8388607.5f) != 8388608.0f)
    return 73;

  if (nearbyintf(8388608.0f) != 8388608.0f)
    return 74;



  if (nearbyintf(2147483646.0f) != 2147483646.0f)
    return 91;

  if (nearbyintf(2147483648.0f) != 2147483648.0f)
    return 92;

  if (nearbyintf(2147483647.0f) != 2147483647.0f)
    return 93;



	if (nearbyintf(-0.0) != -0.0)
		return 111;

	if (nearbyintf(-1.0) != -1.0)
		return 112;

	if (nearbyintf(-2.0) != -2.0)
		return 113;

	if (nearbyintf(-3.0) != -3.0)
		return 114;

	if (nearbyintf(-4.0) != -4.0)
		return 115;

	if (nearbyintf(-8.0) != -8.0)
		return 116;


	if (nearbyintf(-2.3) != -2.0)
		return 121;

	if (nearbyintf(-2.4) != -2.0)
		return 122;

	if (nearbyintf(-2.5) != -2.0)
		return 123;

	if (nearbyintf(-2.6) != -3.0)
		return 124;

	if (nearbyintf(-2.7) != -3.0)
		return 125;

  if (nearbyintf(to_float(0xc01FFFFF))!=-2.0)  // 2.5 - LSB=1
    return 126;

  if (nearbyintf(to_float(0xc0200001))!=-3.0)  // 2.5 + LSB=1
    return 127;


	if (nearbyintf(-3.3) != -3.0)
		return 131;

	if (nearbyintf(-3.4) != -3.0)
		return 132;

	if (nearbyintf(-3.5) != -4.0)
		return 133;

	if (nearbyintf(-3.6) != -4.0)
		return 134;

	if (nearbyintf(-3.7) != -4.0)
		return 135;

  if (nearbyintf(to_float(0xC05FFFFF))!=-3.0)  // 3.5 - LSB=1
    return 136;

  if (nearbyintf(to_float(0xC0600001))!=-4.0)  // 3.5 + LSB=1
    return 137;


  if (nearbyintf(to_float(0xBeFFFFFF)) != -0.0f) // 0.49999..
    return 141;

  if (nearbyintf(-0.5) != -0.0f)
    return 142;

  if (nearbyintf(to_float(0xBf000001)) != -1.0f)  // 0.500000...1
    return 143;


  if (nearbyintf(-8388606.5f) != -8388606.0f)
    return 171;

  if (nearbyintf(-8388607.0f) != -8388607.0f)
    return 172;

  if (nearbyintf(-8388607.5f) != -8388608.0f)
    return 173;

  if (nearbyintf(-8388608.0f) != -8388608.0f)
    return 174;


  if (nearbyintf(-2147483646.0f) != -2147483646.0f)
    return 191;

  if (nearbyintf(-2147483648.0f) != -2147483648.0f)
    return 192;

  if (nearbyintf(-2147483647.0f) != -2147483647.0f)
    return 193;


  if (cmpfl(nearbyintf(to_float(0xFF800000)), 0xFF800000UL))
    return 144;

  if (cmpfl(nearbyintf(to_float(0x7F800000)), 0x7F800000UL))
    return 145;

  if (!isnan(nearbyintf(to_float(0xFFC00000))))
    return 146;

  if (!isnan(nearbyintf(to_float(0x7FC00000))))
    return 147;

  if (!isnan(nearbyintf(to_float(0x7F800001))))
    return 148;

  if (!isnan(nearbyintf(to_float(0xFF800001))))
    return 149;

  if (cmpfl(nearbyintf(to_float(0x00000000)), 0x00000000UL))
    return 150;

  if (cmpfl(nearbyintf(to_float(0x80000000)), 0x80000000UL))
    return 151;

  if (cmpfl(nearbyintf(to_float(0x00000001)), 0x00000000UL))
    return 152;

  if (cmpfl(nearbyintf(to_float(0x80000001)), 0x80000000UL))
    return 153;

  if (cmpfl(nearbyintf(to_float(0x007FFFFF)), 0x00000000UL))
    return 154;

  if (cmpfl(nearbyintf(to_float(0x807FFFFF)), 0x80000000UL))
    return 155;

  if (cmpfl(nearbyintf(to_float(0x3EFFFFFF)), 0x00000000UL))
    return 156;

  if (cmpfl(nearbyintf(to_float(0xBEFFFFFF)), 0x80000000UL))
    return 157;

  if (cmpfl(nearbyintf(to_float(0x3F000001)), 0x3F800000UL))
    return 158;

  if (cmpfl(nearbyintf(to_float(0xBF000001)), 0xBF800000UL))
    return 159;

  if (cmpfl(nearbyintf(to_float(0x40200000)), 0x40000000UL))
    return 160;

  if (cmpfl(nearbyintf(to_float(0xC0200000)), 0xC0000000UL))
    return 161;

  if (cmpfl(nearbyintf(to_float(0x40A00000)), 0x40A00000UL))
    return 162;

  if (cmpfl(nearbyintf(to_float(0xC0A00000)), 0xC0A00000UL))
    return 163;

  if (cmpfl(nearbyintf(to_float(0x00800000)), 0x00000000UL))
    return 164;

  if (cmpfl(nearbyintf(to_float(0x80800000)), 0x80000000UL))
    return 165;

  if (cmpfl(nearbyintf(to_float(0x3F000000)), 0x00000000UL))
    return 166;

  if (cmpfl(nearbyintf(to_float(0xBF000000)), 0x80000000UL))
    return 167;

  if (cmpfl(nearbyintf(to_float(0x3F7FFFFF)), 0x3F800000UL))
    return 168;

  if (cmpfl(nearbyintf(to_float(0xBF7FFFFF)), 0xBF800000UL))
    return 169;

  if (cmpfl(nearbyintf(to_float(0x3F800000)), 0x3F800000UL))
    return 170;

  if (cmpfl(nearbyintf(to_float(0xBF800000)), 0xBF800000UL))
    return 175;

  if (cmpfl(nearbyintf(to_float(0x3F800001)), 0x3F800000UL))
    return 176;

  if (cmpfl(nearbyintf(to_float(0xBF800001)), 0xBF800000UL))
    return 177;

  if (cmpfl(nearbyintf(to_float(0x3FC00000)), 0x40000000UL))
    return 178;

  if (cmpfl(nearbyintf(to_float(0xBFC00000)), 0xC0000000UL))
    return 179;

  if (cmpfl(nearbyintf(to_float(0x3FFFFFFF)), 0x40000000UL))
    return 180;

  if (cmpfl(nearbyintf(to_float(0xBFFFFFFF)), 0xC0000000UL))
    return 181;

  if (cmpfl(nearbyintf(to_float(0x4AFFFFFF)), 0x4B000000UL))
    return 182;

  if (cmpfl(nearbyintf(to_float(0xCAFFFFFF)), 0xCB000000UL))
    return 183;

  if (cmpfl(nearbyintf(to_float(0x4B000000)), 0x4B000000UL))
    return 184;

  if (cmpfl(nearbyintf(to_float(0xCB000000)), 0xCB000000UL))
    return 185;

  if (cmpfl(nearbyintf(to_float(0x4B000001)), 0x4B000001UL))
    return 186;

  if (cmpfl(nearbyintf(to_float(0xCB000001)), 0xCB000001UL))
    return 187;

  if (cmpfl(nearbyintf(to_float(0x7F7FFFFF)), 0x7F7FFFFFUL))
    return 188;

  if (cmpfl(nearbyintf(to_float(0xFF7FFFFF)), 0xFF7FFFFFUL))
    return 189;

  if (cmpfl(nearbyintf(to_float(0x43000000)), 0x43000000UL))
    return 190;

  if (cmpfl(nearbyintf(to_float(0xC3000000)), 0xC3000000UL))
    return 194;

  if (cmpfl(nearbyintf(to_float(0x43000001)), 0x43000000UL))
    return 195;

  if (cmpfl(nearbyintf(to_float(0xC3000001)), 0xC3000000UL))
    return 196;

  if (cmpfl(nearbyintf(to_float(0x43400000)), 0x43400000UL))
    return 197;

  if (cmpfl(nearbyintf(to_float(0xC3400000)), 0xC3400000UL))
    return 198;

  if (cmpfl(nearbyintf(to_float(0x437FFFFF)), 0x43800000UL))
    return 199;

  if (cmpfl(nearbyintf(to_float(0xC37FFFFF)), 0xC3800000UL))
    return 200;

  if (cmpfl(nearbyintf(to_float(0x43800000)), 0x43800000UL))
    return 201;

  if (cmpfl(nearbyintf(to_float(0xC3800000)), 0xC3800000UL))
    return 202;

  if (cmpfl(nearbyintf(to_float(0x43800001)), 0x43800000UL))
    return 203;

  if (cmpfl(nearbyintf(to_float(0xC3800001)), 0xC3800000UL))
    return 204;

  if (cmpfl(nearbyintf(to_float(0x43C00000)), 0x43C00000UL))
    return 205;

  if (cmpfl(nearbyintf(to_float(0xC3C00000)), 0xC3C00000UL))
    return 206;

  if (cmpfl(nearbyintf(to_float(0x43FFFFFF)), 0x44000000UL))
    return 207;

  if (cmpfl(nearbyintf(to_float(0xC3FFFFFF)), 0xC4000000UL))
    return 208;

  if (cmpfl(nearbyintf(to_float(0x47000000)), 0x47000000UL))
    return 209;

  if (cmpfl(nearbyintf(to_float(0xC7000000)), 0xC7000000UL))
    return 210;

  if (cmpfl(nearbyintf(to_float(0x47000001)), 0x47000000UL))
    return 211;

  if (cmpfl(nearbyintf(to_float(0xC7000001)), 0xC7000000UL))
    return 212;

  if (cmpfl(nearbyintf(to_float(0x47400000)), 0x47400000UL))
    return 213;

  if (cmpfl(nearbyintf(to_float(0xC7400000)), 0xC7400000UL))
    return 214;

  if (cmpfl(nearbyintf(to_float(0x477FFFFF)), 0x47800000UL))
    return 215;

  if (cmpfl(nearbyintf(to_float(0xC77FFFFF)), 0xC7800000UL))
    return 216;

  if (cmpfl(nearbyintf(to_float(0x47800000)), 0x47800000UL))
    return 217;

  if (cmpfl(nearbyintf(to_float(0xC7800000)), 0xC7800000UL))
    return 218;

  if (cmpfl(nearbyintf(to_float(0x47800001)), 0x47800000UL))
    return 219;

  if (cmpfl(nearbyintf(to_float(0xC7800001)), 0xC7800000UL))
    return 220;

  if (cmpfl(nearbyintf(to_float(0x47C00000)), 0x47C00000UL))
    return 221;

  if (cmpfl(nearbyintf(to_float(0xC7C00000)), 0xC7C00000UL))
    return 222;

  if (cmpfl(nearbyintf(to_float(0x47FFFFFF)), 0x48000000UL))
    return 223;

  if (cmpfl(nearbyintf(to_float(0xC7FFFFFF)), 0xC8000000UL))
    return 224;

  if (cmpfl(nearbyintf(to_float(0x4A800000)), 0x4A800000UL))
    return 225;

  if (cmpfl(nearbyintf(to_float(0xCA800000)), 0xCA800000UL))
    return 226;

  if (cmpfl(nearbyintf(to_float(0x4A800001)), 0x4A800000UL))
    return 227;

  if (cmpfl(nearbyintf(to_float(0xCA800001)), 0xCA800000UL))
    return 228;

  if (cmpfl(nearbyintf(to_float(0x4AC00000)), 0x4AC00000UL))
    return 229;

  if (cmpfl(nearbyintf(to_float(0xCAC00000)), 0xCAC00000UL))
    return 230;

	return 0;
}
