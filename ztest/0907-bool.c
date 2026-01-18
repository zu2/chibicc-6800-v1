#include "common.h"

int main(void)
{
  int a = 1, b = 2;
  char c = 5;
  long l = 1000000L;
  float f = 1.0;
  _Bool r;

  r = (a == b) || !(a == b);
  if (r != 1) {
    return 1;
  }

  r = !(a == b) || (a == b);
  if (r != 1) {
    return 2;
  }

  r = (a == b) && !(a == b);
  if (r != 0) {
    return 11;
  }

  r = !(a == b) && (a == b);
  if (r != 0) {
    return 12;
  }

  if (!(r == 0)) {
    return 13;
  }
  if (!(r != 1)) {
    return 14;
  }

  r = 5;
  if (r != 1) {
    return 20;
  }
  r = c;
  if (r != 1) {
    return 21;
  }
  r = l;
  if (r != 1) {
    return 22;
  }
  r = f;
  if (r != 1) {
    return 23;
  }

  r = 1;
  r += 255;
  if (r!=1) {
    return 30;
  }
  r = 1;
  r += 65535;
  if (r!=1) {
    return 31;
  }
  r = 1;
  r -= -65535;
  if (r!=1) {
    return 32;
  }
  r = 1;
  r *= 2;
  if (r!=1) {
    return 33;
  }
  r = 10;
  r /= 2;
  if (r!=0) {
    return 34;
  }

  return 0;
}
