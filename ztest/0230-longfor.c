extern void print(int i);
extern void putc(int c);

int main(int argc, char **argv)
{
	long	i;
	unsigned long	j;
	unsigned long	k = 100000;

	for (i=0; i<100000; i++){
		if(i==50000)
			break;
	}
	if (i!=50000)
		return 1;

	for (j=0; j<k; j++){
		if(j==50000)
			break;
	}
	if (j!=50000)
		return 1;

	return 0;
}
