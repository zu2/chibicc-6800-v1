int sle(signed char a, signed char b)
{
	return a<=b;
}
int sge(signed char a, signed char b)
{
	return a>=b;
}

int ule(unsigned char a, unsigned char b)
{
	return a<=b;
}
int uge(unsigned char a, unsigned char b)
{
	return a>=b;
}

int main(int argc, char **argv)
{
  int a=0;
  int b=1;
  int c=-1;

  if (sle(1,0))
    return 1;
  if (sle(0,-1))
    return 2;
  if (sle(2,1))
    return 3;
  if (sle(127,-128))
    return 2;
  if (sle(0,-128))
    return 3;

  if (ule(1,0))
    return 6;
  if (!ule(0,1))
    return 7;
  if (ule(2,1))
    return 8;
  if (!ule(0,255))
    return 7;
  if (ule(255,1))
    return 8;

  if (sge(0,1))
    return 11;
  if (sge(-1,0))
    return 12;
  if (sge(1,2))
    return 13;
  if (sge(-128,127))
    return 12;
  if (sge(-128,0))
    return 13;

  if (uge(0,1))
    return 16;
  if (!uge(1,0))
    return 17;
  if (!uge(1,1))
    return 18;
  if (uge(0,255))
    return 19;
  if (!uge(255,0))
    return 20;

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
