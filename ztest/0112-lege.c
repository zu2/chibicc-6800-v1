long lt(long a, long b)
{
	return a<=b;
}
long gt(long a, long b)
{
	return a>=b;
}

int main(int argc, char **argv)
{
	return lt(1,0) + gt(0,1);
}
