#include <float.h>
#include <math.h>

int main(int argc, char **argv)
{
  float f = 3.0;
  float g = 0.0;
  float h = NAN;

  if (3.0) { ; }else{ return 1; }
  if (!3.0)       return 2;
  if (!(char)3.0) return 3;
  if (!(long)3.0) return 4;
  if (!!3.0) { ; }else{ return 5; }
  if (!!!3.0)     return 6;

  if (0.0)       return 12;
  if ((char)0.0) return 13;
  if ((long)0.0) return 14;
  if (!0.0) { ; }else{ return 15; }
  if (!!0.0)     return 16;

  if (f) { ; }else{ return 21; }
  if (!f)       return 22;
  if (!(char)f) return 23;
  if (!(long)f) return 24;
  if (!!f) { ; }else{ return 25; }
  if (!!!f)     return 26;

  if (g)       return 42;
  if ((char)g) return 43;
  if ((long)g) return 44;
  if (!g) { ; }else{ return 45; }
  if (!!g)     return 46;

  if (h) { ; }else{ return 31; }
  if (!h)       return 32;
//if (!!(char)h) return 33;		// undefined behavior
//if (!!(long)h) return 34;		// undefined behavior
  if (!!h) { ; }else{ return 35; }
  if (!!!h)     return 36;

  return 0;
}

