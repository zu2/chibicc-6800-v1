void putc(int ch);
extern void print(int i);

void putstr(char *s)
{
	while (*s)
		putc(*s++);
}

char *strcpy(char *p, char *q)
{
	while (*q){
		*p++ = *q++;
	}
}
int strcmp(char *p, char *q)
{
	while (*p && *q && *p==*q){
		p++;
		q++;
	}
	return *p - *q;
}

char *itoa(int n, char *buffer, int radix)
{
	unsigned int	sign;
	unsigned int	m;
	unsigned char	*p;

	if (radix!=10)
		return	0;	// sorry

	p = buffer + 8;
	*--p = '\0';
	if (n>=0){
		sign = 0;
		m = n;
	}else{
		sign = 1;
		m = -n;
	}
	do {
		*--p = (m%10) + '0';
		m = m / 10;
	}while( m>0 );

	if (sign)
		*--p = '-';

	return p;
}


int main(int argc, char **argv)
{
	char *p;
	char q[100];

	p = itoa(-32501,q,10);
	putstr(p); putc('\n');

	return strcmp(p,"-32501");
}
