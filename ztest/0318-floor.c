//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

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
		return 31;

	if (floorf(3.5) != 3.0)
		return 32;

	if (floorf(3.6) != 3.0)
		return 33;

	if (floorf(3.7) != 3.0)
		return 34;

  if (floorf(to_float(0x405FFFFF))!=3.0)  // 3.5 - LSB=1
    return 35;

  if (floorf(to_float(0x40600001))!=3.0)  // 3.5 + LSB=1
    return 36;


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
		return 131;

	if (floorf(-3.5) != -4.0)
		return 132;

	if (floorf(-3.6) != -4.0)
		return 133;

	if (floorf(-3.7) != -4.0)
		return 134;

  if (floorf(to_float(0xC05FFFFF))!=-4.0)  // 3.5 - LSB=1
    return 135;

  if (floorf(to_float(0xC0600001))!=-4.0)  // 3.5 + LSB=1
    return 136;


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

	return 0;
}
