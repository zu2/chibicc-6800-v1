#include "common.h"

int a[1024];

int main(argc, char **argv)
{
  int b[1024];
  int *c = &b[1000];
  long asum = 0;
  long bsum = 0;

  for (int i = 0; i < 1000; i = i + 1) {
    a[i] = i;
  }
  for (int i = 0; i < 1000; i = i + 1) {
    asum += a[i];
  }
  //	puthexl(asum);
  if (asum != 499500) {
    return 1;
  }

  for (int i = 0; i < 1000; i = i + 1) {
    b[i] = i;
  }
  for (int i = 0; i < 1000; i = i + 1) {
    bsum += b[i];
  }
  //	puthexl(bsum);
  if (bsum != 499500) {
    return 11;
  }

  if (a[0] != 0) {
    return 2;
  }

  if (a[999] != 999) {
    return 3;
  }

  if (b[0] != 0) {
    return 12;
  }

  if (b[999] != 999) {
    return 13;
  }

  if (c[-1] != 999) {
    return 14;
  }

  return 0;
}
