#include "bm-graphics.h"

extern void putchar(int ch);
extern void getchar(void);

int sub(int x, int y)
{
  return 0;
}

int
main(int argc, char **argv)
{
  int m[129],n[129];

  for (int j=0; j<=128; j++) {
    m[j] = 0;
    n[j] = 97;
  }

  setgr(0xcc);
  cls();

  for (int y=2; y<82; y++) {
    for (int x=1; x<40; x++) {
      int z = sub(x,y);
      int d = z+x+(y/2);
      int e = x-y+88;
      if (d>m[e] || d<n[e]) {
        if (d>m[e]) m[e] = d;
        if (d<n[e]) n[e] = d;
        if (d<96 && d>1) {
          plot(e,97-d,1);
        }
			}
    }
	}
  putchar('\007');
  (void)getchar();
  setgr(0x00);

	return 0;
}
