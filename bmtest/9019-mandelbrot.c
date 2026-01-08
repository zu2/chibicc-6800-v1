extern void putchar(int ch);
extern void getchar(void);

static unsigned char x2bw[] = { 0x80,0x40,0x20,0x10,8,4,2,1 };
static unsigned char x2bb[] = { 0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe };

void setgr(unsigned char mode)
{
  *((char *)0xefe0) = mode;
}

void plot(unsigned char x, unsigned char y, unsigned char z)
{
#if 0
  static unsigned char *p;

  p = (y<<5) + (x>>3) + 0x2100;
  x = 7 - (x&7);

  if (z) {
    *p |= ((unsigned char)1<<x);
  }else{
    *p &= ~((unsigned char)1<<x);
  }
#else
  static unsigned char *p;

  if (y>191)
    return;

  p = (y<<5) + (x>>3) + 0x2100;
  x &= 7;

  if (z) {
    *p |= x2bw[x];
  }else{
    *p &= x2bb[x];
  }
#endif
}

void cls()
{
#if 1
  asm(" clrb");
  asm(" ldx #$2100");
  asm("CLS01:");
  asm(" stab 0,x");
  asm(" inx");
  asm(" cpx #$3900");
  asm(" bne CLS01");
#else
  static char *p;

  for ( p=0x2100; p<0x3900; p++) {
    *p = 0;
  }
#endif
}

main()
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
			for (i = 0; i <24; i++) {
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
      if (i<24) {
        plot(x+192,y+96,1);
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
