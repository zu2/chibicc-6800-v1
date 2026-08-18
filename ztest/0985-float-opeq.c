int x;

float gv;
float ga[4];
struct S { float m; } gs;
struct S *gsp;
float *gp;

double gd;
double gda[4];


float long2float(unsigned long x)
{
  return *((float *)&x);
}

int cmpfl(float f, unsigned long g)
{
  int i;
  unsigned char *p = (unsigned char *)&f;
  unsigned char *q = (unsigned char *)&g;

  for (i=0; i<4; ++i,++p,++q){
    if (*p != *q){
      return i+1;
    }
  }
  return 0;
}

int func()
{
  return ++x;
}

int main(int argc, char **argv)
{
  float lv;
  float la[4];
  static float sv;
  int i;

  lv = 8.0f;  lv += 2.0f;  if (lv != 10.0f) return 10;
  lv = 8.0f;  lv -= 2.0f;  if (lv !=  6.0f) return 11;
  lv = 8.0f;  lv *= 2.0f;  if (lv != 16.0f) return 12;
  lv = 8.0f;  lv /= 2.0f;  if (lv !=  4.0f) return 13;

  gv = 8.0f;  gv += 2.0f;  if (gv != 10.0f) return 20;
  gv = 8.0f;  gv -= 2.0f;  if (gv !=  6.0f) return 21;
  gv = 8.0f;  gv *= 2.0f;  if (gv != 16.0f) return 22;
  gv = 8.0f;  gv /= 2.0f;  if (gv !=  4.0f) return 23;

  la[1] = 8.0f;  la[1] += 2.0f;  if (la[1] != 10.0f) return 30;
  la[1] = 8.0f;  la[1] -= 2.0f;  if (la[1] !=  6.0f) return 31;
  la[1] = 8.0f;  la[1] *= 2.0f;  if (la[1] != 16.0f) return 32;
  la[1] = 8.0f;  la[1] /= 2.0f;  if (la[1] !=  4.0f) return 33;

  ga[1] = 8.0f;  ga[1] += 2.0f;  if (ga[1] != 10.0f) return 40;
  ga[1] = 8.0f;  ga[1] -= 2.0f;  if (ga[1] !=  6.0f) return 41;
  ga[1] = 8.0f;  ga[1] *= 2.0f;  if (ga[1] != 16.0f) return 42;
  ga[1] = 8.0f;  ga[1] /= 2.0f;  if (ga[1] !=  4.0f) return 43;

  sv = 8.0f;  sv += 2.0f;  if (sv != 10.0f) return 50;
  sv = 8.0f;  sv -= 2.0f;  if (sv !=  6.0f) return 51;
  sv = 8.0f;  sv *= 2.0f;  if (sv != 16.0f) return 52;
  sv = 8.0f;  sv /= 2.0f;  if (sv !=  4.0f) return 53;

  i = 2;
  la[i] = 8.0f;  la[i] += 2.0f;  if (la[2] != 10.0f) return 60;
  la[i] = 8.0f;  la[i] -= 2.0f;  if (la[2] !=  6.0f) return 61;
  la[i] = 8.0f;  la[i] *= 2.0f;  if (la[2] != 16.0f) return 62;
  la[i] = 8.0f;  la[i] /= 2.0f;  if (la[2] !=  4.0f) return 63;

  ga[i] = 8.0f;  ga[i] += 2.0f;  if (ga[2] != 10.0f) return 70;
  ga[i] = 8.0f;  ga[i] -= 2.0f;  if (ga[2] !=  6.0f) return 71;
  ga[i] = 8.0f;  ga[i] *= 2.0f;  if (ga[2] != 16.0f) return 72;
  ga[i] = 8.0f;  ga[i] /= 2.0f;  if (ga[2] !=  4.0f) return 73;

  gp = &gv;
  *gp = 8.0f;  *gp += 2.0f;  if (gv != 10.0f) return 80;
  *gp = 8.0f;  *gp -= 2.0f;  if (gv !=  6.0f) return 81;
  *gp = 8.0f;  *gp *= 2.0f;  if (gv != 16.0f) return 82;
  *gp = 8.0f;  *gp /= 2.0f;  if (gv !=  4.0f) return 83;

  gs.m = 8.0f;  gs.m += 2.0f;  if (gs.m != 10.0f) return 90;
  gs.m = 8.0f;  gs.m -= 2.0f;  if (gs.m !=  6.0f) return 91;
  gs.m = 8.0f;  gs.m *= 2.0f;  if (gs.m != 16.0f) return 92;
  gs.m = 8.0f;  gs.m /= 2.0f;  if (gs.m !=  4.0f) return 93;

  gsp = &gs;
  gsp->m = 8.0f;  gsp->m += 2.0f;  if (gs.m != 10.0f) return 100;
  gsp->m = 8.0f;  gsp->m -= 2.0f;  if (gs.m !=  6.0f) return 101;
  gsp->m = 8.0f;  gsp->m *= 2.0f;  if (gs.m != 16.0f) return 102;
  gsp->m = 8.0f;  gsp->m /= 2.0f;  if (gs.m !=  4.0f) return 103;

  gd = 8.0;  gd += 2.0;  if (gd != 10.0) return 110;
  gd = 8.0;  gd -= 2.0;  if (gd !=  6.0) return 111;
  gd = 8.0;  gd *= 2.0;  if (gd != 16.0) return 112;
  gd = 8.0;  gd /= 2.0;  if (gd !=  4.0) return 113;

  gda[i] = 8.0;  gda[i] += 2.0;  if (gda[2] != 10.0) return 120;
  gda[i] = 8.0;  gda[i] -= 2.0;  if (gda[2] !=  6.0) return 121;
  gda[i] = 8.0;  gda[i] *= 2.0;  if (gda[2] != 16.0) return 122;
  gda[i] = 8.0;  gda[i] /= 2.0;  if (gda[2] !=  4.0) return 123;

  i = 2;
  ga[i] = 8.0f;  ga[i] += i;  if (ga[2] != 10.0f) return 130;
  ga[i] = 8.0f;  ga[i] -= i;  if (ga[2] !=  6.0f) return 131;
  ga[i] = 8.0f;  ga[i] *= i;  if (ga[2] != 16.0f) return 132;
  ga[i] = 8.0f;  ga[i] /= i;  if (ga[2] !=  4.0f) return 133;

  for (i=0; i<4; i++) {
    ga[i] = i;
  }
  x = 1;
  ga[func()] += 1.0f;
  if (ga[2] != 3.0f) return 140;
  if (ga[3] != 3.0f) return 141;
  if (x != 2) return 142;

  x = 0;
  ga[++x] -= 1.0f;
  if (ga[1] != 0.0f) return 150;
  if (ga[2] != 3.0f) return 151;

  i = 2;
  ga[i] = long2float(0xFF800000);  ga[i] += 1.0f;
  if (cmpfl(ga[2], 0xFF800000)) return 160;
  ga[i] = long2float(0x7F800000);  ga[i] += 1.0f;
  if (cmpfl(ga[2], 0x7F800000)) return 161;
  ga[i] = long2float(0xFF800000);  ga[i] -= 1.0f;
  if (cmpfl(ga[2], 0xFF800000)) return 162;
  ga[i] = long2float(0x7F800000);  ga[i] *= 2.0f;
  if (cmpfl(ga[2], 0x7F800000)) return 163;

  ga[i] = 1.0f;   ga[i] /= 0.0f;
  if (cmpfl(ga[2], 0x7F800000)) return 170;
  ga[i] = -1.0f;  ga[i] /= 0.0f;
  if (cmpfl(ga[2], 0xFF800000)) return 171;
  ga[i] = long2float(0x7F7FFFFF);  ga[i] *= 10.0f;
  if (cmpfl(ga[2], 0x7F800000)) return 172;

  ga[i] = 0.0f;  ga[i] /= 0.0f;
  if (ga[2] == ga[2]) return 180;
  ga[i] = long2float(0x7F800000);  ga[i] -= long2float(0x7F800000);
  if (ga[2] == ga[2]) return 181;
  ga[i] = long2float(0x7FC00000);  ga[i] += 1.0f;
  if (ga[2] == ga[2]) return 182;
  ga[i] = long2float(0x7F800000);  ga[i] *= 0.0f;
  if (ga[2] == ga[2]) return 183;

  ga[i] = long2float(0x80000000);  ga[i] -= 0.0f;
  if (cmpfl(ga[2], 0x80000000)) return 190;
  ga[i] = 0.0f;  ga[i] *= -1.0f;
  if (cmpfl(ga[2], 0x80000000)) return 191;

  return 0;
}
