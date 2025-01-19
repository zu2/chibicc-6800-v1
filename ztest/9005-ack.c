/*
 *	ackerman
 */

extern	void cpu_counter(void);

unsigned char
ack_c(unsigned char x, unsigned char y)
{
	if(x==0){
		return y+1;
	}else if(y==0){
		return ack_c(x-1,1);
	}else{
		return ack_c(x-1,ack_c(x,y-1));
	}
}

int
ack_i(int x, int y)
{
	if(x==0){
		return y+1;
	}else if(y==0){
		return ack_i(x-1,1);
	}else{
		return ack_i(x-1,ack_i(x,y-1));
	}
}

int main(int argc, char *argv[])
{
	if(ack_c(3,4)!=125)
		return 1;
	if(ack_c(3,5)!=253)
		return 2;
	if(ack_i(3,6)!=509)
		return 3;
	if(ack_i(3,7)!=1021)
		return 4;
	if(ack_i(3,8)!=2045)
		return 5;
	if(ack_i(4,0)!=13)
		return 10;

	cpu_counter();

	return 0;
}
