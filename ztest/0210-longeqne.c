int eq(long a, long b)
{
	return a==b;
}
int ne(long a, long b)
{
	return a!=b;
}

unsigned long gul;
long gsl;

int main(int argc, char **argv)
{
  unsigned long lul;
  long lsl;

  if (gul>=0) {
    //
  }else{
    return 1;
  }
  if (gul<0) {
    return 2;
  }
  gsl = -1;
  if (gsl>=0) {
    return 11;
  }
  gsl = 1;
  if (gsl<0) {
    return 12;
  }

  if (lul>=0) {
    //
  }else{
    return 101;
  }
  if (lul<0) {
    return 102;
  }
  lsl = -1;
  if (lsl>=0) {
    return 111;
  }
  lsl = 1;
  if (lsl<0) {
    return 112;
  }

  if (eq(0L,1L)) {
    return 255;
  }
	return 0;
}
