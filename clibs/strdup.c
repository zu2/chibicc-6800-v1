#include <stddef.h>
#include <string.h>
#include <malloc.h>

char *strdup(const char *s)
{
  static size_t n;
  static char  *p;

  n = strlen(s) + 1;

  if ((p=malloc(n)))
    memcpy(p, s, n);

  return p;
}
