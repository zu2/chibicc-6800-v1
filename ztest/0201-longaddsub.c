long	al,bl,cl,dl;

int main(int argc, char **argv)
{
	long a,b,c,d;

	if ( (1L-2L)-(3L-4L) )
		return 1;

	a = 1;
	b = 2;
	c = 3;
	d = 4;

	if ( (a-b)-(c-d) )
	  return 2;

	a = 200001;
	b = 200000;
	c = 300001;
	d = 300000;

	if ( (a-b)-(c-d) )
	  return 3;

	al = 1;
	bl = 2;
	cl = 3;
	dl = 4;

	if ( (al-bl)-(cl-dl) )
	  return 12;

	al = 200001;
	bl = 200000;
	cl = 300001;
	dl = 300000;

	if ( (a-b)-(c-d) )
	  return 13;

	return 0;
}
