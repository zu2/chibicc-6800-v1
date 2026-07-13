#include "common.h"

// 16-bit signed integer divide
int div(int a, int b) {
    return a / b;
}  

int main(void)
{
// Group 1
  if (div(0, 1) != 0) return 10;
  if (div(1, 1) != 1) return 11;
  if (div(-1, -1) != 1) return 12;
  if (div(1, -1) != -1) return 13;
  if (div(-1, 1) != -1) return 14;
  if (div(2, 2) != 1) return 15;
  if (div(-2, -2) != 1) return 16;
  if (div(32767, 1) != 32767) return 17;
  if (div(-32768, 1) != -32768) return 18;
  if (div(32767, -1) != -32767) return 19;
  if (div(-32768, -1) != -32768) return 20;
  if (div(32767, 2) != 16383) return 21;
  if (div(-32768, 2) != -16384) return 22;
  if (div(3, 2) != 1) return 23;
  if (div(4, 3) != 1) return 24;
  if (div(32767, 32767) != 1) return 25;
  if (div(-32768, -32768) != 1) return 26;
  if (div(32767, -32768) != 0) return 27;
  if (div(-32768, 32767) != -1) return 28;
  if (div(-256, 255) != -1) return 29;
  if (div(255, -256) != 0) return 30;
  if (div(-256, 255) != -1) return 31;
  if (div(255, -256) != 0) return 32;
// Group 2
  if (div(-256, 255) != -1) return 40;
  if (div(255, -256) != 0) return 41;
  if (div(-32768, 1) != -32768) return 42;
  if (div(32512, 256) != 127) return 43;
  if (div(255, -1) != -255) return 44;
  if (div(-32768, 256) != -128) return 45;
  if (div(32767, 256) != 127) return 46;
  if (div(255, 3) != 85) return 47;
  if (div(-256, -2) != 128) return 48;
  if (div(255, 2) != 127) return 49;
  if (div(-256, 2) != -128) return 50;
  if (div(-256, 255) != -1) return 51;
  if (div(255, 255) != 1) return 52;
  if (div(127, 1) != 127) return 53;
  if (div(-127, 1) != -127) return 54;
  if (div(123, 456) != 0) return 55;
  if (div(-24575, 384) != -63) return 56;
  if (div(255, -255) != -1) return 57;
  if (div(-32767, 32766) != -1) return 58;
// Group 3
  if (div(30000, 12345) != 2) return 60;
  if (div(-30000, -12345) != 2) return 61;
  if (div(12345, -30000) != 0) return 62;
  if (div(-12345, 30000) != 0) return 63;
  if (div(32766, 2) != 16383) return 64;
  if (div(-32768, -32767) != 1) return 65;
  if (div(32767, -32766) != -1) return 66;
  if (div(32766, 3) != 10922) return 67;
  if (div(10922, 32766) != 0) return 68;
  if (div(-5, 4) != -1) return 69;
  if (div(13, 7) != 1) return 70;
  if (div(-13, 7) != -1) return 71;
  if (div(13, -7) != -1) return 72;
  if (div(-13, -7) != 1) return 73;
// Group 4
  if (div(-25866, 4247) != -6) return 80;
  if (div(-6073, -20361) != 0) return 81;
  if (div(7917, 8340) != 0) return 82;
  if (div(3864, 27782) != 0) return 83;
  if (div(-9644, -120) != 80) return 84;
  if (div(-27641, -23713) != 1) return 85;
  if (div(32081, 13352) != 2) return 86;
  if (div(6940, -752) != -9) return 87;
  if (div(-26666, -28966) != 0) return 88;
  if (div(-16188, -23646) != 0) return 89;
  if (div(29601, -13397) != -2) return 90;
  if (div(2821, 25971) != 0) return 91;
  if (div(-9210, -698) != 13) return 92;
  if (div(-535, 14404) != 0) return 93;
  if (div(12507, -12461) != -1) return 94;
  if (div(20593, 9073) != 2) return 95;
  if (div(14078, -7680) != -1) return 96;
  if (div(-32691, -30639) != 1) return 97;
  if (div(-6123, 8545) != 0) return 98;
  if (div(-3450, 9424) != 0) return 99;

  return 0;
}
