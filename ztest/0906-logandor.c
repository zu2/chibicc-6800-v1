#include "common.h"

int func1(void)
{
  if (0 || 1) {
    return 1;
  }

  return 0;
}

int func2(void)
{
  if (1 && 0) {
    return 1;
  }

  return 0;
}

int main(void)
{
  int a = 1, b = 2;
  int c = 0;
  int r;

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

  if ((c && a) || (c && b)) {
    return 13;
  }

  if (!func1()) {
    return 21;
  }

  if (func2()) {
    return 22;
  }

  return 0;
}
