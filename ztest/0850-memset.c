#include	"common.h"

unsigned char	buf[3000];

extern	char	*memset(char *s, int c, int n);

void setup_buf()
{
	int	i;

	for (i=0; i<3000; i++){
		buf[i] = 1;
	}
}

int	sum(int n)
{
	int	i;
	int	sum;

	for (sum=0,i=0; i<0x0101; i++){
		sum += buf[0];
	}

	return	sum;
}


int	main(int argc, char **argv)
{
	int	i;
	char	*p;

	buf[0] = 1;
	p = memset(buf,0,0);
	if (p!=buf)
		return 1;
	if (buf[0]==0)
		return 3;

	setup_buf();
	p = memset(buf,0,0xff);
	if (p!=buf)
		return 11;
	if (sum(0xff))
		return 12;
	if (buf[0xff]==0)
		return 13;

	setup_buf();
	p = memset(buf,0,0x0100);
	if (p!=buf)
		return 21;
	if (sum(0x100))
		return 22;
	if (buf[0x100]==0)
		return 23;

	setup_buf();
	p = memset(buf,0,0x0101);
	if (p!=buf)
		return 31;
	if (sum(0x101))
		return 32;
	if (buf[0x101]==0)
		return 33;


	return	0;
}

