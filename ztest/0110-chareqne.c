int seq(signed char a, signed char b)
{
	return a==b;
}
int sne(signed char a, signed char b)
{
	return a!=b;
}
int ueq(unsigned char a, unsigned char b)
{
	return a==b;
}
int une(unsigned char a, unsigned char b)
{
	return a!=b;
}

int main(int argc, char **argv)
{
	signed   char sa=0;
	signed   char sb=1;
	signed   char sc=-1;
	unsigned char ua=0;
	unsigned char ub=1;
	unsigned char uc=-1;

	if(!seq(0,0))
	  return 1;
	if(seq(0,1))
	  return 2;
	if(!sne(0,1))
	  return 3;
	if(sne(1,1))
	  return 4;

	if(!ueq(0,0))
	  return 11;
	if(ueq(0,1))
	  return 12;
	if(!une(0,1))
	  return 13;
	if(une(1,1))
	  return 14;


	if (!(sa==0))
	  return 20;
	if (sa!=0)
	  return 21;
	if (sb==0)
	  return 22;
	if (!(sb!=0))
	  return 23;

	if (!(ua==0))
	  return 30;
	if (ua!=0)
	  return 31;
	if (ub==0)
	  return 32;
	if (!(ub!=0))
	  return 33;

	return 0;
}
