extern	void print(int i);
extern	void putchar(int ch);

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

//#define EPS (1.0e-10)
#define EPS (0.0000000000001)
#define f(x) (x*x-2.0)
#define df(x) (2.0*x)
#define f2(x) (mulf(x,x)-2.0)
#define df2(x) (mulf(2.0,x))

extern	void	cpu_counter();
extern	float	fabsf(float x);

int main(int argc, char **argv)
{
    float xn, xo, x1; // x_new x_old

    cpu_counter();
    xn = (xo = 10.0) + 1.0;
    if (xo!=10.0)
        return 1;
    if (xn!=11.0)
        return 2;
    if (fabsf(xn - xo)!=1.0)
	return 3;
    while(fabsf(xn - xo) > EPS){
        xo = xn;
        xn = -f(xo) / df(xo) + xo;
    }
    cpu_counter();
    putstr("√2=");puthexf(xn);putchar('\n');
    putstr("√2*√2=");puthexf(xn*xn);putchar('\n');
    putstr("2.0:");puthexf(2.0);putchar('\n');
    putchar('\n');

    return fabsf((xn*xn)-2.0)>0.000001;
}

