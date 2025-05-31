int x;

int func()
{
  return ++x;
}

int main(int argc, char **argv)
{
  int a[10];
  int i;
  int y;

  for (i=0; i<10; i++) {
    a[i] = i;
  }

  a[9] += 1;
  if (a[9] != 10)
    return 1;

  x = 7;
  a[func()] += 1;
  if (a[8] != 9)
    return 2;
  if (a[9] != 10)
    return 3;

  x = 0;
  a[++x] += 1;
  if (a[1] != 2)
    return 3;
  if (a[2] != 2)
    return 4;

  x = 2;
  y = 5;
  a[++x] += y;
  if (a[3] != 8)
    return 5;
  if (a[4] != 4)
    return 6;

  return 0;
}
