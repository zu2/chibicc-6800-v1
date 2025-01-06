//
//	float basic library
//

extern	void print(int i);
extern	void putc(int ch);

void putstr(unsigned char *s)
{
	while (*s)
		putc(*s++);
}

void
puthexc(unsigned int ch)
{
	int	v1=(ch>>4)&0x0f;
	int	v2=(ch&0x0f);

	putc((v1<10)?(v1+'0'):(v1+'A'-10));
	putc((v2<10)?(v2+'0'):(v2+'A'-10));
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

int
cmpf(float f, float g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	puthexf(f);putc(' ');
	puthexf(g);putc('\n');
#if 0
	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
#endif
	return 0;
}

int main(int argc, char **argv)
{
	float	f,g,h,e;
	float	xo,xn;

#if 0
	f = 0.1;
	g = 0.2;
	h = 0.3;
	e = 0.02;
	putstr("0.1+0.2+0.3:");puthexf(0.6);putc('\n');
	putstr("0.1+0.2+0.3=");puthexf(f+g+h);putc('\n');
	putstr("0.1*0.2:");puthexf(0.02);putc('\n');
	putstr("0.1*0.2=");puthexf(f*g);putc('\n');
	putstr("0.02*0.3:");puthexf(0.006);putc('\n');
	putstr("0.02*0.3=");puthexf(e*h);putc('\n');
	putstr("0.1*0.2*0.3:");puthexf(0.006);putc('\n');
	putstr("0.1*0.2*0.3=");puthexf(f*g*h);putc('\n');
#endif

	xn = (xo = 10.0) + 1.0;

	if (xo!=10.0)
		return 1;
	if (xn!=11.0)
		return 2;

	return 0;
}
