// checks "x <= C" and "x > C" at the edge of each type

signed char sc;
unsigned char uc;
char pc;
_Bool b;
enum E { A, B, C, D } e;
short s;
unsigned short us;
long l;
unsigned long ul;

int main(void)
{
    sc = 127;
    if ((sc <= 126) != 0) return 10;
    if ((sc <= 127) != 1) return 11;
    if ((sc >  126) != 1) return 12;
    if ((sc >  127) != 0) return 13;
    sc = 126;
    if ((sc <= 126) != 1) return 14;
    if ((sc >  126) != 0) return 15;
    sc = -128;
    if ((sc <= -128) != 1) return 16;
    if ((sc >  -128) != 0) return 17;
    if ((sc <= -129) != 0) return 18;
    if ((sc >  -129) != 1) return 19;

    uc = 255;
    if ((uc <= 254) != 0) return 20;
    if ((uc <= 255) != 1) return 21;
    if ((uc >  254) != 1) return 22;
    if ((uc >  255) != 0) return 23;
    uc = 254;
    if ((uc <= 254) != 1) return 24;
    if ((uc >  254) != 0) return 25;
    uc = 0;
    if ((uc <= 0) != 1) return 26;
    if ((uc >  0) != 0) return 27;
    uc = 128;
    if ((uc <= 127) != 0) return 28;
    if ((uc >  127) != 1) return 29;

    pc = 100;
    if ((pc <= 99)  != 0) return 30;
    if ((pc <= 100) != 1) return 31;
    if ((pc >  99)  != 1) return 32;
    if ((pc >  100) != 0) return 33;

    b = 1;
    if ((b <= 0) != 0) return 40;
    if ((b >  0) != 1) return 41;
    if ((b <= 1) != 1) return 42;
    if ((b >  1) != 0) return 43;
    b = 0;
    if ((b <= 0) != 1) return 44;
    if ((b >  0) != 0) return 45;

    e = C;
    if ((e <= B) != 0) return 50;
    if ((e <= C) != 1) return 51;
    if ((e >  B) != 1) return 52;
    if ((e >  C) != 0) return 53;

    s = 32767;
    if ((s <= 32766) != 0) return 60;
    if ((s <= 32767) != 1) return 61;
    if ((s >  32766) != 1) return 62;
    if ((s >  32767) != 0) return 63;
    s = -32768;
    if ((s <= -32768) != 1) return 64;
    if ((s >  -32768) != 0) return 65;
    us = 65535;
    if ((us <= 65534u) != 0) return 66;
    if ((us <= 65535u) != 1) return 67;
    if ((us >  65534u) != 1) return 68;
    if ((us >  65535u) != 0) return 69;

    l = 2147483647;
    if ((l <= 2147483646) != 0) return 70;
    if ((l <= 2147483647) != 1) return 71;
    if ((l >  2147483646) != 1) return 72;
    if ((l >  2147483647) != 0) return 73;
    ul = 4294967295u;
    if ((ul <= 4294967294u) != 0) return 74;
    if ((ul <= 4294967295u) != 1) return 75;
    if ((ul >  4294967294u) != 1) return 76;
    if ((ul >  4294967295u) != 0) return 77;

    return 0;
}
