void putc(int ch);

void putstr(char *s)
{
	while (*s)
		putc(*s++);
}

int main(int argc, char **argv)
{
	putstr("Hello, World\n");

	return 0;
}
