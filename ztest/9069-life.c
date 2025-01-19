//
//	Life Game
//
//

extern	void print(int i);
extern	void putchar(int ch);

void putstr(unsigned char *s)
{
	while (*s)
		putchar(*s++);
}


unsigned char a1[66][64];
unsigned char a2[66][64];

unsigned long rnd;

unsigned long xorshift32()
{
	rnd ^= rnd << 13;
	rnd ^= rnd >> 17;
	rnd ^= rnd << 5;

	return rnd;
}

void
display()
{
	unsigned char x,y;

	putstr("\x1b[0;0H");
	for (y=1; y<=48; y++) {
		for (x=1; x<=64; x++){
			putchar(a1[x][y]?'*':' ');
		}
		putchar('\n');
	}
	putchar('\n');
}

void
init()
{
	unsigned char x,y;
	rnd = 99990;
	while (rnd==0) {
		rnd += *((int *)0x0b)+110;
	}

	for (y=0; y<64; y++){ 
		for (x=0; x<66; x++){
			a1[x][y] = 0;
			a2[x][y] = 0;
		}
	}
	for (y=1; y<=48; y++){ 
		for (x=1; x<=64; x++){
			if (xorshift32()&1){
				a1[x][y] = 1;
			}
		}
	}
	putstr("\x1b[2J");
}

int
main(int argc, char **argv)
{
	unsigned char x,y;
	unsigned char alive;
	unsigned char x1,y1,x2,y2;
	int i;

	init();
	display();
	for(i=0;i<999;i++){
		for (y=1; y<=48; y++) {
			y1 = y-1;
			y2 = y+1;
			for (x=1; x<=64; x++){
				x1 = x-1;
				x2 = x+1;
				alive  = a1[x1][y1] + a1[x][y1] + a1[x2][y1];
				alive += a1[x1][y]              + a1[x2][y];
				alive += a1[x1][y2] + a1[x][y2] + a1[x2][y2];
				if (alive == 3){
					a2[x][y] = 1;
				}else if (alive != 2){
					a2[x][y] = 0;
				}
			}
		}
		for (y=1; y<=48; y++) {
			for (x=1; x<=64; x++){
				a1[x][y] = a2[x][y];
			}
		}
		display();
	}
	return 0;
}
