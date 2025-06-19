#include <stdio.h>
#include "my_printf.c"

struct Bits {
  unsigned int a : 1;
  unsigned int b : 2;
  unsigned int c : 5;
};

struct Bits3 {
  unsigned int a : 3;
};

struct SBits2 {
  signed int a : 2;
};

struct Mix {
  unsigned a : 4;
  unsigned b : 4;
  unsigned c : 8;
};

struct Arr {
  unsigned int a : 1;
};

struct Test8 {
  unsigned int x : 8;
};

int main(int argc, char **argv)
{
  // Test 1: Simple bit field assignment and retrieval
  struct Bits x1;
  x1.a = 0;
  x1.b = 0;
  x1.c = 0;
  if (x1.a != 0 || x1.b != 0 || x1.c != 0) {
    return 11; // 1st check
  }
  x1.a = 1;
  x1.b = 3;
  x1.c = 31;
  if (x1.a != 1 || x1.b != 3 || x1.c != 31) {
    return 12; // 2nd check
  }
  x1.a = 0;
  x1.b = 0;
  x1.c = 0;
  if (x1.a != 0 || x1.b != 0 || x1.c != 0) {
    return 13; // 3rd check
  }

  // Test 2: Assigning values exceeding bit width
  struct Bits3 x2;
  x2.a = 0;
  if (x2.a != 0) {
    return 21;
  }
  x2.a = 7;
  if (x2.a != 7) {
    return 22; // max for 3 bits
  }
  x2.a = 8;
  if (x2.a != 0) {
    return 23; // only lower 3 bits stored
  }
  x2.a = 15;
  if (x2.a != 7) {
    return 24; // only lower 3 bits stored
  }
  x2.a = -1;
  if (x2.a != 7) {
    return 25; // unsigned, -1 becomes 0xFF, lower 3 bits = 7
  }

  // Test 3: Signed/unsigned bit field boundary check
  struct SBits2 x3;
  x3.a = 0;
  if (x3.a != 0) {
    return 31;
  }
  x3.a = 1;
  if (x3.a != 1) {
    printf("%d\n",x3.a);
    return 32;
  }
  x3.a = 2;
  if (x3.a != -2) {
    return 33; // 2 for 2 bits signed is -2
  }
  x3.a = 3;
  if (x3.a != -1) {
    return 34; // 3 for 2 bits signed is -1
  }
  x3.a = -1;
  if (x3.a != -1) {
    return 35;
  }
  x3.a = -2;
  if (x3.a != -2) {
    return 36;
  }
  x3.a = -3;
  if (x3.a != 1) {
    return 37; // lower 2 bits = 01
  }
  x3.a = -4;
  if (x3.a != 0) {
    return 38; // lower 2 bits = 00
  }

  // Test 4: Multiple bit fields and packing check
  struct Mix x4;
  x4.a = 0;
  x4.b = 0;
  x4.c = 0;
  if (x4.a != 0 || x4.b != 0 || x4.c != 0) {
    return 41;
  }
  x4.a = 15;
  x4.b = 15;
  x4.c = 255;
  if (x4.a != 15) {
    return 42; // max for 4 bits
  }
  if ((x4.b & 0xF) != (15 & 0xF)) {
    return 43; // 4 bits
  }
  if ((x4.c & 0xFF) != 255) {
    return 44; // 8 bits
  }
  x4.a = 16;
  if (x4.a != 0) {
    return 45; // only lower 4 bits stored
  }
  x4.b = 16;
  if (x4.b != 0) {
    return 46;
  }
  x4.c = 256;
  if (x4.c != 0) {
    return 47;
  }

  // Test 5: Bit field in struct array
  struct Arr arr[2];
  arr[0].a = 0;
  arr[1].a = 1;
  if (arr[0].a != 0 || arr[1].a != 1) {
    return 51;
  }
  arr[0].a = 1;
  arr[1].a = 0;
  if (arr[0].a != 1 || arr[1].a != 0) {
    return 52;
  }
  arr[0].a = 2;
  if (arr[0].a != 0) {
    return 53; // 2 for 1 bit is 0
  }
  arr[1].a = -1;
  if (arr[1].a != 1) {
    return 54; // unsigned 1 bit, -1 is 1
  }

  // Test 6: Overwriting all bit patterns
  struct Test8 t6;
  int i;
  for (i = 0; i < 256; i++) {
    t6.x = i;
    if (t6.x != i) {
      return 61;
    }
  }
  t6.x = 257;
  if (t6.x != 1) {
    return 62;
  }
  t6.x = -1;
  if (t6.x != 255) {
    return 63;
  }
  t6.x = -256;
  if (t6.x != 0) {
    return 64;
  }

  return 0;
}
