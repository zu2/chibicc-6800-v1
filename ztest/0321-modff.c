//
//	float library
//

#include <float.h>
#include <math.h>
#include <stdint.h>
#include "common.h"

#define to_float(x) (*(const float*)&(uint32_t){x})

int main(int argc, char **argv)
{
  float int_part;

  if (!isnan(modff(NAN,&int_part)) || !isnan(int_part))
    return 1;

  if (modff(INFINITY,&int_part)!=0.0f || int_part != INFINITY)
    return 2;

  if (modff(0.0f,&int_part) != 0.0f || int_part != 0.0f)
    return 3;

  if (modff(FLT_TRUE_MIN,&int_part) != FLT_TRUE_MIN || int_part != 0.0f)
    return 4;


	if (modff(0.0,&int_part) !=0.0 || int_part != 0.0)
		return 11;

	if (modff(1.0,&int_part) !=0.0 || int_part != 1.0)
		return 12;

	if (modff(2.0,&int_part) !=0.0 || int_part != 2.0)
		return 13;

	if (modff(3.0,&int_part) !=0.0 || int_part != 3.0)
		return 14;

	if (modff(4.0,&int_part) !=0.0 || int_part != 4.0)
		return 15;

	if (modff(8.0,&int_part) !=0.0 || int_part != 8.0)
		return 16;


	if (modff(2.3,&int_part) != to_float(0x3E999998) || int_part != 2.0)
		return 21;

	if (modff(2.4,&int_part) != to_float(0x3ECCCCD0) || int_part != 2.0)
		return 22;

	if (modff(2.5,&int_part) != 0.5 || int_part != 2.0)
		return 23;

	if (modff(2.6,&int_part) != to_float(0x3F199998) || int_part != 2.0)
		return 24;

	if (modff(2.7,&int_part) != to_float(0x3F333334) || int_part != 2.0)
		return 25;

  if (modff(to_float(0x401FFFFF),&int_part)!=to_float(0x3EFFFFF8)
  ||  int_part != 2.0)  //2.5-LSB=1
    return 26;

  if (modff(to_float(0x40200001),&int_part)!=to_float(0x3F000004)
  ||  int_part != 2.0)  //2.5+LSB=1
    return 27;


	if (modff(3.3,&int_part) != to_float(0x3E999998) || int_part != 3.0)
		return 31;

	if (modff(3.4,&int_part) != to_float(0x3ECCCCD0) || int_part != 3.0)
		return 32;

	if (modff(3.5,&int_part) != 0.5 || int_part != 3.0)
		return 33;

	if (modff(3.6,&int_part) != to_float(0x3F199998) || int_part != 3.0)
		return 34;

	if (modff(3.7,&int_part) != to_float(0x3F333334) || int_part != 3.0)
		return 35;

  if (modff(to_float(0x405FFFFF),&int_part) != to_float(0x3EFFFFF8)
  ||  int_part != 3.0)  // 3.5 - LSB=1
    return 36;

  if (modff(to_float(0x40600001),&int_part) != to_float(0x3F000004)
  ||  int_part != 3.0)  // 3.5 + LSB=1
    return 37;


  if (modff(to_float(0x3EFFFFFF),&int_part) != to_float(0x3EFFFFFF)
  ||  int_part != 0.0f) // 0.49999..
    return 41;

  if (modff(0.5,&int_part) != 0.5f && int_part != 0.0f)
    return 42;

  if (modff(to_float(0x3F000001),&int_part) != to_float(0x3F000001)
  ||  int_part != 0.0f)  // 0.500000...1
    return 43;


  if (modff(8388606.5f,&int_part) != 0.5f && int_part != 8388607.0f)
    return 71;

  if (modff(8388607.0f,&int_part) != 0.0f && int_part != 8388607.0f)
    return 72;

  if (modff(8388607.5f,&int_part) != 0.5f && int_part != 8388607.0f)
    return 73;

  if (modff(8388608.0f,&int_part) != 0.0f && int_part != 8388608.0f)
    return 74;

  if (modff(2147483646.0f,&int_part) !=0.0f && int_part  != 2147483646.0f)
    return 91;

  if (modff(2147483648.0f,&int_part) !=0.0f && int_part  != 2147483648.0f)
    return 92;

  if (modff(2147483647.0f,&int_part) !=0.0f && int_part  != 2147483647.0f)
    return 93;


  if (!isnan(modff(-NAN,&int_part)) || !isnan(int_part))
    return 101;

  if (modff(-INFINITY,&int_part)!=0.0f || int_part != -INFINITY)
    return 102;

  if (modff(0.0f,&int_part) != 0.0f || int_part != 0.0f)
    return 103;

  if (modff(FLT_TRUE_MIN,&int_part) != FLT_TRUE_MIN || int_part != 0.0f)
    return 104;


	if (modff(-0.0,&int_part) !=0.0 || int_part != 0.0)
		return 111;

	if (modff(-1.0,&int_part) !=0.0 || int_part != -1.0)
		return 112;

	if (modff(-2.0,&int_part) !=0.0 || int_part != -2.0)
		return 113;

	if (modff(-3.0,&int_part) !=0.0 || int_part != -3.0)
		return 114;

	if (modff(-4.0,&int_part) !=0.0 || int_part != -4.0)
		return 115;

	if (modff(-8.0,&int_part) !=0.0 || int_part != -8.0)
		return 116;

	if (modff(-2.3,&int_part) != to_float(0xBE999998) || int_part != -2.0)
		return 121;

	if (modff(-2.4,&int_part) != to_float(0xBECCCCD0) || int_part != -2.0)
		return 122;

	if (modff(-2.5,&int_part) != -0.5 || int_part != -2.0)
		return 123;

	if (modff(-2.6,&int_part) != to_float(0xBF199998) || int_part != -2.0)
		return 124;

	if (modff(-2.7,&int_part) != to_float(0xBF333334) || int_part != -2.0)
		return 125;

  if (modff(to_float(0xC01FFFFF),&int_part)!=to_float(0xBEFFFFF8)
  ||  int_part != -2.0)  //2.5-LSB=1
    return 126;

  if (modff(to_float(0xC0200001),&int_part)!=to_float(0xBF000004)
  ||  int_part != -2.0)  //2.5+LSB=1
    return 127;


	if (modff(-3.3,&int_part) != to_float(0xBE999998) || int_part != -3.0)
		return 131;

	if (modff(-3.4,&int_part) != to_float(0xBECCCCD0) || int_part != -3.0)
		return 132;

	if (modff(-3.5,&int_part) != -0.5 || int_part != -3.0)
		return 133;

	if (modff(-3.6,&int_part) != to_float(0xBF199998) || int_part != -3.0)
		return 134;

	if (modff(-3.7,&int_part) != to_float(0xBF333334) || int_part != -3.0)
		return 135;

  if (modff(to_float(0xC05FFFFF),&int_part) != to_float(0xBEFFFFF8)
  ||  int_part != -3.0)  // 3.5 - LSB=1
    return 136;

  if (modff(to_float(0xC0600001),&int_part) != to_float(0xBF000004)
  ||  int_part != -3.0)  // 3.5 + LSB=1
    return 137;


  if (modff(to_float(0xBEFFFFFF),&int_part) != to_float(0xBEFFFFFF)
  ||  int_part != 0.0f) // 0.49999..
    return 141;

  if (modff(-0.5,&int_part) != 0.5f && int_part != 0.0f)
    return 142;

  if (modff(to_float(0xBF000001),&int_part) != to_float(0xBF000001)
  ||  int_part != 0.0f)  // 0.500000...1
    return 143;


  if (modff(-8388606.5f,&int_part) != -0.5f && int_part != -8388607.0f)
    return 171;

  if (modff(-8388607.0f,&int_part) != 0.0f && int_part != -8388607.0f)
    return 172;

  if (modff(-8388607.5f,&int_part) != -0.5f && int_part != -8388607.0f)
    return 173;

  if (modff(-8388608.0f,&int_part) != 0.0f && int_part != -8388608.0f)
    return 174;

  if (modff(-2147483646.0f,&int_part) != 0.0f && int_part  != -2147483646.0f)
    return 191;

  if (modff(-2147483648.0f,&int_part) != 0.0f && int_part  != -2147483648.0f)
    return 192;

  if (modff(-2147483647.0f,&int_part) != 0.0f && int_part  != -2147483647.0f)
    return 193;


	return 0;
}
