int main(int argc, char **argv)
{
	int i=0;

	if (i++)
		return 1;
	if (--i)
		return 2;
	++i;
	if (--i)
		return 3;
	--i;
	if (++i)
		return 4;

	return (i+=1)-1;
}
