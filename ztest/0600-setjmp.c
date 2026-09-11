#include <stdlib.h>
#include <setjmp.h>

jmp_buf env;

void subroutine(int n) {
    if (n == 0) {
        longjmp(env, 123);
    }
    exit(1);
}

int no_frame(void)
{
  if (setjmp(env)) return 1;
  subroutine(0);
  return 2;
}

int with_local(void)
{
  int x;

  x = 42;
  if (setjmp(env)) return x;
  subroutine(0);
  return 0;
}

int main(int argc, char **argv) {
    int ret;

    if (no_frame() != 1) return 10;
    if (with_local() != 42) return 20;

    switch(setjmp(env)) {
    case 0:
      subroutine(0);
      return 1;
    case 123:
      return 0;
    default:
      return 2;
    }

    return 99;
}
