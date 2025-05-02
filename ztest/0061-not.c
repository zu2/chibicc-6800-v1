int main(int argc, char **argv)
{
	int	a=1;
	int	b=3;

	if (!1)
		return 1;

	if (~0xffff)
		return 2;

	if (!a)
		return 3;

	if (!(a&b))
		return 4;

	return 0;
}
