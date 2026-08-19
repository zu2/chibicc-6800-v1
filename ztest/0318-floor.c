//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})
#define to_bits(x)  (*(const uint32_t*)&(float){x})

int main(int argc, char **argv)
{
  if (!isnan(floorf(NAN)))
    return 1;

  if (floorf(INFINITY) != INFINITY)
    return 2;

  if (floorf(0.0f) != 0.0f)
    return 3;

  if (floorf(FLT_TRUE_MIN) != 0.0f)
    return 4;


	if (floorf(0.0) != 0.0)
		return 11;

	if (floorf(1.0) != 1.0)
		return 12;

	if (floorf(2.0) != 2.0)
		return 13;

	if (floorf(3.0) != 3.0)
		return 14;

	if (floorf(4.0) != 4.0)
		return 15;

	if (floorf(8.0) != 8.0)
		return 16;


	if (floorf(2.3) != 2.0)
		return 21;

	if (floorf(2.4) != 2.0)
		return 22;

	if (floorf(2.5) != 2.0)
		return 23;

	if (floorf(2.6) != 2.0)
		return 24;

	if (floorf(2.7) != 2.0)
		return 25;

  if (floorf(to_float(0x401FFFFF))!=2.0)  // 2.5 - LSB=1
    return 26;

  if (floorf(to_float(0x40200001))!=2.0)  // 2.5 + LSB=1
    return 27;


	if (floorf(3.3) != 3.0)
		return 31;

	if (floorf(3.4) != 3.0)
		return 32;

	if (floorf(3.5) != 3.0)
		return 33;

	if (floorf(3.6) != 3.0)
		return 34;

	if (floorf(3.7) != 3.0)
		return 35;

  if (floorf(to_float(0x405FFFFF))!=3.0)  // 3.5 - LSB=1
    return 36;

  if (floorf(to_float(0x40600001))!=3.0)  // 3.5 + LSB=1
    return 37;


  if (floorf(to_float(0x3eFFFFFF)) != 0.0f) // 0.49999..
    return 41;

  if (floorf(0.5) != 0.0f)
    return 42;

  if (floorf(to_float(0x3f000001)) != 0.0f)  // 0.500000...1
    return 43;


  if (floorf(8388606.5f) != 8388606.0f)
    return 71;

  if (floorf(8388607.0f) != 8388607.0f)
    return 72;

  if (floorf(8388607.5f) != 8388607.0f)
    return 73;

  if (floorf(8388608.0f) != 8388608.0f)
    return 74;


  if (floorf(2147483646.0f) != 2147483646.0f)
    return 91;

  if (floorf(2147483648.0f) != 2147483648.0f)
    return 92;

  if (floorf(2147483647.0f) != 2147483647.0f)
    return 93;


	if (floorf(-0.0) != -0.0)
		return 111;

	if (floorf(-1.0) != -1.0)
		return 112;

	if (floorf(-2.0) != -2.0)
		return 113;

	if (floorf(-3.0) != -3.0)
		return 114;

	if (floorf(-4.0) != -4.0)
		return 115;

	if (floorf(-8.0) != -8.0)
		return 116;


	if (floorf(-2.3) != -3.0)
		return 121;

	if (floorf(-2.4) != -3.0)
		return 122;

	if (floorf(-2.5) != -3.0)
		return 123;

	if (floorf(-2.6) != -3.0)
		return 124;

	if (floorf(-2.7) != -3.0)
		return 125;

  if (floorf(to_float(0xC01FFFFF))!=-3.0)  // 2.5 - LSB=1
    return 126;

  if (floorf(to_float(0xC0200001))!=-3.0)  // 2.5 + LSB=1
    return 127;


	if (floorf(-3.3) != -4.0)
		return 131;

	if (floorf(-3.4) != -4.0)
		return 132;

	if (floorf(-3.5) != -4.0)
		return 133;

	if (floorf(-3.6) != -4.0)
		return 134;

	if (floorf(-3.7) != -4.0)
		return 135;

  if (floorf(to_float(0xC05FFFFF))!=-4.0)  // 3.5 - LSB=1
    return 136;

  if (floorf(to_float(0xC0600001))!=-4.0)  // 3.5 + LSB=1
    return 137;


  if (floorf(to_float(0xBeFFFFFF)) != -1.0f) // 0.49999..
    return 141;

  if (floorf(-0.5) != -1.0f)
    return 142;

  if (floorf(to_float(0xBf000001)) != -1.0f)  // 0.500000...1
    return 143;


  if (floorf(-8388606.5f) != -8388607.0f)
    return 171;

  if (floorf(-8388607.0f) != -8388607.0f)
    return 172;

  if (floorf(-8388607.5f) != -8388608.0f)
    return 173;

  if (floorf(-8388608.0f) != -8388608.0f)
    return 174;


  if (floorf(-2147483646.0f) != -2147483646.0f)
    return 191;

  if (floorf(-2147483648.0f) != -2147483648.0f)
    return 192;

  if (floorf(-2147483647.0f) != -2147483647.0f)
    return 193;


  if (to_bits(floorf(to_float(0xFF800000))) != 0xFF800000)
    return 201;

  if (to_bits(floorf(to_float(0x7F800000))) != 0x7F800000)
    return 202;

  if (!isnan(floorf(to_float(0xFFC00000))))
    return 203;

  if (!isnan(floorf(to_float(0x7FC00000))))
    return 204;

  if (!isnan(floorf(to_float(0x7F800001))))
    return 205;

  if (!isnan(floorf(to_float(0xFF800001))))
    return 206;

  if (to_bits(floorf(to_float(0x00000000))) != 0x00000000)
    return 207;

  if (to_bits(floorf(to_float(0x80000000))) != 0x80000000)
    return 208;

  if (to_bits(floorf(to_float(0x00000001))) != 0x00000000)
    return 209;

  if (to_bits(floorf(to_float(0x80000001))) != 0xBF800000)
    return 210;

  if (to_bits(floorf(to_float(0x007FFFFF))) != 0x00000000)
    return 211;

  if (to_bits(floorf(to_float(0x807FFFFF))) != 0xBF800000)
    return 212;

  if (to_bits(floorf(to_float(0x00800000))) != 0x00000000)
    return 213;

  if (to_bits(floorf(to_float(0x80800000))) != 0xBF800000)
    return 214;

  if (to_bits(floorf(to_float(0x3F000000))) != 0x00000000)
    return 215;

  if (to_bits(floorf(to_float(0xBF000000))) != 0xBF800000)
    return 216;

  if (to_bits(floorf(to_float(0x3F7FFFFF))) != 0x00000000)
    return 217;

  if (to_bits(floorf(to_float(0xBF7FFFFF))) != 0xBF800000)
    return 218;

  if (to_bits(floorf(to_float(0x3F800000))) != 0x3F800000)
    return 219;

  if (to_bits(floorf(to_float(0xBF800000))) != 0xBF800000)
    return 220;

  if (to_bits(floorf(to_float(0x3F800001))) != 0x3F800000)
    return 221;

  if (to_bits(floorf(to_float(0xBF800001))) != 0xC0000000)
    return 222;

  if (to_bits(floorf(to_float(0x3FC00000))) != 0x3F800000)
    return 223;

  if (to_bits(floorf(to_float(0xBFC00000))) != 0xC0000000)
    return 224;

  if (to_bits(floorf(to_float(0x3FFFFFFF))) != 0x3F800000)
    return 225;

  if (to_bits(floorf(to_float(0xBFFFFFFF))) != 0xC0000000)
    return 226;

  if (to_bits(floorf(to_float(0x4AFFFFFF))) != 0x4AFFFFFE)
    return 227;

  if (to_bits(floorf(to_float(0xCAFFFFFF))) != 0xCB000000)
    return 228;

  if (to_bits(floorf(to_float(0x4B000000))) != 0x4B000000)
    return 229;

  if (to_bits(floorf(to_float(0xCB000000))) != 0xCB000000)
    return 230;

  if (to_bits(floorf(to_float(0x4B000001))) != 0x4B000001)
    return 231;

  if (to_bits(floorf(to_float(0xCB000001))) != 0xCB000001)
    return 232;

  if (to_bits(floorf(to_float(0x7F7FFFFF))) != 0x7F7FFFFF)
    return 233;

  if (to_bits(floorf(to_float(0xFF7FFFFF))) != 0xFF7FFFFF)
    return 234;


  if (to_bits(floorf(to_float(0x3F800000))) != 0x3F800000)
    return 144;

  if (to_bits(floorf(to_float(0xBF800000))) != 0xBF800000)
    return 145;

  if (to_bits(floorf(to_float(0x3F800001))) != 0x3F800000)
    return 146;

  if (to_bits(floorf(to_float(0xBF800001))) != 0xC0000000)
    return 147;

  if (to_bits(floorf(to_float(0x3FC00000))) != 0x3F800000)
    return 148;

  if (to_bits(floorf(to_float(0xBFC00000))) != 0xC0000000)
    return 149;

  if (to_bits(floorf(to_float(0x3FFFFFFF))) != 0x3F800000)
    return 150;

  if (to_bits(floorf(to_float(0xBFFFFFFF))) != 0xC0000000)
    return 151;

  if (to_bits(floorf(to_float(0x43000000))) != 0x43000000)
    return 152;

  if (to_bits(floorf(to_float(0xC3000000))) != 0xC3000000)
    return 153;

  if (to_bits(floorf(to_float(0x43000001))) != 0x43000000)
    return 154;

  if (to_bits(floorf(to_float(0xC3000001))) != 0xC3010000)
    return 155;

  if (to_bits(floorf(to_float(0x43400000))) != 0x43400000)
    return 156;

  if (to_bits(floorf(to_float(0xC3400000))) != 0xC3400000)
    return 157;

  if (to_bits(floorf(to_float(0x437FFFFF))) != 0x437F0000)
    return 158;

  if (to_bits(floorf(to_float(0xC37FFFFF))) != 0xC3800000)
    return 159;

  if (to_bits(floorf(to_float(0x43800000))) != 0x43800000)
    return 160;

  if (to_bits(floorf(to_float(0xC3800000))) != 0xC3800000)
    return 161;

  if (to_bits(floorf(to_float(0x43800001))) != 0x43800000)
    return 162;

  if (to_bits(floorf(to_float(0xC3800001))) != 0xC3808000)
    return 163;

  if (to_bits(floorf(to_float(0x43C00000))) != 0x43C00000)
    return 164;

  if (to_bits(floorf(to_float(0xC3C00000))) != 0xC3C00000)
    return 165;

  if (to_bits(floorf(to_float(0x43FFFFFF))) != 0x43FF8000)
    return 166;

  if (to_bits(floorf(to_float(0xC3FFFFFF))) != 0xC4000000)
    return 167;

  if (to_bits(floorf(to_float(0x47000000))) != 0x47000000)
    return 168;

  if (to_bits(floorf(to_float(0xC7000000))) != 0xC7000000)
    return 169;

  if (to_bits(floorf(to_float(0x47000001))) != 0x47000000)
    return 170;

  if (to_bits(floorf(to_float(0xC7000001))) != 0xC7000100)
    return 235;

  if (to_bits(floorf(to_float(0x47400000))) != 0x47400000)
    return 236;

  if (to_bits(floorf(to_float(0xC7400000))) != 0xC7400000)
    return 237;

  if (to_bits(floorf(to_float(0x477FFFFF))) != 0x477FFF00)
    return 238;

  if (to_bits(floorf(to_float(0xC77FFFFF))) != 0xC7800000)
    return 239;

  if (to_bits(floorf(to_float(0x47800000))) != 0x47800000)
    return 240;

  if (to_bits(floorf(to_float(0xC7800000))) != 0xC7800000)
    return 241;

  if (to_bits(floorf(to_float(0x47800001))) != 0x47800000)
    return 242;

  if (to_bits(floorf(to_float(0xC7800001))) != 0xC7800080)
    return 243;

  if (to_bits(floorf(to_float(0x47C00000))) != 0x47C00000)
    return 244;

  if (to_bits(floorf(to_float(0xC7C00000))) != 0xC7C00000)
    return 245;

  if (to_bits(floorf(to_float(0x47FFFFFF))) != 0x47FFFF80)
    return 246;

  if (to_bits(floorf(to_float(0xC7FFFFFF))) != 0xC8000000)
    return 247;

  if (to_bits(floorf(to_float(0x4A800000))) != 0x4A800000)
    return 248;

  if (to_bits(floorf(to_float(0xCA800000))) != 0xCA800000)
    return 249;

  if (to_bits(floorf(to_float(0x4A800001))) != 0x4A800000)
    return 250;

  if (to_bits(floorf(to_float(0xCA800001))) != 0xCA800002)
    return 251;

  if (to_bits(floorf(to_float(0x4AC00000))) != 0x4AC00000)
    return 252;

  if (to_bits(floorf(to_float(0xCAC00000))) != 0xCAC00000)
    return 253;

  if (to_bits(floorf(to_float(0x4AFFFFFF))) != 0x4AFFFFFE)
    return 254;

  if (to_bits(floorf(to_float(0xCAFFFFFF))) != 0xCB000000)
    return 255;

	return 0;
}
