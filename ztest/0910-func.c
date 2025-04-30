#include "common.h"

extern	int	strcmp(char *p, char *q);

int main(int argc, char **argv)
{
  if (strcmp(__func__,"main"))
    return 1;

  if (strcmp(__FUNCTION__,"main"))
    return 1;

  putstr(__func__);putchar('\n');
  putstr(__FUNCTION__);putchar('\n');

  return 0;
}

