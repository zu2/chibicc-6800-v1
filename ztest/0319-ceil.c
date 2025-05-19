//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

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

	return 0;
}
