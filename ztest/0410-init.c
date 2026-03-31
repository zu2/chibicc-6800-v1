int strcmp(char *a, char *b)
{
  char c;

  while (*a) {
    if (c = (*a - *b)) {
      return c;
    }
    a++; b++;
  }
  return *a - *b;
}

int
main(int argc, char **argv) {
    int a[2][2] = {{1,2},{3,4}};
    const char *color[] =  { "Red", "Green", "Blue" };
    int *p = &a[0][0];
    char **q = color;

    if (*p++ != 1)
      return 1;
    if (*p++ != 2)
      return 2;
    if (*p++ != 3)
      return 3;
    if (*p++ != 4)
      return 4;

    if (strcmp(*q++,"Red"))
      return 11;
    if (strcmp(*q++,"Green"))
      return 12;
    if (strcmp(*q++,"Blue"))
      return 13;

    return 0;
}
