#include <stdio.h>
#include "my_printf.c"

long div(long a, long b)
{
    return a / b;
}

typedef struct {
    long dividend;
    long divisor;
    long expected;
} test_case_t;

static const test_case_t test_data[] = {
    {0L, 1L, 0L},
    {0L, (-1L), 0L},
    {1L, 1L, 1L},
    {(-1L), 1L, (-1L)},
    {1L, (-1L), (-1L)},
    {(-1L), (-1L), 1L},
    {10L, 2L, 5L},
    {10L, (-2L), (-5L)},
    {(-10L), 2L, (-5L)},
    {(-10L), (-2L), 5L},
    {100L, 1L, 100L},
    {100L, (-1L), (-100L)},
    {(-100L), 1L, (-100L)},
    {(-100L), (-1L), 100L},
    {0x00007FFFL, 1L, 32767L},
    {0x00007FFFL, (-1L), (-32767L)},
    {(-32768L), 1L, (-32768L)},
    {(-32768L), (-1L), 32768L},
    {0x000003E8L, 10L, 100L},
    {0x000003E8L, (-10L), (-100L)},
    {(-1000L), 10L, (-100L)},
    {(-1000L), (-10L), 100L},
    {255L, 5L, 51L},
    {255L, (-5L), (-51L)},
    {128L, 8L, 16L},
    {127L, 3L, 42L},
    {0x00010000L, 0x00000100L, 256L},
    {0x00010000L, (-256L), (-256L)},
    {(-65536L), 0x00000100L, (-256L)},
    {(-65536L), (-256L), 256L},
    {0x00008000L, 128L, 256L},
    {0x00008000L, (-128L), (-256L)},
    {0x0000FFFFL, 255L, 257L},
    {0x0000FFFEL, 2L, 32767L},
    {0x000186A0L, 0x000003E8L, 100L},
    {0x000186A0L, (-1000L), (-100L)},
    {0x01000000L, 0x00001000L, 4096L},
    {0x01000000L, (-4096L), (-4096L)},
    {(-16777216L), 0x00001000L, (-4096L)},
    {0x000F4240L, 0x000003E8L, 1000L},
    {0x000F4240L, (-1000L), (-1000L)},
    {0x00FF0000L, 255L, 65536L},
    {0x00FFFFFFL, 0x0000FFFFL, 256L},
    {0x7FFFFFFFL, 1L, 0x7FFFFFFFL},
    {0x7FFFFFFFL, (-1L), (-2147483647L)},
    {0x7FFFFFFFL, 2L, 0x3FFFFFFFL},
    {0x7FFFFFFFL, 0x000003E8L, 0x0020C49BL},
    {0x7FFFFFFFL, 0x00010000L, 32767L},
    {0x7FFEA810L, 0x0000B504L, 46340L},
    {(-2147395600L), 0x0000B504L, (-46340L)},
    {0x7FFEA810L, (-46340L), (-46340L)},
    {(-2147395600L), (-46340L), 46340L},
    {(-0x7FFFFFFF-1), 1L, (-0x7FFFFFFF-1)},
    {(-0x7FFFFFFF-1), 2L, (-0x40000000L)},
    {(-0x7FFFFFFF-1), (-2L), 0x40000000L},
    {100L, 10L, 10L},
    {0x00000400L, 32L, 32L},
    {0x00010000L, 0x00000100L, 256L},
    {0x000F4240L, 0x000003E8L, 1000L},
    {0x01000000L, 0x00001000L, 4096L},
    {100L, 3L, 33L},
    {100L, 7L, 14L},
    {0x000003E8L, 17L, 58L},
    {0x00007FFFL, 100L, 327L},
    {0x0000FFFFL, 0x00000101L, 255L},
    {0x000F4240L, 7L, 142857L},
    {0x7FFFFFFFL, 0x000F4240L, 2147L},
    {1L, 10L, 0L},
    {1L, 100L, 0L},
    {10L, 100L, 0L},
    {100L, 0x000003E8L, 0L},
    {0x000003E8L, 0x000F4240L, 0L},
    {255L, 0x00000100L, 0L},
    {0x0000FFFFL, 0x00010000L, 0L},
    {0x00000100L, 2L, 128L},
    {0x00000100L, 4L, 64L},
    {0x00000100L, 8L, 32L},
    {0x00000100L, 16L, 16L},
    {0x00000400L, 2L, 512L},
    {0x00010000L, 0x00000100L, 256L},
    {0x01000000L, 0x00010000L, 256L},
    {(-100L), 3L, (-33L)},
    {100L, (-3L), (-33L)},
    {(-100L), (-3L), 33L},
    {(-1000L), 17L, (-58L)},
    {0x000003E8L, (-17L), (-58L)},
    {(-32768L), 100L, (-327L)},
    {0x00008000L, (-100L), (-327L)},
    {0x7FFFFFFEL, 2L, 0x3FFFFFFFL},
    {0x7FFFFFFDL, 3L, 0x2AAAAAA9L},
    {(-2147483647L), 2L, (-1073741823L)},
    {(-2147483647L), (-2L), 0x3FFFFFFFL},
    {0x7FFFFFFFL, 0x7FFFFFFFL, 1L},
    {(-0x7FFFFFFF-1), 0x7FFFFFFFL, (-1L)},
    {0x000003E8L, 0x7FFFFFFFL, 0L},
    {(-1000L), 0x7FFFFFFFL, 0L},
    {0x000003E8L, (-0x7FFFFFFF-1), 0L},
    {(-1000L), (-0x7FFFFFFF-1), 0L}
};

int main(int argc, char **argv)
{
    int num_tests = sizeof(test_data) / sizeof(test_data[0]);
    int failed = 0;
    
    printf("Running %d division test cases...\n", num_tests);
    
    for (int i = 0; i < num_tests; i++) {
        long result = div(test_data[i].dividend, test_data[i].divisor);
        
        if (result != test_data[i].expected) {
            printf("FAIL: Test #%d: div(%ld, %ld) = %ld, expected %ld\n",
                   i,
                   test_data[i].dividend,
                   test_data[i].divisor,
                   result,
                   test_data[i].expected);
            failed++;
        }
    }
    
    if (failed == 0) {
        printf("All %d tests PASSED!\n", num_tests);
        return 0;
    } else {
        printf("%d/%d tests FAILED\n", failed, num_tests);
        return 1;
    }
    return 0;
}

