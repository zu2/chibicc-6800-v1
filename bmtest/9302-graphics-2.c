#include "bm-graphics.h"

extern void putchar(int ch);
extern void getchar(void);
extern int abs(int x);


int sub(int x, int y)
{
  int s = x-20;
  s *= s;
  int t = abs(40-y)-20;
  t *= t;
  int z = (250-s-t)/6;
  if (z<0)  z=0;
  if (y>40) z=-z;

  return z;
}

int
main(int argc, char **argv)
{
  int m[256],n[256];

  for (int j=0; j<256; j++) {
    m[j] = 0;
    n[j] = 192;
  }

  setgr(0xcc);
  cls();

  for (int y=2; y<192; y++) {
    for (int x=1; x<64; x++) {
      int z = sub(x,y);
      int d = z+x+(y/2);
      int e = x-y+192;
      if (e>=0 && e<256) {
        if (d>m[e] || d<n[e]) {
          if (d>m[e]) m[e] = d;
          if (d<n[e]) n[e] = d;
          if (d<192 && d>0) {
            plot(e,191-d,1);
          }
        }
			}
    }
	}
  putchar('\007');
  (void)getchar();
  setgr(0x00);

	return 0;
}
