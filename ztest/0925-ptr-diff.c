int sub(int *a, int *b)
{
    return *a - *b;
}

int main(int argc, char **argv)
{
  int a = 10;
  int b = 20;

  if (sub(&a,&b)==0)
    return 1;

  if (sub(&a,&b)>=0)
    return 2;

  return 0;
}
