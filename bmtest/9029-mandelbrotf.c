extern void putchar(int ch);
extern void getchar(void);

void setgr(unsigned char mode)
{
  *((char *)0xefe0) = mode;
}

void plot(unsigned char x, unsigned char y, unsigned char z)
{
  static unsigned char *p;

  p = (y<<5) + (x>>3) + 0x2100;
  x = 7 - (x&7);

  if (z) {
    *p |= (1<<x);
  }else{
    *p &= ~(1<<x);
  }
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
	int i, x, y;
	float a, b, ca, cb, t;
	float x1,y1;

  setgr(0xcc);
  cls();
	for (y = -96; y <= 95; y++) {
		for (x = -192; x <= 63; x++) {
			ca = x * 0.0143125;
			cb = y * 0.0185026;
//			ca = x * 0.0458;
//			cb = y * 0.08333;
			a = ca;
			b = cb;
			for (i = 0; i <24; i++) {
				t = a * a - b * b + ca;
				b = 2.0 * a * b + cb;
				a = t;
				if ((a * a + b * b) > 4.0) {
					break;
				}
			}
      if (i<24) {
        plot(x+192,y+96,1);
      }
		}
	}
  putchar('\007');
  (void)getchar();
  setgr(0x00);
	return 0;
}
