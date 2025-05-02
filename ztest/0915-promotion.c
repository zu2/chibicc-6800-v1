int main(int argc, char **argv)
{
  unsigned char a = 1;

  if (-a != -1)
    return 1;

  if (~a != -2)
    return 2;

  return 0;
}

