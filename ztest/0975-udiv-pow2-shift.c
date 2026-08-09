unsigned int u;
unsigned short us;
unsigned char uc;
unsigned long ul;
long l;
int s;
signed char sc;
_Bool b;

int main(void)
{
    u = 1000;
    if (u / 32 != 31) return 1;
    if (u / 64 != 15) return 2;
    if (u / 128 != 7) return 3;
    if (u / 256 != 3) return 4;
    if (u / 512 != 1) return 5;
    if (u / 1024 != 0) return 6;

    u = 0xFFFF;
    if (u / 32 != 2047) return 10;
    if (u / 256 != 255) return 11;
    if (u / 1024 != 63) return 12;
    if (u / 16384 != 3) return 13;
    if (u / 32768u != 1) return 14;

    u = 0x8000;
    if (u / 32 != 1024) return 20;
    if (u / 256 != 128) return 21;
    if (u / 32768u != 1) return 22;

    u = 0;
    if (u / 32 != 0) return 30;
    u = 31;
    if (u / 32 != 0) return 31;
    u = 32;
    if (u / 32 != 1) return 32;
    u = 63;
    if (u / 32 != 1) return 33;

    u = 1000;
    u /= 32;
    if (u != 31) return 40;
    u = 0xFFFF;
    u /= 256;
    if (u != 255) return 41;

    // the value is promoted to a signed int, but stays 0 or more
    uc = 200;
    if (uc / 8 != 25) return 50;
    if (uc / 32 != 6) return 51;
    uc = 255;
    if (uc / 2 != 127) return 52;
    if (uc / 128 != 1) return 53;

    us = 1000;
    if (us / 32 != 31) return 60;
    us = 0xFFFF;
    if (us / 256 != 255) return 61;

    b = 1;
    if (b / 2 != 0) return 70;

    ul = 1000000UL;
    if (ul / 2 != 500000UL) return 80;
    if (ul / 32 != 31250UL) return 81;
    if (ul / 1024 != 976UL) return 82;
    ul = 0xFFFFFFFFUL;
    if (ul / 256 != 0xFFFFFFUL) return 83;
    if (ul / 65536UL != 0xFFFFUL) return 84;
    ul = 0;
    if (ul / 32 != 0) return 85;
    ul = 31;
    if (ul / 32 != 0) return 86;

    l = -1000000L;
    if (l / 2 != -500000L) return 87;
    if (l / 32 != -31250L) return 88;
    l = -9L;
    if (l / 8 != -1L) return 89;

    // a shift rounds down. a signed divide must round toward zero
    s = -9;
    if (s / 8 != -1) return 90;
    s = -1;
    if (s / 2 != 0) return 91;
    s = -1000;
    if (s / 4 != -250) return 92;
    if (s / 32 != -31) return 93;
    if (s / 256 != -3) return 94;
    if (s / 1024 != 0) return 95;
    s = 1000;
    if (s / 32 != 31) return 96;
    s = -32768;
    if (s / 2 != -16384) return 97;
    if (s / 256 != -128) return 98;
    sc = -100;
    if (sc / 8 != -12) return 99;

    u = 1000;
    if (u / 10 != 100) return 100;
    if (u / 100 != 10) return 101;
    u = 1005;
    if (u / 3 != 335) return 102;

    return 0;
}
