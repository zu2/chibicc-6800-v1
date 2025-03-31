char subc(char x,char y)
{
	char	z=3;

	if (x!=1)
		return 1;
	if (y!=2)
		return 2;
	if (z!=3)
		return 3;

	return 0;
}
int subi(int x,int y)
{
	int	z=3;

	if (x!=1)
		return 11;
	if (y!=2)
		return 12;
	if (z!=3)
		return 13;

	return 0;

}

int main(int argc, char **argv)
{
	int r;

	if(r=subc(1,2))
		return r;
	if(r=subi(1,2))
		return r;

	return 0;
}
