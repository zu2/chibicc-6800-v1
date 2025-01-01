void putc(int ch);

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

int main(int argc, char **argv)
{
	char p[100];
	p[0] = 0;

	strcpy(p,"Hello, World\n");
	putstr(p);

	return strcmp(p, "Hello, World\n");
}