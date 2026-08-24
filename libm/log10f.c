#include <math.h>

#define INV_LN10  0x1.bcb7b2p-2f

float log10f(float x)
{
  return logf(x) * INV_LN10;
}
