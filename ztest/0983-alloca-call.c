// test_alloca_call.c -- use a VLA or an alloca block around a call

int gi;
int gn;

int c1(int a)
{
    return a;
}

int c2(int a, int b)
{
    return a + b;
}

int c3(int a, int b, int c)
{
    return a + b + c;
}

int c4(int a, int b, int c, int d)
{
    return a + b + c + d;
}

int c6(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}

int c10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    return a + b + c + d + e + f + g + h + i + j;
}

int c12(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j,
        int k, int l)
{
    return a + b + c + d + e + f + g + h + i + j + k + l;
}

// The block sits below @bp, so putting the stack pointer back to @bp
// lets the next call write over it.
int args1(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c1(3);
    c1(4);
    return a[0] + a[1];
}

int args2(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c2(3, 4);
    c2(5, 6);
    return a[0] + a[1];
}

int args3(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c3(3, 4, 5);
    c3(6, 7, 8);
    return a[0] + a[1];
}

int args4(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return a[0] + a[1];
}

int args6(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c6(3, 4, 5, 6, 7, 8);
    c6(9, 10, 11, 12, 13, 14);
    return a[0] + a[1];
}

int args10(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c10(3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    c10(3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    return a[0] + a[1];
}

int args12(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c12(3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
    c12(3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
    return a[0] + a[1];
}

// The inner call ends while the outer args are still on the stack.
int nest24(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c2(3, c4(4, 5, 6, 7));
    c2(8, c4(9, 10, 11, 12));
    return a[0] + a[1];
}

int nest43(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c4(3, c3(4, 5, 6), 7, 8);
    c4(9, c3(10, 11, 12), 13, 14);
    return a[0] + a[1];
}

int nest62(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c6(3, 4, c2(5, 6), 7, 8, 9);
    c6(10, 11, c2(12, 13), 14, 15, 16);
    return a[0] + a[1];
}

int deep(int n)
{
    char a[n];

    a[0] = 1;
    a[1] = 2;
    c4(3, c3(4, c2(5, 6), 7), 8, 9);
    c4(10, c3(11, c2(12, 13), 14), 15, 16);
    return a[0] + a[1];
}

// A call must not lose a value written into the block after it returns.
int write_after(int n)
{
    char a[n];

    c4(1, 2, 3, 4);
    a[0] = 5;
    a[1] = 6;
    c4(7, 8, 9, 10);
    return a[0] + a[1];
}

int use_retval(int n)
{
    char a[n];
    int s;

    a[0] = 1;
    a[1] = 2;
    s = c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return a[0] + a[1] + s - 18;
}

int var_index(int n)
{
    char a[n];
    int s;

    a[gi] = 1;
    a[gi + 1] = 2;
    c4(3, 4, 5, 6);
    s = a[gi] + a[gi + 1];
    a[gi + 2] = 4;
    c4(7, 8, 9, 10);
    return s + a[gi + 2];
}

int in_loop(int n)
{
    char a[n];
    int i;
    int s;

    for (i = 0; i < 4; i = i + 1)
        a[i] = i + 1;
    s = 0;
    for (i = 0; i < 4; i = i + 1)
        s = s + c4(0, a[i], 0, 0);
    return s;
}

int two_vla(int n)
{
    char a[n];
    char b[n];

    a[0] = 1;
    b[0] = 2;
    c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return a[0] + b[0];
}

int with_fixed(int n)
{
    char f[4];
    char a[n];
    int i;

    for (i = 0; i < 4; i = i + 1)
        f[i] = i;
    a[0] = 1;
    a[1] = 2;
    c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return a[0] + a[1] + f[3] - 3;
}

int noparam(void)
{
    char a[gn];

    a[0] = 1;
    a[1] = 2;
    c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return a[0] + a[1];
}

int al_args2(int n)
{
    char *p = alloca(n);

    p[0] = 1;
    p[1] = 2;
    c2(3, 4);
    c2(5, 6);
    return p[0] + p[1];
}

int al_args4(int n)
{
    char *p = alloca(n);

    p[0] = 1;
    p[1] = 2;
    c4(3, 4, 5, 6);
    c4(7, 8, 9, 10);
    return p[0] + p[1];
}

int al_args10(int n)
{
    char *p = alloca(n);

    p[0] = 1;
    p[1] = 2;
    c10(3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    c10(3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    return p[0] + p[1];
}

int al_nested(int n)
{
    char *p = alloca(n);

    p[0] = 1;
    p[1] = 2;
    c2(3, c4(4, 5, 6, 7));
    c2(8, c4(9, 10, 11, 12));
    return p[0] + p[1];
}

int al_write_after(int n)
{
    char *p = alloca(n);

    c4(1, 2, 3, 4);
    p[0] = 5;
    p[1] = 6;
    c4(7, 8, 9, 10);
    return p[0] + p[1];
}

int al_twice(int n)
{
    char *p = alloca(n);
    char *q;

    p[0] = 1;
    c4(3, 4, 5, 6);
    q = alloca(n);
    q[0] = 2;
    c4(7, 8, 9, 10);
    return p[0] + q[0];
}

int main(void)
{
    gi = 0;
    gn = 8;

    if (args1(gn) != 3) return 1;
    if (args2(gn) != 3) return 2;
    if (args3(gn) != 3) return 3;
    if (args4(gn) != 3) return 4;
    if (args6(gn) != 3) return 5;
    if (args10(gn) != 3) return 6;
    if (args12(gn) != 3) return 7;

    if (nest24(gn) != 3) return 10;
    if (nest43(gn) != 3) return 11;
    if (nest62(gn) != 3) return 12;
    if (deep(gn) != 3) return 13;

    if (write_after(gn) != 11) return 20;
    if (use_retval(gn) != 3) return 21;
    if (var_index(gn) != 7) return 22;
    if (in_loop(gn) != 10) return 23;

    if (two_vla(gn) != 3) return 30;
    if (with_fixed(gn) != 3) return 31;
    if (noparam() != 3) return 32;

    if (al_args2(gn) != 3) return 40;
    if (al_args4(gn) != 3) return 41;
    if (al_args10(gn) != 3) return 42;
    if (al_nested(gn) != 3) return 43;
    if (al_write_after(gn) != 11) return 44;
    if (al_twice(gn) != 3) return 45;

    return 0;
}
