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
  int a=0;
  int b=1;
  int c=-1;

  if (lt(0,0))
    return 1;
  if (lt(1,0))
    return 2;
  if (!lt(0,1))
    return 3;

  if (gt(0,0))
    return 11;
  if (gt(0,1))
    return 12;
  if (!gt(1,0))
    return 13;

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
