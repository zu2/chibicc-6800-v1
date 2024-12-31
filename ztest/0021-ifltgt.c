int lt(int a, int b)
{
	return a<b;
}
int gt(int a, int b)
{
	return a>b;
}

int main(int argc, char **argv)
{
	if (lt(1,0))
		return 1;
	if (gt(0,1))
		return 2;
	return 0;
}
