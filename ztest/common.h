//
//	common.h
//	test common routine
//

extern	void print(int i);
extern	void putchar(int ch);
extern	void cpu_counter(void);

void putstr(unsigned char *s)
{
	while (*s)
		putchar(*s++);
}

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
