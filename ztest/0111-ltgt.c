int slt(signed char a, signed char b)
{
	return a<b;
}
int sgt(signed char a, signed char b)
{
	return a>b;
}

int ult(unsigned char a, unsigned char b)
{
	return a<b;
}
int ugt(unsigned char a, unsigned char b)
{
	return a>b;
}

int main(int argc, char **argv)
{
  int a=0;
  int b=1;
  int c=-1;

  if (slt(0,0))
    return 1;
  if (slt(1,0))
    return 2;
  if (!slt(0,1))
    return 3;
  if (ult(0,0))
    return 6;
  if (ult(1,0))
    return 7;
  if (!ult(0,1))
    return 8;

  if (sgt(0,0))
    return 11;
  if (sgt(0,1))
    return 12;
  if (!sgt(1,0))
    return 13;
  if (ugt(0,0))
    return 16;
  if (ugt(0,1))
    return 17;
  if (!ugt(1,0))
    return 18;

  if (b<a)
    return 21;
  if (b<c)
    return 21;

  if (c>a)
    return 31;
  if (c>b)
    return 32;

  if (b<0)
    return 41;
  if (b<1)
    return 42;
  if (c<-1)
    return 43;

  if (c>0)
    return 51;
  if (c>1)
    return 52;
  if (c>-1)
    return 53;

	return 0;
}
