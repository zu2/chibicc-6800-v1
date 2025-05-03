long le(long a, long b)
{
	return a<=b;
}
long ge(long a, long b)
{
	return a>=b;
}

int main(int argc, char **argv)
{
	return le(1,0) + ge(0,1);
}
