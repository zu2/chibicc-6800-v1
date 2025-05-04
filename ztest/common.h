//
//	common.h
//	test common routine
//

#if defined(__clang__) || defined(__GNUC__)
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#define print(i)	printf("%d\n",i)
#define cpu_counter()	{ clock_t t; t=clock();printf("CPU = %.6f sec\n",(double)t/CLOCKS_PER_SEC); }
#else
// chibicc, fcc, cc68
extern	void print(int i);
extern	void putchar(int ch);
extern	void cpu_counter(void);
#endif

#if defined(__clang__) || defined(__GNUC__)
#define putstr(s) fputs(s,stdout)
#else
void putstr(unsigned char *s)
{
	while (*s)
		putchar(*s++);
}
#endif

void
puthexc(unsigned int ch)
{
	int	v1=(ch>>4)&0x0f;
	int	v2=(ch&0x0f);

	putchar((v1<10)?(v1+'0'):(v1+'A'-10));
	putchar((v2<10)?(v2+'0'):(v2+'A'-10));
}
void
puthexi(int v)
{
	unsigned char *p = (unsigned char *)&v;

	puthexc(*p);
	puthexc(*(p+1));
}
void
puthexl(long v)
{
	unsigned char *p = (unsigned char *)&v;

	puthexc(*p);
	puthexc(*(p+1));
	puthexc(*(p+2));
	puthexc(*(p+3));
}
void
puthexf(float f)
{
	unsigned char *p = (unsigned char *)&f;

	puthexc(*p);
	puthexc(*(p+1));
	puthexc(*(p+2));
	puthexc(*(p+3));
}
