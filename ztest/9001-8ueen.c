/*
 *	8 queens
 */

extern	void putc(int ch);
extern	void print(int i);
extern	void cpu_counter(void);

unsigned char	x[8],col[8],up[15],down[15];
int	count=0;

void putstr(char *s)
{
	while (*s)
		putc(*s++);
}

void
generate(unsigned char n)
{
	char	k;
	char	h;
	char	i;

	for(h=0; h<8; h++){
		if(col[h] && up[n-h+7] && down[n+h]){
			x[n] = h;
			col[h] = 0;
			up[n-h+7] = 0;
			down[n+h] = 0;
			if(n==7){
				for(k=0; k<8; k++){
					putc(x[k]+'0');
				}
				putc('\n');
				count++;
			}else{
				generate(n+1);
			}
			down[n+h] = 1;
			up[n-h+7] = 1;
			col[h] = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	unsigned char	k;

	cpu_counter();
	for(k=0; k<8; k++){
		x[k]=-1;
		col[k]=1;
	}
	for(k=0; k<15; k++){
		up[k] = 1;
		down[k] = 1;
	}
	generate(0);
	cpu_counter();
	putstr("total=");print(count);putc('\n');

	return count!=92;
}
