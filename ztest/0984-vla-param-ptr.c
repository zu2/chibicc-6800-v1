// test_vla_param_ptr.c -- a pointer to a VLA used as a parameter

int gn;
int gm;

int get(int n, int (*q)[n], int i, int j)
{
    return q[i][j];
}

void put(int n, int (*q)[n], int i, int j, int v)
{
    q[i][j] = v;
}

int step(int n, int (*q)[n])
{
    q++;
    return q[0][0];
}

int pre(int n, int (*q)[n])
{
    return (++q)[0][0];
}

int back(int n, int (*q)[n])
{
    q++;
    q--;
    return q[0][0];
}

int predec(int n, int (*q)[n])
{
    q++;
    return (--q)[0][0];
}

int addn(int n, int (*q)[n], int k)
{
    return (q + k)[0][0];
}

int subn(int n, int (*q)[n], int k)
{
    q = q + 3;
    return (q - k)[0][0];
}

int elem_size(int n, int (*q)[n])
{
    return sizeof(*q);
}

int row_size(int n, int (*q)[n])
{
    return sizeof(q[0]);
}

int plus1(int n, int (*q)[n + 1], int i, int j)
{
    return q[i][j];
}

int twovar(int n, int m, int (*q)[n * m], int i, int j)
{
    return q[i][j];
}

int useglobal(int (*q)[gn], int i, int j)
{
    return q[i][j];
}

int with_local(int n, int (*q)[n], int i, int j)
{
    int t;
    int u[3];

    t = 100;
    u[0] = 1;
    u[1] = 2;
    u[2] = 3;
    return q[i][j] + t + u[2] - 103;
}

int fill(int n, int (*q)[n], int rows)
{
    int i;
    int j;
    int v;

    v = 0;
    for (i = 0; i < rows; i = i + 1)
        for (j = 0; j < n; j = j + 1) {
            q[i][j] = v;
            v = v + 1;
        }
    return v;
}

int main(void)
{
    gn = 3;
    gm = 2;

    {
        int b[4][gn];

        if (fill(gn, b, 4) != 12) return 1;
        if (get(gn, b, 0, 0) != 0) return 2;
        if (get(gn, b, 1, 2) != 5) return 3;
        if (get(gn, b, 3, 0) != 9) return 4;

        put(gn, b, 2, 1, 77);
        if (get(gn, b, 2, 1) != 77) return 5;
        put(gn, b, 2, 1, 7);

        if (step(gn, b) != 3) return 10;
        if (pre(gn, b) != 3) return 11;
        if (back(gn, b) != 0) return 12;
        if (predec(gn, b) != 0) return 13;
        if (addn(gn, b, 2) != 6) return 14;
        if (subn(gn, b, 1) != 6) return 15;

        if (elem_size(gn, b) != gn * (int)sizeof(int)) return 20;
        if (row_size(gn, b) != gn * (int)sizeof(int)) return 21;

        if (useglobal(b, 1, 1) != 4) return 30;
        if (with_local(gn, b, 1, 1) != 4) return 31;
    }

    {
        int c[4][2];

        c[0][0] = 40;
        c[1][0] = 41;
        c[2][1] = 42;
        if (plus1(1, c, 0, 0) != 40) return 40;
        if (plus1(1, c, 1, 0) != 41) return 41;
        if (plus1(1, c, 2, 1) != 42) return 42;
    }

    {
        int d[3][6];

        d[0][0] = 50;
        d[1][5] = 51;
        d[2][3] = 52;
        if (twovar(gn, gm, d, 0, 0) != 50) return 50;
        if (twovar(gn, gm, d, 1, 5) != 51) return 51;
        if (twovar(gn, gm, d, 2, 3) != 52) return 52;
    }

    return 0;
}
