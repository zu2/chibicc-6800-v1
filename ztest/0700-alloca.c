int sub(x)
{
	unsigned char	i;
	unsigned char	sum = 0;
	unsigned char	*p = alloca(x);

	for (i=0; i<10; i++){
		p[i] = i;
	}
	for (i=0; i<10; i++){
		sum += p[i];
	}
	return sum;
}

int main(int argc, char **argv)
{
	return sub(10)!=45;
}
