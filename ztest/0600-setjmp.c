#include <stdlib.h>
#include <setjmp.h>

jmp_buf env;

void subroutine(int n) {
    if (n == 0) {
        longjmp(env, 123);
    }
    exit(1);
}

int main(int argc, char **argv) {
    int ret;

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
