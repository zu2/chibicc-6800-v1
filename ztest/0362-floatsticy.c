#include "common.h"

#if !defined(__CHIBICC__)
#include <stdio.h>
#include <stdint.h>

#define putstr(s) fputs(s,stdout);

void puthexf(float x)
{
  printf("%08x",*(uint32_t *)&x);
}
#endif

float to_float(long x)
{
  return *(float *)&x;
}

int main(int argc, char **argv)
{
  float One        = to_float(0x3f800000);
  float U2         = to_float(0x34000000);
  float OneAndHalf = to_float(0x3fc00000);
  float Half       = to_float(0x3f000000);
  float Radix      = to_float(0x40000000);
  float Zero       = to_float(0x00000000);

  float Z,T,X,Y;
  float result;

  Z = (One + U2) * OneAndHalf;
  putstr("Z:");puthexf(Z); putchar('\n');
  T = (OneAndHalf + U2) - Z + U2;
  putstr("T:");puthexf(T); putchar('\n');
  X = One + Half / Radix;
  putstr("X:");puthexf(X); putchar('\n');
  Y = One + Radix * U2;
  putstr("Y:");puthexf(Y); putchar('\n');
  Z = X * Y;
  putstr("Z:");puthexf(Z); putchar('\n');

// 1.250000+2.000000*0.000000-1.250000=0.000000 0
  result = X + Radix*U2 - Z;

#if defined(__CHIBICC_6800__)
  puthexf(result); putchar('\n');
  // B4000000 : -0.00000011920928955078125
#endif

  return (result != Zero);
}
