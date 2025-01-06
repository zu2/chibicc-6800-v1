/*
cpm c asciiart.c -LF
*/

extern	void	putc(int ch);
extern	void	print(int i);
extern	void	cpu_counter(void);

void putstr(unsigned char *s)
{
	while (*s)
		putc(*s++);
}


char	junk[256];

main()
{
	int i, x, y;
	float a, b, ca, cb, t;
	float x1,y1;

//	printf("hit Enter key:");
//	gets(junk);

	for (y = -12; y <= 12; y++) {
		for (x = -39; x <= 39; x++) {
			ca = x * 0.0458;
			cb = y * 0.08333;
			a = ca;
			b = cb;
			for (i = 0; i <=15; i++) {
				t = a * a - b * b + ca;
				b = 2.0 * a * b + cb;
				a = t;
				if ((a * a + b * b) > 4.0) {
					break;
				}
			}
			putc("0123456789ABCDEF "[i]);
		}
		putc('\n');
	}
	putstr("OK\n");
	cpu_counter();
//	putstr("hit Enter key:\n");
//	gets(junk);
}
