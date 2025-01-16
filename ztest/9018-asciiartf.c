/*
cpm c asciiart.c -LF
*/

#include "common.h"

char	junk[256];

main()
{
	int i, x, y;
	float a, b, ca, cb, t;
	float x1,y1;

	cpu_counter();
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
			putchar("0123456789ABCDEF "[i]);
		}
		putchar('\n');
	}
	cpu_counter();
	return 0;
}
