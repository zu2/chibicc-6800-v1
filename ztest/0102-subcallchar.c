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
  char x = 1;
  char y = 2;

	if(r=subc(1,2))
		return 1;
	if(r=subi(1,2))
		return 2;
	if(r=subc(x,y))
		return 3;
	if(r=subi(x,y))
		return 4;
	if(r=subc(y-x,x+x))
		return 5;
	if(r=subi(y-x,x+x))
		return 6;

	return 0;
}
