//extern void print(int i);
//extern void putc(int c);

int main(int argc, char **argv)
{
	int	i;

	for (i=0; i<1000; i=i+1){
		if(i==50)
			break;
	}

	return (i!=50);
}
