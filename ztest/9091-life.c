//
//	https://scrapbox.io/imasaraC/%E3%83%A9%E3%82%A4%E3%83%95%E3%82%B2%E3%83%BC%E3%83%A0
//
 

extern	void print(int i);
extern	void putc(int ch);

void putstr(unsigned char *s)
{
	while (*s)
		putc(*s++);
}

#define N  44  /* 縦方向 */
#define M  78  /* 横方向 */
unsigned char a[N + 2][M + 2], b[N + 2][M + 2];  /* 盤 */
 
int main(void)
{
	unsigned char i, j, g;
 
	putstr("\x1b[2J");
	a[N/2][M/2] = a[N/2-1][M/2] = a[N/2+1][M/2]
		= a[N/2][M/2-1] = a[N/2-1][M/2+1] = 1;  /* 初期状態 */
	for (g = 1; g <= 250; g++) {
		putstr("\x1b[0;0HGeneration ");print(g);putc('\n');  /* 世代 */
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= M; j++) {
				if (a[i][j]) {
					putc('*');
					b[i-1][j-1]++;  b[i-1][j]++;  b[i-1][j+1]++;
					b[i  ][j-1]++;                b[i  ][j+1]++;
					b[i+1][j-1]++;  b[i+1][j]++;  b[i+1][j+1]++;
				} else {
					putc('.');
				}
			}
			putc('\n');
		}
		for (i = 0; i <= N + 1; i++) {
			for (j = 0; j <= M + 1; j++) {
				if (b[i][j] != 2) a[i][j] = (b[i][j] == 3);
				b[i][j] = 0;
			}
		}
	}
	return 0;
}
