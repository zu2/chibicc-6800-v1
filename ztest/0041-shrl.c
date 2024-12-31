extern void print(int i);

int main(int argc, char **argv)
{
	int i=1;

	i <<= 8;
	if (i!=256)
		return 1;

	i >>= 8;
	if (i!=1)
		return 2;

	return 0;
}
