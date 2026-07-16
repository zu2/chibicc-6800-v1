//
//	float to int/long conversion
//

#include "common.h"

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

int sccheck(int sc)
{
  float f;

  f = sc;
  if ((int)f != sc){
    print(sc);
    print((int)f);
    puthexf(f);
    putchar('\n');
    return 1;
  }
  return 0;
}

int uccheck(unsigned int uc)
{
  float f;

  f = uc;
  if ((unsigned int)f != uc){
    print(uc);
    print((unsigned int)f);
    puthexf(f);
    putchar('\n');
    return 1;
  }
  return 0;
}

int sicheck(int si)
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

int uicheck(unsigned int ui)
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

  for (si=-128; si<127; si++){
    if (sccheck(si))
      return 1;
  }
  for (si=-32767; si<32767; si++){
    if (sicheck(si))
      return 11;
  }
  if (sicheck(-32768))
    return 2;
  if (sicheck(32767))
    return 3;

  for (ui=0; ui<255; ui++){
    if (uccheck(ui))
      return 21;
  }
  for (ui=0; ui<65535; ui++){
    if (uicheck(ui))
      return 31;
  }
  if (uicheck(65535))
    return 13;

  return 0;
}
