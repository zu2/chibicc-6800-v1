//
//	alloca
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

int sub()
{
	unsigned char	i;
	unsigned char	sum = 0;
	unsigned char	*p = alloca(10);

	for (i=0; i<10; i++){
		p[i] = i;
	}
	for (i=0; i<10; i++){
		sum += p[i];
	}
	return sum;
}

int main(int argc, char **argv)
{
	return sub(10)!=45;
}
