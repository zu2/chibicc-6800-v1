//
//	float library
//

#include <float.h>
#include <math.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

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
		return 21;

	if (nearbyintf(2.5) != 2.0)
		return 22;

	if (nearbyintf(2.6) != 3.0)
		return 23;

	if (nearbyintf(2.7) != 3.0)
		return 24;

  if (nearbyintf(to_float(0x401FFFFF))!=2.0)  // 2.5 - LSB=1
    return 25;

  if (nearbyintf(to_float(0x40200001))!=3.0)  // 2.5 + LSB=1
    return 26;


	if (nearbyintf(3.3) != 3.0)
		return 31;

	if (nearbyintf(3.4) != 3.0)
		return 31;

	if (nearbyintf(3.5) != 4.0)
		return 32;

	if (nearbyintf(3.6) != 4.0)
		return 33;

	if (nearbyintf(3.7) != 4.0)
		return 34;

  if (nearbyintf(to_float(0x405FFFFF))!=3.0)  // 3.5 - LSB=1
    return 35;

  if (nearbyintf(to_float(0x40600001))!=4.0)  // 3.5 + LSB=1
    return 36;


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
		return 121;

	if (nearbyintf(-2.5) != -2.0)
		return 122;

	if (nearbyintf(-2.6) != -3.0)
		return 123;

	if (nearbyintf(-2.7) != -3.0)
		return 124;

  if (nearbyintf(to_float(0xc01FFFFF))!=-2.0)  // 2.5 - LSB=1
    return 125;

  if (nearbyintf(to_float(0xc0200001))!=-3.0)  // 2.5 + LSB=1
    return 126;


	if (nearbyintf(-3.3) != -3.0)
		return 131;

	if (nearbyintf(-3.4) != -3.0)
		return 131;

	if (nearbyintf(-3.5) != -4.0)
		return 132;

	if (nearbyintf(-3.6) != -4.0)
		return 133;

	if (nearbyintf(-3.7) != -4.0)
		return 134;

  if (nearbyintf(to_float(0xC05FFFFF))!=-3.0)  // 3.5 - LSB=1
    return 135;

  if (nearbyintf(to_float(0xC0600001))!=-4.0)  // 3.5 + LSB=1
    return 136;


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

	return 0;
}
