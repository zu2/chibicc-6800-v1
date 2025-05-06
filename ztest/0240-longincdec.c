int main(int argc, char **argv)
{
	unsigned long ul=0;
	signed   long sl=0;

	if (ul++)
		return 1;
	if (--ul)
		return 2;
	++ul;
	if (--ul)
		return 3;
	--ul;
	if (++ul)
		return 4;
	ul++;
	if (--ul)
		return 5;
	ul--;
	if (++ul)
		return 6;


	if (sl++)
		return 101;
	if (--sl)
		return 102;
	++sl;
	if (--sl)
		return 103;
	--sl;
	if (++sl)
		return 104;
	sl++;
	if (--sl)
		return 105;
	sl--;
	if (++sl)
		return 106;


	return (sl+=1)-1;
}
