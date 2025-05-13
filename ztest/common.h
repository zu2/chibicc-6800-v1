//
//	common.h
//	test common routine
//

#if !defined(__CHIBICC_COMMON_H__)
#define __CHIBICC_COMMON_H__
#if defined(__clang__) || defined(__GNUC__)
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#define print(i)	printf("%d\n",i)
#define cpu_counter()	{ clock_t t; t=clock();printf("CPU = %.6f sec\n",(double)t/CLOCKS_PER_SEC); }
#define	fmodf(x,y)	fmod(x,y)
#else
#if defined(__CHIBICC_6800__)
// chibicc, fcc, cc68
extern	void print(int i);
extern	void putchar(int ch);
extern	void cpu_counter(void);
extern	void exit(int n);
#endif

#if defined(__clang__) || defined(__GNUC__)
#define putstr(s) fputs(s,stdout)
#else
void putstr(unsigned char *s);
#endif

void puthexc(unsigned int ch);
void puthexi(int v);
void puthexl(long v);
#if	defined(__CHIBICC__) || defined(__FCC__)
void puthexf(float f);
#endif
void assert(int expected, int actual, char *code);
#define ASSERT(x, y) assert(x, y, #y)

#endif
#endif
