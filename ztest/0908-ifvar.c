int func1(char var)
{
	if (var==0)
	  return 1;

	return 0;
}

int func2(char var)
{
	if (!var)
	  return 1;

	return 0;
}

int main(int argc, char **argv)
{
	if (func1(1))
	  return 1;

	if (func1(0)!=1)
	  return 2;

	if (func2(1))
	  return 21;

	if (func2(0)!=1)
	  return 22;

	return 0;
}
