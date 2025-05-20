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

int scheck(long sl)
{
  float f;

  f = sl;
  if ((signed long)f != sl){
    putstr("sl (sl)f f\n");
    puthexl(sl);putchar(' ');puthexl((signed long)f);putchar(' ');
    puthexf(f); putchar('\n');
    return 1;
  }
  return 0;
}

int scheck2(long sl, long sl2)
{
  float f;

  f = sl;
  if ((signed long)f != sl2){
    putstr("f=(float)sl2\n");
    putstr("sl sl2 (sl)f f\n");
    puthexl(sl);putchar(' '); puthexl(sl2);putchar(' ');
    puthexl((signed long)f);putchar(' ');
    puthexf(f); putchar('\n');
    return 1;
  }
  return 0;
}

int ucheck(unsigned long ul)
{
  float f;

  f = ul;
  if ((unsigned long)f != ul){
    putstr("ul (ul)f f\n");
    puthexl(ul);putchar(' ');puthexl((unsigned long)f);putchar(' ');
    puthexf(f); putchar('\n');
    return 1;
  }
  return 0;
}

int ucheck2(unsigned long ul, unsigned long ul2)
{
  float f;

  f = ul;
  if ((unsigned long)f != ul2){
    putstr("ul ul2 (ul)f f\n");
    puthexl(ul);putchar(' '); puthexl(ul2);putchar(' ');
    puthexl((unsigned long)f);putchar(' ');
    puthexf(f); putchar('\n');
    return 1;
  }
  return 0;
}

int main(int argc, char **argv)
{
  signed   long sl;
  unsigned long ul;

  // 23bit
  if (scheck2(0x007FFFFF,0x007FFFFF))
    return 1;
  if (scheck2(-0x007FFFFF,-0x007FFFFF))
    return 2;


  // 24bit
  if (scheck2(0x00800000,0x00800000))
    return 11;
  if (scheck2(0x00800001,0x00800001))
    return 12;
  if (scheck2(0x00FFFFFF,0x00FFFFFF))
    return 13;
  if (scheck2(-0x00800000,-0x00800000))
    return 16;
  if (scheck2(-0x00800001,-0x00800001))
    return 17;
  if (scheck2(-0x00FFFFFF,-0x00FFFFFF))
    return 18;

  // 25bit
  if (scheck2(0x01000000,0x01000000))
    return 21;
  if (scheck2(0x01000001,0x01000000))
    return 22;
  if (scheck2(0x01000002,0x01000002))
    return 23;
  if (scheck2(0x01000003,0x01000004))
    return 24;
  if (scheck2(0x01000004,0x01000004))
    return 25;
  if (scheck2(0x01000005,0x01000004))
    return 26;

  if (scheck2(-0x01000000,-0x01000000))
    return 21;
  if (scheck2(-0x01000001,-0x01000000))
    return 22;
  if (scheck2(-0x01000002,-0x01000002))
    return 23;
  if (scheck2(-0x01000003,-0x01000004))
    return 24;
  if (scheck2(-0x01000004,-0x01000004))
    return 25;
  if (scheck2(-0x01000005,-0x01000004))
    return 26;

  // 26bit
  if (scheck2(0x01FFFFFE,0x01FFFFFE))
    return 31;
  if (scheck2(0x01FFFFFF,0x02000000))
    return 32;
  if (scheck2(0x02000000,0x02000000))
    return 33;
  if (scheck2(0x02000001,0x02000000))
    return 34;
  if (scheck2(0x02000002,0x02000000))
    return 35;
  if (scheck2(0x02000003,0x02000004))
    return 36;

  if (scheck2(-0x01FFFFFE,-0x01FFFFFE))
    return 41;
  if (scheck2(-0x01FFFFFF,-0x02000000))
    return 42;
  if (scheck2(-0x02000000,-0x02000000))
    return 43;
  if (scheck2(-0x02000001,-0x02000000))
    return 44;
  if (scheck2(-0x02000002,-0x02000000))
    return 45;
  if (scheck2(-0x02000003,-0x02000004))
    return 46;

  // 27bit
  if (scheck2(0x03FFFFF8,0x03FFFFF8))
    return 51;
  if (scheck2(0x03FFFFFB,0x03FFFFFC))
    return 52;
  if (scheck2(0x03FFFFFC,0x03FFFFFC))
    return 53;
  if (scheck2(0x03FFFFFF,0x04000000))
    return 54;
  if (scheck2(0x04000000,0x04000000))
    return 55;
  if (scheck2(0x04000001,0x04000000))
    return 56;
  if (scheck2(0x04000003,0x04000000))
    return 57;
  if (scheck2(0x04000004,0x04000000))
    return 58;
  if (scheck2(0x04000009,0x04000008))
    return 59;

  if (scheck2(-0x03FFFFF8,-0x03FFFFF8))
    return 61;
  if (scheck2(-0x03FFFFFB,-0x03FFFFFC))
    return 62;
  if (scheck2(-0x03FFFFFC,-0x03FFFFFC))
    return 63;
  if (scheck2(-0x03FFFFFF,-0x04000000))
    return 64;
  if (scheck2(-0x04000000,-0x04000000))
    return 65;
  if (scheck2(-0x04000001,-0x04000000))
    return 66;
  if (scheck2(-0x04000003,-0x04000000))
    return 67;
  if (scheck2(-0x04000004,-0x04000000))
    return 68;
  if (scheck2(-0x04000009,-0x04000008))
    return 69;

  // 31bit
  if (scheck2(0x7FFFFFBF,0x7FFFFF80))
    return 71;
  if (scheck2(0x7FFFFFC0,0x80000000))
    return 72;
  if (scheck2(0x7FFFFFFE,0x80000000))
    return 73;
  if (scheck2(0x7FFFFFFF,0x80000000))
    return 74;

  if (scheck2(0x80000200,0x80000200))
    return 81;
  if (scheck2(0x80000180,0x80000180))
    return 82;
  if (scheck2(0x80000101,0x80000100))
    return 83;
  if (scheck2(0x80000100,0x80000100))
    return 84;
  if (scheck2(0x80000080,0x80000080))
    return 85;
  if (scheck2(0x8000007F,0x80000080))
    return 86;
  if (scheck2(0x80000002,0x80000000))
    return 87;
  if (scheck2(0x80000001,0x80000000))
    return 88;
  if (scheck2(0x80000000,0x80000000))
    return 89;

  if (ucheck2(0x007FFFFF,0x007FFFFF))
    return 101;
  if (ucheck2(0x00FFFFFF,0x00FFFFFF))
    return 102;
  if (ucheck2(0x01000000,0x01000000))
    return 103;

  if (ucheck2(0x01000001,0x01000000))
    return 111;
  if (ucheck2(0x01000003,0x01000004))
    return 112;

  if (ucheck2(0x7FFFFF80,0x7FFFFF80))
    return 121;
  if (ucheck2(0x7FFFFFFF,0x80000000))
    return 122;

  if (ucheck2(0x80000000,0x80000000))
    return 131;
  if (ucheck2(0x80000001,0x80000000))
    return 132;

  if (ucheck2(0xFFFFFF00,0xFFFFFF00))
    return 141;
  if (ucheck2(0xFFFFFFFF,0x00000000)) // 4294967296.0 -> 0
    return 142;

    
  return 0;

  // The following takes a very long time
  for (sl=-8388607; sl<8388607; sl++){
    if (scheck(sl))
      return 231;
  }
  if (scheck(-8388608))
    return 232;
  if (scheck(8388607))
    return 233;

  for (ul=0; ul<16777215; ul++){
    if (ucheck(ul))
      return 241;
  }
  if (ucheck(16777215))
    return 243;

  return 0;
}
