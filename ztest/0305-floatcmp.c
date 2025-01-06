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

	f = 1.0;
       	g = 2.0;
	h = 0.0;

	// EQ
	putstr("0.0==0.0:\t");print(0.0==0.0);
	putstr("0.0==0.0=\t");print(h==h);
	putstr("1.0==2.0:\t");print(1.0==2.0);
	putstr("1.0==2.0=\t");print(f==g);
	putstr("2.0==1.0:\t");print(2.0==1.0);
	putstr("2.0==1.0:\t");print(g==f);
	putc('\n');
	// NE
	putstr("0.0!=0.0:\t");print(0.0!=0.0);
	putstr("0.0!=0.0=\t");print(h!=h);
	putstr("1.0!=2.0:\t");print(1.0!=2.0);
	putstr("1.0!=2.0=\t");print(f!=g);
	putstr("2.0!=1.0:\t");print(2.0!=1.0);
	putstr("2.0!=1.0=\t");print(g!=f);
	putc('\n');
	// LT
	putstr("0.0<0.0:\t");print(0.0<0.0);
	putstr("0.0<0.0=\t");print(h<h);
	putstr("1.0<2.0:\t");print(1.0<2.0);
	putstr("1.0<2.0=\t");print(f<g);
	putstr("2.0<1.0:\t");print(2.0<1.0);
	putstr("2.0<1.0=\t");print(g<f);
	putc('\n');
	// GT
	putstr("0.0>0.0:\t");print(0.0>0.0);
	putstr("0.0>0.0=\t");print(h>h);
	putstr("1.0>2.0:\t");print(1.0>2.0);
	putstr("1.0>2.0=\t");print(f>g);
	putstr("2.0>1.0:\t");print(2.0>1.0);
	putstr("2.0>1.0=\t");print(g>f);
	putc('\n');
	// LE
	putstr("0.0<=0.0:\t");print(0.0<=0.0);
	putstr("0.0<=0.0=\t");print(h<=h);
	putstr("1.0<=2.0:\t");print(1.0<=2.0);
	putstr("1.0<=2.0=\t");print(f<=g);
	putstr("2.0<=1.0:\t");print(2.0<=1.0);
	putstr("2.0<=1.0=\t");print(g<=f);
	putc('\n');
	// GE
	putstr("0.0>=0.0:\t");print(0.0>=0.0);
	putstr("0.0>=0.0=\t");print(h>=h);
	putstr("1.0>=2.0:\t");print(1.0>=2.0);
	putstr("1.0>=2.0=\t");print(f>=g);
	putstr("2.0>=1.0:\t");print(2.0>=1.0);
	putstr("2.0>=1.0=\t");print(g>=f);
	putc('\n');
	return 0;
}
