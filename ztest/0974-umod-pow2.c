unsigned int u;
unsigned short us;
unsigned char uc;
unsigned long ul;
int s;
_Bool b;

int main(void)
{
    u = 1000;
    if (u % 2 != 0) return 1;
    if (u % 4 != 0) return 2;
    if (u % 8 != 0) return 3;
    if (u % 16 != 8) return 4;
    if (u % 256 != 232) return 5;
    if (u % 512 != 488) return 6;
    if (u % 1024 != 1000) return 7;

    u = 0xFFFF;
    if (u % 2 != 1) return 10;
    if (u % 8 != 7) return 11;
    if (u % 256 != 255) return 12;
    if (u % 1024 != 1023) return 13;

    u = 0x8000;
    if (u % 2 != 0) return 20;
    if (u % 256 != 0) return 21;
    if (u % 1024 != 0) return 22;

    u = 0;
    if (u % 8 != 0) return 30;
    u = 7;
    if (u % 8 != 7) return 31;
    u = 8;
    if (u % 8 != 0) return 32;
    u = 1;
    if (u % 1 != 0) return 33;

    u = 1000;
    u %= 8;
    if (u != 0) return 40;
    u = 0xFFFF;
    u %= 256;
    if (u != 255) return 41;

    // the value is promoted to a signed int, but stays 0 or more
    uc = 200;
    if (uc % 8 != 0) return 50;
    if (uc % 16 != 8) return 51;
    uc = 255;
    if (uc % 2 != 1) return 52;
    if (uc % 128 != 127) return 53;

    us = 1000;
    if (us % 8 != 0) return 60;
    us = 0xFFFF;
    if (us % 256 != 255) return 61;

    b = 1;
    if (b % 2 != 1) return 70;
    b = 0;
    if (b % 2 != 0) return 71;

    ul = 1000000UL;
    if (ul % 8 != 0) return 80;
    if (ul % 1024 != 576) return 81;
    ul = 0xFFFFFFFFUL;
    if (ul % 256 != 255) return 82;

    // a signed value can be less than 0. "and" gives a wrong answer there
    s = -3;
    if (s % 2 != -1) return 90;
    s = -1000;
    if (s % 8 != 0) return 91;
    if (s % 16 != -8) return 92;
    s = 1000;
    if (s % 16 != 8) return 93;

    u = 1000;
    if (u % 10 != 0) return 100;
    if (u % 100 != 0) return 101;
    u = 1005;
    if (u % 10 != 5) return 102;

    return 0;
}
