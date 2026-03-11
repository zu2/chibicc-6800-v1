int main(int argc, char **argv)
{
  for (long ii=-32768; ii<=32767; ii++) {
    int i = ii;
    int j = i - 10 + 'A';

    if (j != i+0x37)
      return 99;
  }

	return (1-2)-(3-4);
}
