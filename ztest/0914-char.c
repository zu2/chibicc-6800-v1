#include "common.h"

  signed char gsc = -10;
unsigned char guc = 10;

int
main(int argc, char **argv)
{
    signed char sc = -10;
  unsigned char uc = 10;
  int x;

  x = sc+sc;
  x = uc+uc;
  uc = uc+uc;

  x = sc-sc;
  x = uc-uc;
  uc = uc-uc;

  x = sc+uc;
  x = uc+sc;

  x = sc-uc;
  x = uc-sc;

  x = gsc+gsc;
  x = guc+guc;
  guc = guc+guc;

  x = gsc-gsc;
  x = guc-guc;
  guc = guc-guc;


  x = gsc+guc;
  x = guc+gsc;

  x = gsc-guc;
  x = guc-gsc;

  return  0;
}
