// test_ixdisp.c -- a local array with an index variable far from @bp

int gi;
int gj;

int touch(void)
{
    return 0;
}

int int_array(void)
{
    int a;
    int r[300];
    int b;

    a = gi;
    b = gj;
    r[a] = 11;
    r[b] = 22;
    touch();
    return r[a] + r[b];
}

int char_array(void)
{
    char a;
    char r[600];
    char b;

    a = gi;
    b = gj;
    r[a] = 33;
    r[b] = 44;
    touch();
    return r[a] + r[b];
}

int short_index(void)
{
    short a;
    int r[300];
    short b;

    a = gi;
    b = gj;
    r[a] = 55;
    r[b] = 66;
    touch();
    return r[a] + r[b];
}

int read_only(void)
{
    int a;
    int r[300];
    int b;

    a = gi;
    b = gj;
    r[0] = 77;
    r[299] = 88;
    a = a - a;
    b = b - b + 299;
    touch();
    return r[a] + r[b];
}

int two_arrays(void)
{
    int a;
    int r[300];
    int s[300];
    int b;

    a = gi;
    b = gj;
    r[a] = 1;
    s[a] = 2;
    r[b] = 4;
    s[b] = 8;
    touch();
    return r[a] + s[a] + r[b] + s[b];
}

int main(void)
{
    gi = 0;
    gj = 299;

    if (int_array() != 33) return 1;
    if (char_array() != 77) return 2;
    if (short_index() != 121) return 3;
    if (read_only() != 165) return 4;
    if (two_arrays() != 15) return 5;

    gi = 299;
    gj = 0;

    if (int_array() != 33) return 10;
    if (short_index() != 121) return 11;
    if (two_arrays() != 15) return 12;

    gi = 150;
    gj = 150;

    if (int_array() != 44) return 20;
    if (two_arrays() != 24) return 21;

    return 0;
}
