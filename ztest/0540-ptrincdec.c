unsigned char	a[100];

int main(int argc, char **argv)
{
	unsigned char *uc=a;
	signed   char *sc=a;
	int	i;

	for (i=0; i<100; i++)
	  a[i] = 0;
	int	i;

	for (i=0; i<100; i++)
	  a[i] = 0;

	if (uc[0]++)
		return 1;
	if (--uc[0])
		return 2;
	++uc[0];
	if (--uc[0])
		return 3;
	--uc[0];
	if (++uc[0])
		return 4;
	uc[0]++;
	if (--uc[0])
		return 5;
	uc[0]--;
	if (++uc[0])
		return 6;


	if (sc[1]++)
		return 101;
	if (--sc[1])
		return 102;
	++sc[1];
	if (--sc[1])
		return 103;
	--sc[1];
	if (++sc[1])
		return 104;
	sc[1]++;
	if (--sc[1])
		return 105;
	sc[1]--;
	if (++sc[1])
		return 106;


	return (sc[1]+=1)-1;
}
