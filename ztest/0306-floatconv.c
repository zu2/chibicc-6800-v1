//
//	float to int/long conversion
//

#include "common.h"

extern	float	fsqrt(float x);

int
cmpfl(float f, unsigned long g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}

	return 0;
}

int scheck(int si)
{
  float f;

  f = si;
  if ((int)f != si){
    print(si);
    print((int)f);
    puthexf(f);
    putchar('\n');
    return 1;
  }
  return 0;
}

int ucheck(unsigned int ui)
{
  float f;

  f = ui;
  if ((unsigned int)f != ui){
    print(ui);
    print((unsigned int)f);
    puthexf(f);
    putchar('\n');
    return 1;
  }
  return 0;
}

int main(int argc, char **argv)
{
  signed   int si;
  unsigned int ui;

  for (si=-32767; si<32767; si++){
    if (scheck(si))
      return 1;
  }
  if (scheck(-32768))
    return 2;
  if (scheck(32767))
    return 3;

  for (ui=0; ui<65535; ui++){
    if (ucheck(ui))
      return 11;
  }
  if (ucheck(65535))
    return 13;

  return 0;
}
