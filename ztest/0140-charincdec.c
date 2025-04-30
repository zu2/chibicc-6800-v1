int main(int argc, char **argv)
{
	unsigned char uc=0;
	signed   char sc=0;

	if (uc++)
		return 1;
	if (--uc)
		return 2;
	++uc;
	if (--uc)
		return 3;
	--uc;
	if (++uc)
		return 4;
	uc++;
	if (--uc)
		return 5;
	uc--;
	if (++uc)
		return 6;


	if (sc++)
		return 101;
	if (--sc)
		return 102;
	++sc;
	if (--sc)
		return 103;
	--sc;
	if (++sc)
		return 104;
	sc++;
	if (--sc)
		return 105;
	sc--;
	if (++sc)
		return 106;


	return (sc+=1)-1;
}
