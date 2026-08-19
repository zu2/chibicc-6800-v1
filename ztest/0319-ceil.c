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
  if (!isnan(ceilf(NAN)))
    return 1;

  if (ceilf(INFINITY) != INFINITY)
    return 2;

  if (ceilf(0.0f) != 0.0f)
    return 3;

  if (ceilf(FLT_TRUE_MIN) != 1.0f)
    return 4;


	if (ceilf(0.0) != 0.0)
		return 11;

	if (ceilf(1.0) != 1.0)
		return 12;

	if (ceilf(2.0) != 2.0)
		return 13;

	if (ceilf(3.0) != 3.0)
		return 14;

	if (ceilf(4.0) != 4.0)
		return 15;

	if (ceilf(8.0) != 8.0)
		return 16;


	if (ceilf(2.3) != 3.0)
		return 21;

	if (ceilf(2.4) != 3.0)
		return 22;

	if (ceilf(2.5) != 3.0)
		return 23;

	if (ceilf(2.6) != 3.0)
		return 24;

	if (ceilf(2.7) != 3.0)
		return 25;

  if (ceilf(to_float(0x401FFFFF))!=3.0)  // 2.5 - LSB=1
    return 26;

  if (ceilf(to_float(0x40200001))!=3.0)  // 2.5 + LSB=1
    return 27;


	if (ceilf(3.3) != 4.0)
		return 31;

	if (ceilf(3.4) != 4.0)
		return 31;

	if (ceilf(3.5) != 4.0)
		return 32;

	if (ceilf(3.6) != 4.0)
		return 33;

	if (ceilf(3.7) != 4.0)
		return 34;

  if (ceilf(to_float(0x405FFFFF))!=4.0)  // 3.5 - LSB=1
    return 35;

  if (ceilf(to_float(0x40600001))!=4.0)  // 3.5 + LSB=1
    return 36;


  if (ceilf(to_float(0x3eFFFFFF)) != 1.0f) // 0.49999..
    return 41;

  if (ceilf(0.5) != 1.0f)
    return 42;

  if (ceilf(to_float(0x3f000001)) != 1.0f)  // 0.500000...1
    return 43;


  if (ceilf(8388606.5f) != 8388607.0f)
    return 71;

  if (ceilf(8388607.0f) != 8388607.0f)
    return 72;

  if (ceilf(8388607.5f) != 8388608.0f)
    return 73;

  if (ceilf(8388608.0f) != 8388608.0f)
    return 74;


  if (ceilf(2147483646.0f) != 2147483646.0f)
    return 91;

  if (ceilf(2147483648.0f) != 2147483648.0f)
    return 92;

  if (ceilf(2147483647.0f) != 2147483647.0f)
    return 93;


	if (ceilf(-0.0) != -0.0)
		return 111;

	if (ceilf(-1.0) != -1.0)
		return 112;

	if (ceilf(-2.0) != -2.0)
		return 113;

	if (ceilf(-3.0) != -3.0)
		return 114;

	if (ceilf(-4.0) != -4.0)
		return 115;

	if (ceilf(-8.0) != -8.0)
		return 116;


	if (ceilf(-2.3) != -2.0)
		return 121;

	if (ceilf(-2.4) != -2.0)
		return 122;

	if (ceilf(-2.5) != -2.0)
		return 123;

	if (ceilf(-2.6) != -2.0)
		return 124;

	if (ceilf(-2.7) != -2.0)
		return 125;

  if (ceilf(to_float(0xC01FFFFF))!=-2.0)  // 2.5 - LSB=1
    return 126;

  if (ceilf(to_float(0xC0200001))!=-2.0)  // 2.5 + LSB=1
    return 127;


	if (ceilf(-3.3) != -3.0)
		return 131;

	if (ceilf(-3.4) != -3.0)
		return 131;

	if (ceilf(-3.5) != -3.0)
		return 132;

	if (ceilf(-3.6) != -3.0)
		return 133;

	if (ceilf(-3.7) != -3.0)
		return 134;

  if (ceilf(to_float(0xC05FFFFF))!=-3.0)  // 3.5 - LSB=1
    return 135;

  if (ceilf(to_float(0xC0600001))!=-3.0)  // 3.5 + LSB=1
    return 136;


  if (ceilf(to_float(0xBeFFFFFF)) != -0.0f) // 0.49999..
    return 141;

  if (ceilf(-0.5) != -0.0f)
    return 142;

  if (ceilf(to_float(0xBf000001)) != -0.0f)  // 0.500000...1
    return 143;


  if (ceilf(-8388606.5f) != -8388606.0f)
    return 171;

  if (ceilf(-8388607.0f) != -8388607.0f)
    return 172;

  if (ceilf(-8388607.5f) != -8388607.0f)
    return 173;

  if (ceilf(-8388608.0f) != -8388608.0f)
    return 174;


  if (ceilf(-2147483646.0f) != -2147483646.0f)
    return 191;

  if (ceilf(-2147483648.0f) != -2147483648.0f)
    return 192;

  if (ceilf(-2147483647.0f) != -2147483647.0f)
    return 193;


  if (to_bits(ceilf(to_float(0xFF800000))) != 0xFF800000)
    return 201;

  if (to_bits(ceilf(to_float(0x7F800000))) != 0x7F800000)
    return 202;

  if (!isnan(ceilf(to_float(0xFFC00000))))
    return 203;

  if (!isnan(ceilf(to_float(0x7FC00000))))
    return 204;

  if (!isnan(ceilf(to_float(0x7F800001))))
    return 205;

  if (!isnan(ceilf(to_float(0xFF800001))))
    return 206;

  if (to_bits(ceilf(to_float(0x00000000))) != 0x00000000)
    return 207;

  if (to_bits(ceilf(to_float(0x80000000))) != 0x80000000)
    return 208;

  if (to_bits(ceilf(to_float(0x00000001))) != 0x3F800000)
    return 209;

  if (to_bits(ceilf(to_float(0x80000001))) != 0x80000000)
    return 210;

  if (to_bits(ceilf(to_float(0x007FFFFF))) != 0x3F800000)
    return 211;

  if (to_bits(ceilf(to_float(0x807FFFFF))) != 0x80000000)
    return 212;

  if (to_bits(ceilf(to_float(0x00800000))) != 0x3F800000)
    return 213;

  if (to_bits(ceilf(to_float(0x80800000))) != 0x80000000)
    return 214;

  if (to_bits(ceilf(to_float(0x3F000000))) != 0x3F800000)
    return 215;

  if (to_bits(ceilf(to_float(0xBF000000))) != 0x80000000)
    return 216;

  if (to_bits(ceilf(to_float(0x3F7FFFFF))) != 0x3F800000)
    return 217;

  if (to_bits(ceilf(to_float(0xBF7FFFFF))) != 0x80000000)
    return 218;

  if (to_bits(ceilf(to_float(0x3F800000))) != 0x3F800000)
    return 219;

  if (to_bits(ceilf(to_float(0xBF800000))) != 0xBF800000)
    return 220;

  if (to_bits(ceilf(to_float(0x3F800001))) != 0x40000000)
    return 221;

  if (to_bits(ceilf(to_float(0xBF800001))) != 0xBF800000)
    return 222;

  if (to_bits(ceilf(to_float(0x3FC00000))) != 0x40000000)
    return 223;

  if (to_bits(ceilf(to_float(0xBFC00000))) != 0xBF800000)
    return 224;

  if (to_bits(ceilf(to_float(0x3FFFFFFF))) != 0x40000000)
    return 225;

  if (to_bits(ceilf(to_float(0xBFFFFFFF))) != 0xBF800000)
    return 226;

  if (to_bits(ceilf(to_float(0x4AFFFFFF))) != 0x4B000000)
    return 227;

  if (to_bits(ceilf(to_float(0xCAFFFFFF))) != 0xCAFFFFFE)
    return 228;

  if (to_bits(ceilf(to_float(0x4B000000))) != 0x4B000000)
    return 229;

  if (to_bits(ceilf(to_float(0xCB000000))) != 0xCB000000)
    return 230;

  if (to_bits(ceilf(to_float(0x4B000001))) != 0x4B000001)
    return 231;

  if (to_bits(ceilf(to_float(0xCB000001))) != 0xCB000001)
    return 232;

  if (to_bits(ceilf(to_float(0x7F7FFFFF))) != 0x7F7FFFFF)
    return 233;

  if (to_bits(ceilf(to_float(0xFF7FFFFF))) != 0xFF7FFFFF)
    return 234;

	return 0;
}
