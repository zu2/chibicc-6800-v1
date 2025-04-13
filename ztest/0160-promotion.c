#include "common.h"

int main(int argc,char **argv)
{
    signed char    sc = -1;
    signed char    c  = -128;
    signed char    c2 = -1;
    signed short   ss = -32768; // 16bit signed最小値
    signed int     si = -1;
    unsigned char  uc = 255;
    unsigned short s = 32768; // 16bit最大値+1
    unsigned short us = 65535; // 16bit unsigned最大値
    unsigned int   ui = 65535; // 16bit unsigned最大値
    unsigned int   ui2 = 32768;
    unsigned long  ul = c2; // 32bit変換
    int neg = -8;
    unsigned pos = 8;

    if ((int)sc != -1) return 1;

    if ((unsigned int)uc != 255U) return 2;

    if (c + s != 32640) return 3;

    if ((unsigned int)si != 65535U) return 4;

    if (us + 1 != 0) return 5;

    if (ss + ui2 != 0) return 6;

    if (ul != 4294967295UL) return 7;

    if ((neg >> 1) != -4 || (pos >> 1) != 4) return 8;

    if ((unsigned int)-32768 != 32768U) return 9;

    return 0;
}

