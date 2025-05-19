//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

int main(int argc, char **argv)
{
  if (!isnan(truncf(NAN)))
    return 1;

  if (truncf(INFINITY) != INFINITY)
    return 2;

  if (truncf(0.0f) != 0.0f)
    return 3;

  if (truncf(FLT_TRUE_MIN) != 0.0f)
    return 4;


	if (truncf(0.0) != 0.0)
		return 11;

	if (truncf(1.0) != 1.0)
		return 12;

	if (truncf(2.0) != 2.0)
		return 13;

	if (truncf(3.0) != 3.0)
		return 14;

	if (truncf(4.0) != 4.0)
		return 15;

	if (truncf(8.0) != 8.0)
		return 16;


	if (truncf(2.3) != 2.0)
		return 21;

	if (truncf(2.4) != 2.0)
		return 21;

	if (truncf(2.5) != 2.0)
		return 22;

	if (truncf(2.6) != 2.0)
		return 23;

	if (truncf(2.7) != 2.0)
		return 24;

  if (truncf(to_float(0x401FFFFF))!=2.0)  // 2.5 - LSB=1
    return 25;

  if (truncf(to_float(0x40200001))!=2.0)  // 2.5 + LSB=1
    return 26;


	if (truncf(3.3) != 3.0)
		return 31;

	if (truncf(3.4) != 3.0)
		return 31;

	if (truncf(3.5) != 3.0)
		return 32;

	if (truncf(3.6) != 3.0)
		return 33;

	if (truncf(3.7) != 3.0)
		return 34;

  if (truncf(to_float(0x405FFFFF))!=3.0)  // 3.5 - LSB=1
    return 35;

  if (truncf(to_float(0x40600001))!=3.0)  // 3.5 + LSB=1
    return 36;


  if (truncf(to_float(0x3eFFFFFF)) != 0.0f) // 0.49999..
    return 41;

  if (truncf(0.5) != 0.0f)
    return 42;

  if (truncf(to_float(0x3f000001)) != 0.0f)  // 0.500000...1
    return 43;


  if (truncf(8388606.5f) != 8388606.0f)
    return 71;

  if (truncf(8388607.0f) != 8388607.0f)
    return 72;

  if (truncf(8388607.5f) != 8388607.0f)
    return 73;

  if (truncf(8388608.0f) != 8388608.0f)
    return 74;


  if (truncf(2147483646.0f) != 2147483646.0f)
    return 91;

  if (truncf(2147483648.0f) != 2147483648.0f)
    return 92;

  if (truncf(2147483647.0f) != 2147483647.0f)
    return 93;


	if (truncf(-0.0) != -0.0)
		return 111;

	if (truncf(-1.0) != -1.0)
		return 112;

	if (truncf(-2.0) != -2.0)
		return 113;

	if (truncf(-3.0) != -3.0)
		return 114;

	if (truncf(-4.0) != -4.0)
		return 115;

	if (truncf(-8.0) != -8.0)
		return 116;


	if (truncf(-2.3) != -2.0)
		return 121;

	if (truncf(-2.4) != -2.0)
		return 121;

	if (truncf(-2.5) != -2.0)
		return 122;

	if (truncf(-2.6) != -2.0)
		return 123;

	if (truncf(-2.7) != -2.0)
		return 124;

  if (truncf(to_float(0xC01FFFFF))!=-2.0)  // 2.5 - LSB=1
    return 125;

  if (truncf(to_float(0xC0200001))!=-2.0)  // 2.5 + LSB=1
    return 126;


	if (truncf(-3.3) != -3.0)
		return 131;

	if (truncf(-3.4) != -3.0)
		return 131;

	if (truncf(-3.5) != -3.0)
		return 132;

	if (truncf(-3.6) != -3.0)
		return 133;

	if (truncf(-3.7) != -3.0)
		return 134;

  if (truncf(to_float(0xC05FFFFF))!=-3.0)  // 3.5 - LSB=1
    return 135;

  if (truncf(to_float(0xC0600001))!=-3.0)  // 3.5 + LSB=1
    return 136;


  if (truncf(to_float(0xBeFFFFFF)) != -0.0f) // 0.49999..
    return 141;

  if (truncf(-0.5) != -0.0f)
    return 142;

  if (truncf(to_float(0xBf000001)) != -0.0f)  // 0.500000...1
    return 143;


  if (truncf(-8388606.5f) != -8388606.0f)
    return 171;

  if (truncf(-8388607.0f) != -8388607.0f)
    return 172;

  if (truncf(-8388607.5f) != -8388607.0f)
    return 173;

  if (truncf(-8388608.0f) != -8388608.0f)
    return 174;


  if (truncf(-2147483646.0f) != -2147483646.0f)
    return 191;

  if (truncf(-2147483648.0f) != -2147483648.0f)
    return 192;

  if (truncf(-2147483647.0f) != -2147483647.0f)
    return 193;

	return 0;
}
