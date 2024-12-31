int le(int a, int b)
{
	return a<=b;
}
int ge(int a, int b)
{
	return a>=b;
}

int main(int argc, char **argv)
{
	if (le(1,0))
		return 1;
	if (ge(0,1))
		return 2;
	return 0;
}
