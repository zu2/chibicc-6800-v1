//
// check ilogbf
//
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  if (ilogbf(0.0)!=FP_ILOGB0)
    return 1;
  if (ilogbf(-0.0)!=FP_ILOGB0)
    return 2;

  if (ilogbf(INFINITY)!=INT_MAX)
    return 3;
  if (ilogbf(-INFINITY)!=INT_MAX)
    return 4;

  if (ilogbf(NAN)!=FP_ILOGBNAN)
    return 5;

  if (ilogbf(1.0f) != 0)
    return 21;
  if (ilogbf(1.5f) != 0)
    return 22;
  if (ilogbf(1.9999999f) != 0)
    return 23;

  if (ilogbf(2.0f) != 1)
    return 31;
  if (ilogbf(3.0f) != 1)
    return 32;
  if (ilogbf(4.0f) != 2)
    return 33;
  if (ilogbf(0.5f) != -1)
    return 34;
  if (ilogbf(0.25f) != -2)
    return 35;

  if (ilogbf(-1.0f) != 0)
    return 41;
  if (ilogbf(-1.5f) != 0)
    return 42;
  if (ilogbf(-1.9999999f) != 0)
    return 43;

  if (ilogbf(-2.0f) != 1)
    return 51;
  if (ilogbf(-3.0f) != 1)
    return 52;
  if (ilogbf(-4.0f) != 2)
    return 53;
  if (ilogbf(-0.5f) != -1)
    return 54;
  if (ilogbf(-0.25f) != -2)
    return 55;

  if (ilogbf(0x1.fffffep+0f) != 0)
    return 61;
  if (ilogbf(0x1.0p+1f) != 1)
    return 62;
  if (ilogbf(0x1.0p-1f) != -1)
    return 63;
  if (ilogbf(0x1.fffffep-2f) != -2)
    return 64;

  if (ilogbf(-0x1.fffffep+0f) != 0)
    return 71;
  if (ilogbf(-0x1.0p+1f) != 1)
    return 72;
  if (ilogbf(-0x1.0p-1f) != -1)
    return 73;
  if (ilogbf(-0x1.fffffep-2f) != -2)
    return 74;

  if (ilogbf(0x1.0p-126f) != -126)
    return 101;
  if (ilogbf(0x1.fffffep+127f) != 127)
    return 102;
  if (ilogbf(-0x1.0p-126f) != -126)
    return 103;
  if (ilogbf(-0x1.fffffep+127f) != 127)
    return 104;

  // subnormal
  if (ilogbf(0x1.0p-149f) != -149)
    return 121;
  if (ilogbf(0x1.fffffcp-127f) != -127)
    return 122;
  if (ilogbf(0x1.0p-148f) != -148)
    return 123;
  if (ilogbf(0x1.0p-147f) != -147)
    return 124;

  if (ilogbf(-0x1.0p-149f) != -149)
    return 131;
  if (ilogbf(-0x1.fffffcp-127f) != -127)
    return 132;
  if (ilogbf(-0x1.0p-148f) != -148)
    return 133;
  if (ilogbf(-0x1.0p-147f) != -147)
    return 134;


  return 0;
}
