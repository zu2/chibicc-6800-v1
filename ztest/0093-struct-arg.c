#include "common.h"

struct complex {
    int r;
    int i;
};

#if 1
struct complex complex_add(struct complex com_1, struct complex com_2, struct complex *com_3){
    struct complex ret;

    ret.r = com_1.r + com_2.r;
    ret.i = com_1.i + com_2.i;

    return ret;
}

void complex_add2(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    (*com_3).r = (*com_1).r + (*com_2).r;
    (*com_3).i = (*com_1).i + (*com_2).i;
}

void complex_pluseq(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    com_1->r += com_2->r + com_3->r;
    com_1->i += com_2->i + com_3->i;
}

void complex_pluseq2(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    (*com_1).r += (*com_2).r + (*com_3).r;
    (*com_1).i += (*com_2).i + (*com_3).i;
}
#endif

struct complex sub(struct complex c)
{
#if	0
	putstr("sub: &c="); puthexi(&c);putchar('\n');
	print(c.r);
	print(c.i);
#endif
	c.r = c.r+1;
	c.i = c.i+1;
#if	0
	++c.r;
	++c.i;
#endif

	return c;
}

main(int argc,char **argv)
{
        struct complex com01, com02, com03;

        com01.r=2;
        com01.i=3;
#if	0
	putstr("main 1\n");
	print(com01.r);
	print(com01.i);
#endif
	com02 = sub(com01);

	putstr("main 2\n");
	print(com01.r);
	print(com01.i);
	print(com02.r);
	print(com02.i);
	return com02.r+com02.i != 7;

#if	1
        com02.r=4;
        com02.i=8;

        com03 = complex_add(com01,com02,&com03);

	if (com01.r + com02.r != com03.r)
		return 1;
	if (com01.i + com02.i != com03.i)
		return 2;
        complex_add2(&com01,&com02,&com03);
	if (com01.r + com02.r != com03.r)
		return 11;
	if (com01.i + com02.i != com03.i)
		return 12;

        complex_pluseq(&com01,&com02,&com03);
	if (com01.r != 12)
		return 21;
	if (com01.i != 22)
		return 22;

        complex_pluseq2(&com01,&com02,&com03);
	if (com01.r != 22)
		return 31;
	if (com01.i != 41)
		return 32;
#endif

	return 0;
}
