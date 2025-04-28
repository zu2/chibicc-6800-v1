int eq(int a, int b)
{
	return a==b;
}
int ne(int a, int b)
{
	return a!=b;
}

int main(int argc, char **argv)
{
	int a=0;
	int b=1;

	if(!eq(0,0))
	  return 1;

	if(eq(0,1))
	  return 2;

	if(!ne(0,1))
	  return 3;

	if(ne(1,1))
	  return 4;


	if (!(a==0))
	  return 10;
	if (a!=0)
	  return 11;
	if (b==0)
	  return 12;
	if (!(b!=0))
	  return 13;

	return 0;
}
