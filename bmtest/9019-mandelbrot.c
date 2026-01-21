#include "bm-graphics.h"

extern void putchar(int ch);
extern void getchar(void);

int main(int argc, char **argv)
{
  int f, i, x, y;
  int c, d, a, b, q, s, t, p;

  setgr(0xcc);
  cls();

  f = 50;
  for (y = -96; y <= 95; y++) {
    for (x = -192; x <= 63; x++) {
      c = x * 60 / 100;
      //		c = x * 70 / 100;
      d = y * 52 / 100;
      a = c;
      b = d;
      for (i = 0; i < 24; i++) {
        q = b / 50;
        s = b - q * 50;
        t = (a * a - b * b) / f + c;
        b = 2 * (a * q + a * s / f) + d;
        a = t;
        p = a / 50;
        q = b / 50;
        if ((p * p + q * q) > 4) {
          break;
        }
      }
      if (i < 24) {
        plot(x + 192, y + 96, 1);
      }
      //			if (i>15)
      //				putchar(' ');
      //			else if (i<10)
      //				putchar(i+'0');
      //			else
      //				putchar(i-10+'A');
    }
    //		putchar('\r');
  }
  putchar('\007');
  (void)getchar();
  setgr(0x00);

  return 0;
}
