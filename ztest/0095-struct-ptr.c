#include "common.h"

struct complex {
    int r;
    int i;
};
struct complex com_01,com_02,com_03;

struct complex3 {
    struct complex *c1;
    struct complex *c2;
    struct complex *c3;
};

void complex_add(struct complex3 *cp)
{
    cp->c3->r = cp->c1->r + cp->c2->r;
    cp->c3->i = cp->c1->i + cp->c2->i;
}

void complex_add2(struct complex3 *cp)
{
    (*cp).c3->r = (*cp).c1->r + (*cp).c2->r;
    (*cp).c3->i = (*cp).c1->i + (*cp).c2->i;
}

void complex_pluseq(struct complex3 *cp)
{
    cp->c3->r += cp->c1->r + cp->c2->r;
    cp->c3->i += cp->c1->i + cp->c2->i;
}

void complex_pluseq2(struct complex3 *cp)
{
    (*cp).c3->r += (*cp).c1->r + (*cp).c2->r;
    (*cp).c3->i += (*cp).c1->i + (*cp).c2->i;
}

struct complex *complex_add3(struct complex3 *cp)
{
    static struct complex com_3;

    com_3.r = (*cp).c1->r + (*cp).c2->r;
    com_3.i = (*cp).c1->i + (*cp).c2->i;

    return &com_3;
}


main(){
        struct complex3 c;
        struct complex  *cp;

	c.c1 = &com_01;
	c.c2 = &com_02;
	c.c3 = &com_03;

        c.c1->r=2;
        c.c1->i=3;
        c.c2->r=4;
        c.c2->i=8;
        c.c3->r=0;
        c.c3->i=0;

        complex_add(&c);		// c.c3 = c.c1 + c.c2

	if (c.c1->r + c.c2->r != c.c3->r)
		return 1;
	if (c.c1->i + c.c2->i != c.c3->i)
		return 2;

        complex_add2(&c);		 // c.c3 = c.c1 + c.c2
	if (c.c1->r + c.c2->r != c.c3->r)
		return 11;
	if (c.c1->i + c.c2->i != c.c3->i)
		return 12;

        c.c3->r=0;
        c.c3->i=0;
        complex_pluseq(&c);		 // c.c3 += c.c1 + c.c2
	if (c.c1->r + c.c2->r != c.c3->r)
		return 21;
	if (c.c1->i + c.c2->i != c.c3->i)
		return 22;

        c.c3->r=0;
        c.c3->i=0;
        complex_pluseq2(&c);		// c.c3 += c.c1 + c.c2
	if (c.c1->r + c.c2->r != c.c3->r)
		return 31;
	if (c.c1->i + c.c2->i != c.c3->i)
		return 32;

        c.c3->r=0;
        c.c3->i=0;
        cp = complex_add3(&c);		// cp = c.c1 + c.c2
	if (c.c1->r + c.c2->r != cp->r)
		return 41;
	if (c.c1->i + c.c2->i != cp->i)
		return 42;

	return 0;
}
