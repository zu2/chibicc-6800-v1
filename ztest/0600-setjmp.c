#include "common.h"
#include <setjmp.h>

jmp_buf env;

void subroutine(int n) {
    if (n == 0) {
        longjmp(env, 123);
    }
    putstr("longjmp error\n");
}

int main(int argc, char **argv) {
    int ret;

    ret = setjmp(env);

    if (ret == 0) {
        subroutine(0);
        return 1;
    } else {
        return (ret == 123) ? 0 : 2;
    }
    return 99;
}
