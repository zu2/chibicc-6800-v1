#include <stdio.h>
#include <stdint.h>

#include "my_printf.c"

// Returns 32 for input 0 (0 input: undefined behavior)
int safe_clz(uint32_t x) {
    return x == 0 ? 32 : __builtin_clz(x);
}
int safe_ctz(uint32_t x) {
    return x == 0 ? 32 : __builtin_ctz(x);
}

// Test case struct: input value, expected clz, expected ctz
typedef struct {
    uint32_t value;
    int expected_clz;
    int expected_ctz;
} TestCase;

int main(void) {
    TestCase tests[] = {
        {0x00000000, 32, 32}, // all bits 0
        {0x00000001, 31,  0}, // only LSB is 1
        {0x80000000,  0, 31}, // only MSB is 1
        {0x00F0F000,  8, 12}, // middle bits
        {0xFFFFFFFF,  0,  0}, // all bits 1
        {0x0000FFFF, 16,  0}, // lower 16 bits 1
        {0xF0000000,  0, 28}, // upper 4 bits 1
        {0x01000000,  7, 24}, // bit 24 is 1
        {0x00010000, 15, 16}, // bit 16 is 1
    };
    int n = sizeof(tests) / sizeof(tests[0]);

    printf("   value        clz   ctz   exp_clz exp_ctz  clz_ok ctz_ok\n");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        uint32_t v = tests[i].value;
        int clz = safe_clz(v);
        int ctz = safe_ctz(v);
        int clz_ok = (clz == tests[i].expected_clz);
        int ctz_ok = (ctz == tests[i].expected_ctz);
        printf("0x%08lx   %2d    %2d      %2d      %2d     %s    %s\n",
            v, clz, ctz, tests[i].expected_clz, tests[i].expected_ctz,
            clz_ok ? "OK " : "NG ", ctz_ok ? "OK " : "NG ");
    }
    return 0;
}

