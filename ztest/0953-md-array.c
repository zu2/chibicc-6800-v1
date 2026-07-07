//
// multidimensional array indexing, pointer arithmetic, and layout
//
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int max2(int a, int b) { return (a > b) ? a : b; }

int main(void) {

    /* 1–20: function pointer */
    int (*fp[2][2])(int,int) = {
        { add, sub },
        { mul, max2 }
    };

    if (fp[0][0](10,5) != 15) return 1;
    if (fp[0][1](10,5) != 5)  return 2;
    if (fp[1][0](10,5) != 50) return 3;
    if (fp[1][1](10,5) != 10) return 4;

    int (*(*pf)[2])(int,int) = fp;

    if (pf[1][0](4,3) != 12) return 5;
    if (pf[1][1](4,3) != 4)  return 6;

    /* 21–60: char */
    char ca[2][2][2] = {{{'A','B'},{'C','D'}},{{'E','F'},{'G','H'}}};
    signed char sca[2][2] = {{-1,2},{3,-4}};
    unsigned char uca[2][2] = {{1,2},{3,4}};

    if (ca[1][1][1] != 'H') return 21;
    if (sca[1][1] != -4) return 22;
    if (uca[1][1] != 4) return 23;

    /* 61–100: integer */
    short sa[2][2] = {{1,2},{3,4}};
    unsigned int uia[2][2] = {{10U,20U},{30U,40U}};
    long la[2][2] = {{100L,200L},{300L,400L}};
    unsigned long ula[2][2] = {{1UL,2UL},{3UL,4UL}};

    if (sa[1][1] != 4) return 61;
    if (uia[1][0] != 30U) return 62;
    if (la[0][1] != 200L) return 63;
    if (ula[1][1] != 4UL) return 64;

    /* 101–130: float */
    float fa[2][2][2] = {
        {{1.0f,2.0f},{3.0f,4.0f}},
        {{5.0f,6.0f},{7.0f,8.0f}}
    };

    if (fa[1][1][1] != 8.0f) return 101;

    /* 131–160: const */
    const int cia[2][2] = {{1,2},{3,4}};
    const int (*cip)[2] = cia;

    if (cia[1][1] != 4) return 131;
    if (cip[1][0] != 3) return 132;

    /* 161–200: pointer element */
    int x = 10, y = 20, z = 30, w = 40;
    int *pa[2][2] = {{&x,&y},{&z,&w}};

    if (*pa[0][0] != 10) return 161;
    if (*pa[1][1] != 40) return 162;

    int *(*ppa)[2] = pa;
    if (*ppa[1][0] != 30) return 163;

    /* 201–230: multi-dim access */
    int a[2][3][4];
    int val = 0;

    for (int i=0;i<2;i++)
        for (int j=0;j<3;j++)
            for (int k=0;k<4;k++)
                a[i][j][k] = ++val;

    int (*p3)[3][4] = a;
    if (p3[1][2][3] != 24) return 201;

    int (*p2)[4] = a[1];
    if (p2[2][3] != 24) return 202;

    int *p1 = &a[0][0][0];
    if (p1[23] != 24) return 203;

    if (*(*(*(a+1)+2)+3) != 24) return 204;

    /* 231–255: layout */
    if ((char*)&a[0][1][0] - (char*)&a[0][0][0] != 4*sizeof(int)) return 231;
    if ((char*)&a[1][0][0] - (char*)&a[0][0][0] != 12*sizeof(int)) return 232;

    return 0;
}
