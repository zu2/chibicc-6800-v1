#include "common.h"

struct complex {
    int r;
    int i;
};

struct complex com01, com02, com03;

void complex_add(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    com_3->r = com_1->r + com_2->r;
    com_3->i = com_1->i + com_2->i;
}

void complex_add2(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    (*com_3).r = (*com_1).r + (*com_2).r;
    (*com_3).i = (*com_1).i + (*com_2).i;
}

void complex_pluseq(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    com_3->r += com_1->r + com_2->r;
    com_3->i += com_1->i + com_2->i;
}

void complex_pluseq2(struct complex *com_1, struct complex *com_2, struct complex *com_3){
    (*com_3).r += (*com_1).r + (*com_2).r;
    (*com_3).i += (*com_1).i + (*com_2).i;
}

struct complex complex_add3(struct complex *com_1, struct complex *com_2){
    struct complex com_3;

    com_3.r = (*com_1).r + (*com_2).r;
    com_3.i = (*com_1).i + (*com_2).i;

    return com_3;
}


int
main(int argc, char **argv){

        com01.r=2;
        com01.i=3;
        com02.r=4;
        com02.i=8;
        com03.r=0;
        com03.i=0;

        complex_add(&com01,&com02,&com03); // com03 = com01 + com02

	if (com01.r + com02.r != com03.r)
		return 1;
	if (com01.i + com02.i != com03.i)
		return 2;

        complex_add2(&com01,&com02,&com03); // com03 = com01 + com02
	if (com01.r + com02.r != com03.r)
		return 11;
	if (com01.i + com02.i != com03.i)
		return 12;

        com03.r=0;
        com03.i=0;
        complex_pluseq(&com01,&com02,&com03); // com03 += com01 + com02
	if (com01.r + com02.r != com03.r)
		return 21;
	if (com01.i + com02.i != com03.i)
		return 22;

        com03.r=0;
        com03.i=0;
        complex_pluseq2(&com01,&com02,&com03);	// com03 += com01 + com02
	if (com01.r + com02.r != com03.r)
		return 31;
	if (com01.i + com02.i != com03.i)
		return 32;

        com03.r=0;
        com03.i=0;
        com03 = complex_add3(&com01,&com02);	// com03 = com01 + com02
	if (com01.r + com02.r != com03.r)
		return 41;
	if (com01.i + com02.i != com03.i)
		return 42;

	return 0;
}
