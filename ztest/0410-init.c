int strcmp(char *a, char *b)
{
  char c;

  while (*a && (*a == *b)) {
    a++; b++;
  }
  return *(unsigned char *)a - *(unsigned char *)b;
}

int
main(int argc, char **argv) {
    int a[][2] = {{1,2},{3,4}};
    char b[9] = { 1,2,3,4,5,6,7,8, };

    const char *color[] =  { "Red", "Green", "Blue" };
    int *p = &a[0][0];
    char *r = b;
    char **q = color;

    if (*p++ != 1)
      return 1;
    if (*p++ != 2)
      return 2;
    if (*p++ != 3)
      return 3;
    if (*p++ != 4)
      return 4;

    if (r[0] != 1)
      return 11;
    if (r[8] != 0)
      return 19;

    if (strcmp(*q++,"Red"))
      return 21;
    if (strcmp(*q++,"Green"))
      return 22;
    if (strcmp(*q++,"Blue"))
      return 23;

    return 0;
}
