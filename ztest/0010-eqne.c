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
	if(!eq(0,0))
	  return 1;

	if(eq(0,1))
	  return 2;

	if(!ne(0,1))
	  return 3;

	if(ne(1,1))
	  return 4;

	return 0;
}
