int sub(int a)
{
	return a%8-2;
}

int main(int argc, char **argv)
{
  signed int i;
  unsigned int n;

  n = 32;
  n /= 4;
  if (n != 8)
    return 29;
  n %= 12;
  if (n != 8)
    return 30;
  i = -32;
  i /= 4;
  if (i != -8)
    return 31;
  i %= 10;
  if (i != -8)
    return 32;

  return 0;
}
