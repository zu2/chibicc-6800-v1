#include "common.h"

typedef	unsigned char u8;
typedef unsigned int  u16;
typedef   signed int  s16;

int main(int argc, char **argv)
{
  u8  f,u;
  u16 v;

  u = 1;
  v = 255;

  if (f = !u && (s16)v<0)
    return 1;

  return 0;
}

//if (f = !u && (s16)v < 0) v = -v;

