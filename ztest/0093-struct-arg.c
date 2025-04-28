#include "common.h"

struct complex {
    int r;
    int i;
};

struct large_complex {
    long r;
    long i;
    char dummy[100];
};

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

struct complex sub(struct complex c)
{
	++c.r;
	++c.i;

	return c;
}

struct large_complex large_complex_add(struct large_complex com_1, struct large_complex com_2){
    struct large_complex ret;

    ret.r = com_1.r + com_2.r;
    ret.i = com_1.i + com_2.i;

    return ret;
}

main(int argc,char **argv)
{
        struct complex com01, com02, com03;
        struct large_complex com11, com12, com13;

        com01.r=2;
        com01.i=3;
	com02 = sub(com01);

	return com02.r+com02.i != 7;

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

	com11.r=2;
	com11.i=3;
	com12.r=4;
	com12.i=8;
	com13 = large_complex_add(com11,com12);
	if (com11.r + com12.r != com13.r)
		return 101;
	if (com11.i + com12.i != com13.i)
		return 102;

	return 0;
}
