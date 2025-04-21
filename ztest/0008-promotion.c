#include "common.h"
//#include <stdio.h>
//#include <stdint.h>

int func(signed char a)
{
    return a;
}

int lfunc(signed char a)
{
    return a;
}

int main(int argc, char *argv[])
{
    signed char a;
    signed int b;
    signed long c;

    a = 0xA5;
    b = a;
    if ((unsigned)b != 0xFFA5)
        return 1;

    b = 0x1234;
    a = b;
    if (a != 0x34)
        return 2;

    a = 0xAB;
    if ((unsigned)func(a) != 0xFFAB)
        return 3;
    a = 0x20;
    if ((unsigned)func(a) != 0x0020)
        return 4;

    a = 0xA5;
    c = a;
    if ((unsigned long)c != 0xFFFFFFA5)
       return 11;

    b = 0x1234;
    a = b;
    if (a != 0x34)
        return 12;

    a = 0xAB;
    if ((unsigned long)lfunc(a) != 0xFFFFFFAB)
        return 13;

    a = 0x20;
    if ((unsigned long)lfunc(a) != 0x00000020)
        return 14;

    return 0;
}
