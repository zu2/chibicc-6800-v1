#include "common.h"

int main(int argc, char **argv)
{
  unsigned int  ui;
  unsigned long ul;

  ul =  ui = 0x12345678;

  if (ui!=0x5678)
    return 1;
  if (ul!=0x5678)
    return 2;
  
  return 0;
}
