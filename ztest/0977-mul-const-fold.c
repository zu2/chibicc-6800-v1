// checks "(x + c1) * c2" and "(x * c1) * c2" when c1 * c2 is big

unsigned long ul;
long l;
unsigned u;
int s;
int a[400];
int i;

int main(void)
{
    ul = 3;
    if ((ul + 4294967295u) * 4294967295u != 4294967294ul) return 10;
    if ((ul - 4294967295u) * 4294967295u != 4294967292ul) return 11;
    if ((ul + 2147483647u) * 4 != 8ul) return 12;
    if ((ul + 65536u) * 65536u != 196608ul) return 13;
    if ((ul + 100000u) * 100000u != 1410365408ul) return 14;

    u = 3;
    if ((u + 65535u) * 65535u != 65534u) return 20;
    if ((u - 65535u) * 65535u != 65532u) return 21;
    if ((u + 300u) * 300u != 25364u) return 22;
    if ((u + 256u) * 256u != 768u) return 23;

    l = 3;
    if ((l + 100) * 100 != 10300L) return 30;
    if ((l - 100) * 100 != -9700L) return 31;

    s = 3;
    if ((s + 100) * 100 != 10300) return 40;
    if ((s - 100) * 100 != -9700) return 41;

    u = 3;
    if ((u * 14) * 8 != 336u) return 50;
    if ((u * 60) * 60 != 10800u) return 51;
    if ((u * 300) * 300 != 7856u) return 52;
    if ((u * 256) * 256 != 0u) return 53;
    if ((u * 1) * 7 != 21u) return 54;
    if ((u * 0) * 7 != 0u) return 55;

    s = -3;
    if ((s * 14) * 8 != -336) return 60;
    if ((s * 5) * 5 != -75) return 61;

    ul = 3;
    if ((ul * 100000) * 100000 != 4230196224ul) return 70;
    if ((ul * 65536) * 65536 != 0ul) return 71;
    if ((ul * 4294967295u) * 4294967295u != 3ul) return 72;

    l = -3;
    if ((l * 1000) * 1000 != -3000000L) return 80;

    i = 5;
    a[i * 7] = 111;
    if (a[35] != 111) return 90;
    a[i * 6] = 222;
    if (a[30] != 222) return 91;
    a[3 * i] = 333;
    if (a[15] != 333) return 92;
    if (a[i * 7] != 111) return 93;

    return 0;
}
