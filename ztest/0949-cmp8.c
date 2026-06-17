#include <stdio.h>

int main(int argc, char **argv)
{
  int i;
  char x;
  int y;
  int z;

  for (int i=-128; i<128; i++) {
    x = i;
    y = (x==0);
    z = (i==0);
    if (y ^ z) {
      return 1;
    }
  }
  for (int i=-128; i<128; i++) {
    x = i;
    y = (x!=0);
    z = (i!=0);
    if (y ^ z) {
      return 2;
    }
  }
  for (int i=-128; i<128; i++) {
    x = i;
    y = (x>=0);
    z = (i>=0);
    if (y ^ z) {
      return 3;
    }
  }
  for (int i=-128; i<128; i++) {
    x = i;
    y = (x>0);
    z = (i>0);
    if (y ^ z) {
      return 4;
    }
  }
  for (int i=-128; i<128; i++) {
    x = i;
    y = (x<=0);
    z = (i<=0);
    if (y ^ z) {
      return 5;
    }
  }
  for (int i=-128; i<128; i++) {
    x = i;
    y = (x<0);
    z = (i<0);
    if (y ^ z) {
      return 6;
    }
  }
  return 0;
}
