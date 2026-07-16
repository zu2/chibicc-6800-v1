int main(void)
{
    signed char s;
    unsigned char u;
    signed char s2;
    unsigned char u2;
    short sh;
    short sh2;
    unsigned short us;
    unsigned short us2;

    // signed char %= int
    s = -5;   s %= 3;    if (s != -2) return 11;
    s = -5;   s %= -3;   if (s != -2) return 12;
    s = 5;    s %= -3;   if (s != 2)  return 13;
    s = -128; s %= 3;    if (s != -2) return 14;
    s = -128; s %= -3;   if (s != -2) return 15;
    // unsigned char %= int
    u = 5;    u %= -3;   if (u != 2)  return 21;
    u = 255;  u %= 4;    if (u != 3)  return 22;
    u = 255;  u %= -4;   if (u != 3)  return 23;
    u = 254;  u %= 6;    if (u != 2)  return 24;

    // signed char %= signed char
    s2 = 3;   s = -5;    s %= s2; if (s != -2) return 31;
    s2 = -3;  s = -5;    s %= s2; if (s != -2) return 32;
    s2 = -3;  s = 5;     s %= s2; if (s != 2)  return 33;
    s2 = 3;   s = -128;  s %= s2; if (s != -2) return 34;

    // unsigned char %= unsigned char
    u2 = 4;   u = 255;   u %= u2; if (u != 3)  return 41;
    u2 = 150; u = 200;   u %= u2; if (u != 50) return 42;
    u2 = 6;   u = 254;   u %= u2; if (u != 2)  return 43;

    // unsigned char %= signed char
    s2 = -3;  u = 5;     u %= s2; if (u != 2)  return 51;
    s2 = -4;  u = 255;   u %= s2; if (u != 3)  return 52;

    // signed char %= unsigned char
    u2 = 3;   s = -5;    s %= u2; if (s != -2) return 61;
    u2 = 255; s = -128;  s %= u2; if (s != -128) return 62;

    // Trivial cases
    s = -6;   s %= 3;    if (s != 0)  return 71;
    u = 255;  u %= 5;    if (u != 0)  return 72;

    // Overflow check (E1=MIN, E2=-1)
    s2 = -1;  s = -128;  s %= s2; if (s != 0)  return 81;
    sh2 = -1; sh = -32768; sh %= sh2; if (sh != 0) return 82;
    s2 = -1;  s = -128;  s %= -1; if (s != 0)  return 83; // literal E2
    sh2 = -1; sh = -32768; sh %= -1; if (sh != 0) return 84; // literal E2

    // short promotions (short %= int)
    sh = -5;     sh %= 3;    if (sh != -2) return 91;
    sh = -5;     sh %= -3;   if (sh != -2) return 92;
    sh = 5;      sh %= -3;   if (sh != 2)  return 93;
    sh = -32768; sh %= 3;    if (sh != -2) return 94;

    // unsigned short promotions (unsigned short %= int)
    // On 16-bit int, becomes unsigned operation: 5 % 65533
    us = 5;      us %= -3;   if (us != 5)  return 101;
    us = 65535;  us %= 4;    if (us != 3)  return 102;
    us = 65535;  us %= -4;   if (us != 3)  return 103;

    // short %= short
    sh2 = 3;   sh = -5;     sh %= sh2; if (sh != -2) return 111;
    sh2 = -3;  sh = -5;     sh %= sh2; if (sh != -2) return 112;
    sh2 = -3;  sh = 5;      sh %= sh2; if (sh != 2)  return 113;
    sh2 = 3;   sh = -32768; sh %= sh2; if (sh != -2) return 114;

    // unsigned short %= unsigned short
    us2 = 4;     us = 65535; us %= us2; if (us != 3)  return 121;
    us2 = 40000; us = 50000; us %= us2; if (us != 10000) return 122;

    // Mixed short / char promotions
    s2 = -3;   sh = 5;      sh %= s2;  if (sh != 2)  return 131;
    sh2 = -3;  s = 5;       s %= sh2;  if (s != 2)   return 132;
    us2 = 4;   u = 255;     u %= us2;  if (u != 3)   return 133;
    u2 = 4;    us = 65535;  us %= u2;  if (us != 3)  return 134;

    return 0;
}
