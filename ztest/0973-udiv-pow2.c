unsigned int u;
unsigned short us;
int s;
short ss;

int main(void)
{
    u = 1000;
    if (u / 2 != 500) return 1;
    if (u / 4 != 250) return 2;
    if (u / 8 != 125) return 3;
    if (u / 16 != 62) return 4;
    if (u / 32 != 31) return 5;
    if (u / 256 != 3) return 6;

    // the top bit must be kept. a signed shift would make it 0xFFFF
    u = 0xFFFF;
    if (u / 2 != 0x7FFF) return 10;
    if (u / 4 != 0x3FFF) return 11;
    if (u / 8 != 0x1FFF) return 12;
    if (u / 16 != 0x0FFF) return 13;
    if (u / 256 != 0x00FF) return 14;

    u = 0x8000;
    if (u / 2 != 0x4000) return 20;
    if (u / 8 != 0x1000) return 21;
    if (u / 16 != 0x0800) return 22;

    u = 0;
    if (u / 8 != 0) return 30;
    u = 7;
    if (u / 8 != 0) return 31;
    u = 8;
    if (u / 8 != 1) return 32;
    u = 15;
    if (u / 8 != 1) return 33;
    u = 1;
    if (u / 1 != 1) return 34;

    u = 1000;
    u /= 8;
    if (u != 125) return 40;
    u = 0xFFFF;
    u /= 16;
    if (u != 0x0FFF) return 41;

    us = 1000;
    if (us / 8 != 125) return 50;
    us = 0xFFFF;
    if (us / 8 != 0x1FFF) return 51;

    s = 1000;
    if (s / 2 != 500) return 60;
    if (s / 4 != 250) return 61;
    if (s / 8 != 125) return 62;
    if (s / 16 != 62) return 63;
    if (s / 256 != 3) return 64;

    // a shift alone rounds down. signed divide must round toward zero
    s = -1000;
    if (s / 2 != -500) return 65;
    if (s / 4 != -250) return 66;
    if (s / 8 != -125) return 67;
    if (s / 16 != -62) return 68;
    if (s / 256 != -3) return 69;

    s = -1;
    if (s / 2 != 0) return 70;
    s = -3;
    if (s / 2 != -1) return 71;
    s = -7;
    if (s / 8 != 0) return 72;
    s = -8;
    if (s / 8 != -1) return 73;
    s = -9;
    if (s / 8 != -1) return 74;
    s = 0;
    if (s / 2 != 0) return 75;

    s = 32767;
    if (s / 2 != 16383) return 76;
    s = -32768;
    if (s / 2 != -16384) return 77;
    if (s / 256 != -128) return 78;

    s = -1000;
    s /= 2;
    if (s != -500) return 79;

    ss = -1000;
    if (ss / 2 != -500) return 80;
    ss = 32767;
    if (ss / 2 != 16383) return 81;

    // x / 8 and x >> 3 must give the same answer for unsigned
    u = 1000;
    if (u / 8 != u >> 3) return 90;
    u = 0xFFFF;
    if (u / 16 != u >> 4) return 91;

    return 0;
}
