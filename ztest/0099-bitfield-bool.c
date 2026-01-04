#include <stdio.h>

struct S {
    _Bool a : 1;
    _Bool b : 1;
    _Bool c : 1;
};

int main(void) {
    struct S s = {0, 0, 0};

    s.a = 1;
    s.b = 0;
    s.c = 1;

    if (s.a != 1) return 1;
    if (s.b != 0) return 2;
    if (s.c != 1) return 3;

    s.a = 0;
    s.b = 1;
    s.c = 0;

    if (s.a != 0) return 4;
    if (s.b != 1) return 5;
    if (s.c != 0) return 6;

    return 0;
}

