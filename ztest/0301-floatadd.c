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

#if	0
	f = 1.0;
       	g = 2.0;
	h = -1.0;
	e = -2.0;

	putstr("1.0:\t");puthexf(1.0);putc('\n');
	putstr("2.0:\t");puthexf(2.0);putc('\n');
	putstr("3.0:\t");puthexf(3.0);putc('\n');
	putstr("-1.0:\t");puthexf(-1.0);putc('\n');
	putstr("f:\t");puthexf(f);putc('\n');
	putstr("g:\t");puthexf(g);putc('\n');
	putstr("h:\t");puthexf(h);putc('\n');
	putstr("1.0+2.0:\t");puthexf(f+g);putc('\n');
	putstr("2.0+1.0:\t");puthexf(g+f);putc('\n');
	putstr("1.0+(-1.0):\t");puthexf(f+h);putc('\n');
	putstr("(-1.0)+1.0:\t");puthexf(h+f);putc('\n');
	putstr("-1.0:\t");puthexf(-1.0);putc('\n');
	putstr("1.0+(-2.0):\t");puthexf(f+e);putc('\n');
	putstr("2.0+(-1.0):\t");puthexf(g+h);putc('\n');
#endif
	f = 10.0;
	g = -11.0;
//	putstr("10.0-11.0:");puthexf(-1.0);putc('\n');
	putstr("10.0-11.0=");puthexf(f+g);putc('\n');
#if 0
	f = 11.0;
	g = -10.0;
	putstr("11.0-10.0:");puthexf(1.0);putc('\n');
	putstr("11.0-10.0=");puthexf(f+g);putc('\n');
#endif
#if	0
	f = 0.1;
       	g = 0.2;
	h = -0.1;
	e = -0.2;
	putstr("0.1:\t");puthexf(0.1);putc('\n');
	putstr("0.2:\t");puthexf(0.2);putc('\n');
	putstr("0.3:\t");puthexf(0.3);putc('\n');
	putstr("-0.1:\t");puthexf(-0.1);putc('\n');
	putstr("f:\t");puthexf(f);putc('\n');
	putstr("g:\t");puthexf(g);putc('\n');
	putstr("h:\t");puthexf(h);putc('\n');
	putstr("0.1+0.2:\t");puthexf(f+g);putc('\n');
	putstr("0.2+0.1:\t");puthexf(g+f);putc('\n');
	putstr("0.1+(-0.1):\t");puthexf(f+h);putc('\n');
	putstr("(-0.1)+0.1:\t");puthexf(h+f);putc('\n');
	putstr("-0.1:\t");puthexf(-0.1);putc('\n');
	putstr("0.1+(-0.2):\t");puthexf(f+e);putc('\n');
	putstr("0.2+(-0.1):\t");puthexf(g+h);putc('\n');
//	putstr("(unsigned long)1.0f:\t");puthexl((unsigned long)1.0);putc('\n');
#endif
}
