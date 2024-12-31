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
	if (eq(0,1))
		return 1;

	if (ne(0,0))
		return 2;

	return 0;
}
