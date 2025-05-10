#include <stddef.h>
#include <stdlib.h>

extern void _memswap(void *a, void *b, size_t size);

static char *random_pivot(char *first, char *last, size_t size)
{
  size_t len = (last - first) / size + 1;
  size_t idx = rand() % len;
  return first + idx * size;
}

static void qsort3(char *first, char *last, size_t size,
                   int (*compar)(const void *, const void *))
{
  while (last - first >= size) {
    if (last - first == size) {
      if (compar(first, last) > 0) {
        _memswap(first, last, size);
      }
      return;
    }
    if (last - first == 2 * size) {
      char *b = first + size;
      char *c = b + size;
      if (compar(first, b) > 0) {
        _memswap(first, b, size);
      }
      if (compar(b, c) > 0) {
        _memswap(b, c, size);
      }
      if (compar(first, b) > 0) {
        _memswap(first, b, size);
      }
      return;
    }

    char *pivot = random_pivot(first, last, size);
    if (pivot != first) {
      _memswap(first, pivot, size);
      pivot = first;
    }

    char *lt = first;
    char *gt = last;
    char *i = first + size;
    while (i <= gt) {
      int cmp = compar(i, pivot);
      if (cmp < 0) {
        lt += size;
        _memswap(i, lt, size);
        i += size;
      } else if (cmp > 0) {
        _memswap(i, gt, size);
        gt -= size;
      } else {
        i += size;
      }
    }
    if (lt != first) {
      _memswap(first, lt, size);
    }

    if (lt - size >= first) {
      qsort3(first, lt - size, size, compar);
    }

    if (gt + size < last) {
      first = gt + size;
    } else {
      break;
    }
  }
}

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *))
{
  if (nmemb < 2) {
    return;
  }
  char *first = (char *)base;
  char *last = first + (nmemb - 1) * size;
  qsort3(first, last, size, compar);
}
