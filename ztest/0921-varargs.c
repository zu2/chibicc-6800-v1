//#include <stdio.h>
#include "common.h"
#include <stdarg.h>

// 可変長引数の合計を計算する関数
int sum_ints(int count, ...) {
    va_list ap;
    int sum = 0;

    va_start(ap, count); // countの直後から可変引数
    for (int i = 0; i < count; i++) {
        int value = va_arg(ap, int);
        sum += value;
    }
    va_end(ap);

    return sum;
}

int main(void) {
    int result1 = sum_ints(3, 10, 20, 30); // 10 + 20 + 30 = 60
    int result2 = sum_ints(5, 1, 2, 3, 4, 5); // 1 + 2 + 3 + 4 + 5 = 15

    putstr("sum_ints(3, 10, 20, 30) = ");print(result1);
    putstr("sum_ints(5, 1, 2, 3, 4, 5) = ");print( result2);

    if (result1!=60)
      return 1;

    if (result1!=60)
      return 2;

    return 0;
}

