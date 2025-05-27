#include "common.h"

struct complex {
  int r;
  int i;
};
struct complex com_01, com_02, com_03;

struct complex3 {
  struct complex *c1;
  struct complex *c2;
  struct complex *c3;
};

void complex_pluseq(struct complex3 *cp)
{
  cp->c3->r += cp->c1->r + cp->c2->r;
  cp->c3->i += cp->c1->i + cp->c2->i;
}

main()
{
  struct complex3 c;
  struct complex *cp;

  c.c1 = &com_01;
  c.c2 = &com_02;
  c.c3 = &com_03;

  c.c1->r = 2;
  print(c.c1->r);
;
  c.c1->i = 3;
  c.c2->r = 4;
  c.c2->i = 8;
  c.c3->r = 0;
  c.c3->i = 0;

  c.c3->r = 0;
  c.c3->i = 0;
  print(c.c2->r);
  print(c.c3->r);
  complex_pluseq(&c); // c.c3 += c.c1 + c.c2
  if (c.c1->r + c.c2->r != c.c3->r) {
    print(c.c1->r);
    print(c.c2->r);
    print(c.c3->r);
    return 21;
  }
  if (c.c1->i + c.c2->i != c.c3->i) {
    return 22;
  }

  return 0;
}
