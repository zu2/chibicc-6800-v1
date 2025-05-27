//
//	common.h
//	test common routine
//

#if !defined(__CHIBICC_COMMON_H__)
#define __CHIBICC_COMMON_H__
#if defined(__clang__) || defined(__GNUC__)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#define print(i)	printf("%d\n",i)
#define cpu_counter()	{ clock_t t; t=clock();printf("CPU = %.6f sec\n",(double)t/CLOCKS_PER_SEC); }
#define	fmodf(x,y)	fmod(x,y)
#define	floorf(x)	floor(x)
#endif

// chibicc, fcc, cc68
#if defined(__CHIBICC_6800__) || defined(__FCC__)
extern	void print(int i);
extern	void putchar(int ch);
extern	void cpu_counter(void);
extern	void exit(int n);
#endif
void putstr(unsigned char *s);
void puthexc(unsigned int ch);
void puthexi(int v);
void puthexl(long v);
void puthexf(float f);
void assert(int expected, int actual, char *code);
#if defined(__clang__) || defined(__GNUC__)
#define putstr(s) fputs(s,stdout)
#endif
#define ASSERT(x, y) assert(x, y, #y)
#endif
