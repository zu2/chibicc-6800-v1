//
// check signbit
//
#include <math.h>
#include <float.h>

float gf1 = 1.0f;
float gf2 = -1.0f;

int main(int argc, char **argv)
{
  float lf1 = 1.0f;
  float lf2 = -1.0f;
  float p = 3.14f;
  float m = -p;

  if (signbit(0.0))
    return 1;

  if (signbit(1.0))
    return 2;

  if (!signbit(-1.0))
    return 3;

  if (signbit(0))
    return 4;

  if (signbit(1))
    return 5;

  if (!signbit(-1))
    return 6;

  if (signbit(gf1))
    return 11;

  if (!signbit(gf2))
    return 12;

  if (signbit(lf1))
    return 13;

  if (!signbit(lf2))
    return 14;

  if (signbit(gf1*lf1))
    return 21;

  if (!signbit(gf1*lf2))
    return 22;

  if (!signbit(gf2*lf1))
    return 23;

  if (signbit(gf2*lf2))
    return 24;

  if (signbit(p))
    return 101;

  if (!signbit(m))
    return 102;

  if (signbit(NAN))
    return 111;

  if (!signbit(-NAN))
    return 112;

  if (signbit(INFINITY))
    return 121;

  if (!signbit(-INFINITY))
    return 122;

  return 0;
}
