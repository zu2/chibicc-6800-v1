#include <stdio.h>
#include <stdlib.h>

void __assert_fail(const char *expr, const char *file, int line, const char *func) {
    const char *s = func ? func : "";
    printf("%s:%d: %s%s%s: Assertion `%s' failed.\n",
        file, line, s, *s ? ": " : "", expr);
    asm("\tldab #134");
    abort();
}
