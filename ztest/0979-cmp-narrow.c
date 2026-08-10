// checks that "cmp op cmp" and "cmp << n" keep their value

int a, b, r, sh;
short s1;
unsigned int w;
unsigned char u1, u2;
char c1, c2;
_Bool bo;
long l1, l2;
unsigned long v1, v2;
float f1, f2;
int arr[8];
int *p1, *p2;
int calls;

int hit(int x)
{
    calls++;
    return x;
}

int main(void)
{
    a = 1;
    b = 2;

    if (((a < b) | (a < b)) != 1) return 1;
    if (((a < b) | (b < a)) != 1) return 2;
    if (((b < a) | (a < b)) != 1) return 3;
    if (((b < a) | (b < a)) != 0) return 4;
    if (((a < b) ^ (a < b)) != 0) return 5;
    if (((a < b) ^ (b < a)) != 1) return 6;
    if (((b < a) ^ (a < b)) != 1) return 7;
    if (((b < a) ^ (b < a)) != 0) return 8;
    if (((a < b) & (a < b)) != 1) return 9;
    if (((a < b) & (b < a)) != 0) return 10;
    if (((b < a) & (a < b)) != 0) return 11;
    if (((b < a) & (b < a)) != 0) return 12;
    if (((a < b) + (a < b)) != 2) return 13;
    if (((a < b) + (b < a)) != 1) return 14;
    if (((b < a) + (a < b)) != 1) return 15;
    if (((b < a) + (b < a)) != 0) return 16;

    if (((a < b) << 0) != 1) return 20;
    if (((a < b) << 1) != 2) return 21;
    if (((a < b) << 2) != 4) return 22;
    if (((a < b) << 3) != 8) return 23;
    if (((a < b) << 4) != 16) return 24;
    if (((a < b) << 5) != 32) return 25;
    if (((a < b) << 6) != 64) return 26;
    if (((a < b) << 7) != 128) return 27;
    if (((a < b) << 8) != 256) return 28;
    if (((a < b) << 9) != 512) return 29;
    if (((a < b) << 10) != 1024) return 30;
    if (((a < b) << 11) != 2048) return 31;
    if (((a < b) << 12) != 4096) return 32;
    if (((a < b) << 13) != 8192) return 33;
    if (((a < b) << 14) != 16384) return 34;
    if (((b < a) << 0) != 0) return 35;
    if (((b < a) << 7) != 0) return 36;
    if (((b < a) << 8) != 0) return 37;
    if (((b < a) << 14) != 0) return 38;

    sh = 7;
    if (((a < b) << sh) != 128) return 40;
    sh = 8;
    if (((a < b) << sh) != 256) return 41;
    sh = 0;
    if (((a < b) << sh) != 1) return 42;

    if (((a == b) | (a != b)) != 1) return 50;
    if (((a == b) ^ (a != b)) != 1) return 51;
    if (((a == b) & (a != b)) != 0) return 52;
    if (((a == b) + (a != b)) != 1) return 53;
    if (((a <= b) | (a >= b)) != 1) return 54;
    if (((a <= b) ^ (a >= b)) != 1) return 55;
    if (((a <= b) & (a >= b)) != 0) return 56;
    if (((a <= b) + (a >= b)) != 1) return 57;
    if (((a > b) | (a < b)) != 1) return 58;
    if (((a >= a) + (b <= b)) != 2) return 59;
    if (((a == a) << 3) != 8) return 60;
    if (((a != a) << 3) != 0) return 61;

    if (((!a) | (!b)) != 0) return 70;
    if (((!a) ^ (!b)) != 0) return 71;
    if (((!a) & (!b)) != 0) return 72;
    if (((!a) + (!b)) != 0) return 73;
    if (((!a) | (a < b)) != 1) return 74;
    if (((!a) ^ (a < b)) != 1) return 75;
    if (((!a) + (a < b)) != 1) return 76;
    if (((!a) << 4) != 0) return 77;
    a = 0;
    if (((!a) | (!b)) != 1) return 78;
    if (((!a) ^ (!b)) != 1) return 79;
    if (((!a) & (!b)) != 0) return 80;
    if (((!a) + (!b)) != 1) return 81;
    if (((!a) << 0) != 1) return 82;
    if (((!a) << 7) != 128) return 83;
    if (((!a) << 8) != 256) return 84;
    if (((!a) << 14) != 16384) return 85;
    if (((!!a) | (!a)) != 1) return 86;
    a = 1;

    c1 = -1;
    c2 = 1;
    if (((c1 < c2) | (c2 < c1)) != 1) return 90;
    if (((c1 < c2) & (c2 < c1)) != 0) return 91;
    if (((c1 < c2) << 8) != 256) return 92;
    u1 = 200;
    u2 = 100;
    if (((u1 < u2) + (u2 < u1)) != 1) return 93;
    if (((u1 > u2) << 8) != 256) return 94;
    l1 = -100000;
    l2 = 100000;
    if (((l1 < l2) ^ (l2 < l1)) != 1) return 95;
    if (((l1 < l2) << 8) != 256) return 96;
    v1 = 4000000000u;
    v2 = 1;
    if (((v1 < v2) | (v2 < v1)) != 1) return 97;
    if (((v1 > v2) << 14) != 16384) return 98;
    f1 = -1.5;
    f2 = 2.5;
    if (((f1 < f2) & (f2 < f1)) != 0) return 99;
    if (((f1 < f2) | (f2 < f1)) != 1) return 100;
    if (((f1 < f2) << 8) != 256) return 101;
    p1 = arr;
    p2 = arr + 4;
    if (((p1 < p2) | (p2 < p1)) != 1) return 102;
    if (((p1 == p2) + (p1 != p2)) != 1) return 103;
    if (((p1 < p2) << 9) != 512) return 104;

    if (((a < b) | (b < a) | (a == b)) != 1) return 110;
    if (((a < b) ^ (b < a) ^ (a == a)) != 0) return 111;
    if (((a < b) + (b < a) + (a == a)) != 2) return 112;
    if ((((a < b) | (b < a)) << 3) != 8) return 113;
    if ((((a < b) + (a < b)) << 2) != 8) return 114;
    if ((((a < b) << 2) | ((b < a) << 1)) != 4) return 115;
    if ((((a < b) << 2) | ((a < b) << 1)) != 6) return 116;
    if ((-((a < b) | (b < a))) != -1) return 117;
    if ((~((a < b) | (b < a))) != -2) return 118;
    if ((((a < b) | (b < a)) * 1000) != 1000) return 119;
    if ((((a < b) + (a < b)) - 5) != -3) return 120;

    if (!((a < b) | (b < a))) return 130;
    if ((a < b) & (b < a)) return 131;
    while ((b < a) | (b < a)) return 132;
    c1 = (a < b) + (a < b);
    if (c1 != 2) return 133;
    u1 = (a < b) << 7;
    if (u1 != 128) return 134;
    s1 = (a < b) << 14;
    if (s1 != 16384) return 135;
    w = (a < b) << 8;
    if (w != 256) return 136;
    l1 = (a < b) << 8;
    if (l1 != 256L) return 137;
    bo = (a < b) | (b < a);
    if (bo != 1) return 138;
    arr[3] = 77;
    if (arr[((a < b) + (a < b)) + 1] != 77) return 139;
    r = ((a < b) | (b < a)) ? 5 : 6;
    if (r != 5) return 140;
    r = hit((a < b) + (a < b));
    if (r != 2) return 141;

    calls = 0;
    r = (hit(1) < 2) | (hit(2) < 1);
    if (calls != 2) return 150;
    if (r != 1) return 151;
    calls = 0;
    r = (hit(5) < 1) + (hit(6) < 1);
    if (calls != 2) return 152;
    if (r != 0) return 153;
    calls = 0;
    r = (hit(1) < 2) & (hit(2) < 1);
    if (calls != 2) return 154;
    if (r != 0) return 155;

    return 0;
}
