//
//
//

extern	void putc(int ch);
extern	void print(int i);
extern	void cpu_counter(void);

#define N	(8)
#define NN	(8*8)
#define NP	(8*8-2)

unsigned char board[16][16];
char dx[8] = { 2, 1,-1,-2,-2,-1, 1, 2 };
char dy[8] = {  1, 2, 2, 1,-1,-2,-2,-1 };
int count = 0;
int solution = 0;

void exit(int r)
{
	*(char *)0xfeff = r;
}

void putstr(char *s)
{
	while (*s)
		putc(*s++);
}

void
printboard() 
{
    unsigned char i,j;
    putstr("\nSolution ");print(++solution);putc('\n');
    for (i = 2; i <= N + 1; i++) {
        for (j = 2; j <= N + 1; j++) {
			if(board[i][j]>=10){
				putc(board[i][j]/10+'0');
			}else{
				putc(' ');
			}
			putc(board[i][j]%10+'0');
			putc(' ');
		}
		putc('\n');
    }
	putstr("count=");
	if(count>10){
		putc(count/10+'0');
	}
	putc(count%10+'0');
	putc('\n');
}


#define	check(cx,cy,x1,y1,x2,y2)	(x==cx+2 && y==cy+2 && board[x1+2][y1+2]==0 && board[x2+2][y2+2]==0)

void
test(unsigned char x, unsigned char y)
{
    unsigned char i;
    unsigned char nx,ny;

    if (board[x][y])	return;
    board[x][y] = ++count;
    if (count == NN){
		cpu_counter();
		printboard();
		exit(0);
	}else{
//		if(count>NP){
//			printboard();
//		}
		if(check(2,1,0,0,1,2)){
			test(0+2,0+2);
		}else if(check(1,2,0,0,2,1)){
			test(0+2,0+2);
		}else if(check(5,1,7,0,6,2)){
			test(7+2,0+2);
		}else if(check(6,2,7,0,5,1)){
			test(7+2,0+2);
		}else if(check(1,5,0,7,2,6)){
			test(0+2,7+2);
		}else if(check(2,6,0,7,1,5)){
			test(0+2,7+2);
		}else if(check(5,6,7,7,6,5)){
			test(7+2,7+2);
		}else if(check(6,5,7,7,5,6)){
			test(7+2,7+2);
		}else{	
			for (i = 0; i < 8; i++){
				nx = x+dx[i];
				if(nx<2 || nx>9)	continue;
				ny = y+dy[i];
				if(ny<2 || ny>9)	continue;
				if (board[nx][ny]==0){
					test(nx,ny);
				}
			}
		}
	}
    board[x][y] = 0;
	count--;
}

void
knight(unsigned char x, unsigned char y)
{
    unsigned char i,j;

    for (i = 0; i <= N + 3; i++){
        for (j = 0; j <= N + 3; j++){
	    board[i][j] = 1;
	}
    }
    for (i = 2; i <= N + 1; i++){
        for (j = 2; j <= N + 1; j++){
	    board[i][j] = 0;
	}
    }
    if (board[x][y])
	exit(22);

    test(x,y);
}

int main(int argc, char *argv[])
{
	knight(2,2);
	cpu_counter();
}
