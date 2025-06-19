int main(int argc, char **argv)
{
	char a=1,b=2,c=3,d=4;
  char e;

  e = a;
  if (e!=1)
    return 1;

  e = a + b;
  if (e!=3)
    return 2;

  e = a + 1;
  if (e!=b)
    return 3;

  e = -a;
  if (e!=-1)
    return 11;

  e = a - b;
  if (e!=-1)
    return 12;

  e = e - 1;
  if (e!=-b)
    return 13;

	return (a-b)-(c-d);
}
