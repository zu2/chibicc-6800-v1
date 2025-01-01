void putc(int ch);

void putstr(char *s)
{
	while (*s)
		putc(*s++);
}

int isxdigit(ch)
{
	return	(ch>='0' && ch<='9')
	||	(ch>='A' && ch<='F')
	||	(ch>='a' && ch<='f');
}

int main(int argc, char **argv)
{
	char *p = "Hello, World\n";

	while (*p){
		if (isxdigit(*p))
			putc(*p);
		p++;
	}
	putc('\n');

	return isxdigit('@');
}
