int le(int a, int b)
{
	return a<=b;
}
int ge(int a, int b)
{
	return a>=b;
}

int main(int argc, char **argv)
{
  int a=0;
  int b=1;
  int c=-1;

  if (le(1,0))
    return 1;
  if (le(0,-1))
    return 2;
  if (le(2,1))
    return 3;

  if (ge(0,1))
    return 11;
  if (ge(-1,0))
    return 12;
  if (ge(1,2))
    return 13;

  if (a<=-1)
    return 21;
  if (b<=0)
    return 22;
  if (c<-1)
    return 23;

  if (a>=1)
    return 31;
  if (b>=2)
    return 32;
  if (c>=0)
    return 33;

  return 0;
}
