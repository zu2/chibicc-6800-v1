#include "bm-graphics.h"

static uint8_t x2bw[] = {0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1};
static uint8_t x2bb[] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
static uint8_t *p = 0x2100;
static uint8_t *base = 0x2100;

void setgr(uint8_t mode)
{
  *((uint8_t *)0xefe0) = mode;
  base = 0x0900 + (mode & 0x0f) * 0x200;
}

#define set_pat(x) (x2bw[(x) & 7])
#define reset_pat(x) (x2bb[(x) & 7])

void plot(uint8_t x, uint8_t y, uint8_t z)
{

  if (y > 191) {
    return;
  }

  p = (y << 5) + (x >> 3) + base;

  if (z) {
    *p |= set_pat(x);
  } else {
    *p &= reset_pat(x);
  }
}

void cls()
{
#if 1
  if (base == 0x0900) {
    asm(" clrb");
    asm(" ldx #$0900");
    asm("CLS09:");
    asm(" stab 0,x");
    asm(" inx");
    asm(" cpx #$20ff");
    asm(" bne CLS09");
  } else if (base = 0x2100) {
    asm(" clrb");
    asm(" ldx #$2100");
    asm("CLS21:");
    asm(" stab 0,x");
    asm(" inx");
    asm(" cpx #$3900");
    asm(" bne CLS21");
  } else {
#endif
    static uint8_t *end;
    end = base + 0x1800;
    for (p = base; p < end; p++) {
      *p = 0;
    }
#if 1
  }
#endif
}

void vline(uint8_t x, uint8_t y1, uint8_t y2, uint8_t z)
{
  static uint8_t pat;

  if (y1 == y2) {
    plot(x, y1, z);
    return;
  }
  if (y1 > y2) {
    y1 ^= y2;
    y2 ^= y1;
    y1 ^= y2;
  }
  y2 = (y2 > 191) ? 192 : y2 + 1;
  p = (y1 << 5) + (x >> 3) + 0x2100;
  y2++;
  if (z) {
    pat = set_pat(x);
    for (uint8_t y = y1; y < y2; y++, p += 32) {
      *p |= pat;
    }
  } else {
    pat = reset_pat(x);
    for (uint8_t y = y1; y < y2; y++, p += 32) {
      *p &= pat;
    }
  }
}
