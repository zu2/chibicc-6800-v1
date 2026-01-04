extern void putchar(int ch);

main()
{
	int f, i, x, y;
	int c, d, a, b, q, s, t, p;

	f = 50;
	for (y = -12; y <= 12; y++) {
		for (x = -39; x <= 39; x++) {
			c = x * 229 / 100;
			d = y * 416 / 100;
			a = c;
			b = d;
			for (i = 0; i <=15; i++) {
				q = b / f;
				s = b - q * f;
				t = (a * a - b * b) / f + c;
				b = 2 * (a * q + a * s / f) + d;
				a = t;
				p = a / f;
				q = b / f;
				if ((p * p + q * q) > 4) {
					break;
				}
			}
			if (i>15)
				putchar(' ');
			else if (i<10)
				putchar(i+'0');
			else
				putchar(i-10+'A');
		}
		putchar('\r');
	}

	return 0;
}
