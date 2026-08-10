// 0980-arr-minus.c -- a global array indexed by "V - C"

typedef unsigned char  u8;
typedef signed   char  s8;
typedef unsigned short u16;
typedef unsigned long  u32;

static const u8  bt[256] = { [0]=11, [1]=22, [2]=33, [3]=77, [127]=44, [128]=55, [255]=66 };
static const s8  st[256] = { [0]=-11, [1]=-22, [128]=-55, [255]=-66 };
static const short ht[64] = { [0]=1111, [1]=2222, [63]=-3333 };
static const int   it[64] = { [0]=111, [1]=222, [63]=-333 };
static const long  lt[64] = { [0]=100000, [1]=200000, [63]=-300000 };

// the index must come from a call, or the whole thing is folded at compile time
static s8   ids8(s8 x)   { return x; }
static u8   idu8(u8 x)   { return x; }
static short idsh(short x) { return x; }
static u16  idu16(u16 x) { return x; }
static int  idi(int x)   { return x; }
static unsigned idu(unsigned x) { return x; }
static long idl(long x)  { return x; }
static u32  idu32(u32 x) { return x; }

static int   gi;
static u16   gu;
static u8    gb;
static int   garr[4];
static struct { int a; u16 b; } gs;

static const short smin = -32768;
static const short smax = 32767;
static const u16   umax = 65535;

int main(void)
{
    if (bt[idi(0) - 0]     != 11) return 1;
    if (bt[idi(1) - 1]     != 11) return 2;
    if (bt[idi(2) - 1]     != 22) return 3;
    if (bt[idi(128) - 128] != 11) return 4;
    if (bt[idi(129) - 128] != 22) return 5;
    if (bt[idi(255) - 128] != 44) return 6;
    if (bt[idi(-1) - (-1)] != 11) return 7;
    if (bt[idi(0) - (-1)]  != 22) return 8;
    if (bt[idi(0) - (-255)] != 66) return 9;

    if (bt[idu16(1) - 1]     != 11) return 10;
    if (bt[idu16(2) - 1]     != 22) return 11;
    if (bt[idu16(128) - 128] != 11) return 12;
    if (bt[idu16(383) - 128] != 66) return 13;
    // these wrap round in 16 bit, so they only work when int is 16 bit
    if (sizeof(int) == 2) {
        if (bt[idu16(0) - umax]     != 22) return 14;
        if (bt[idu16(2) - umax]     != 77) return 15;
        if (bt[idu(0) - 65535]      != 22) return 16;
    }

    if (*((idi(130) - 130) + bt) != 11) return 20;
    if (*(bt + (idi(131) - 130)) != 22) return 21;
    if (*((idu16(3) - 1) + bt)   != 33) return 22;

    if (st[idi(128) - 128] != -11) return 30;
    if (st[idi(129) - 128] != -22) return 31;
    if (ht[idi(64) - 64]   != 1111) return 32;
    if (ht[idi(65) - 64]   != 2222) return 33;
    if (it[idi(64) - 64]   != 111) return 34;
    if (it[idi(127) - 64]  != -333) return 35;
    if (lt[idi(64) - 64]   != 100000) return 36;
    if (lt[idi(127) - 64]  != -300000) return 37;

    if (bt[ids8(-128) - (-128)] != 11) return 40;
    if (bt[ids8(-127) - (-128)] != 22) return 41;
    if (bt[ids8(127) - 127]     != 11) return 42;
    if (bt[idu8(255) - 255]     != 11) return 43;
    if (bt[idu8(0) - 0]         != 11) return 44;
    if (bt[idsh(smax) - smax]   != 11) return 45;
    if (bt[idsh(-32767) - smin] != 22) return 46;
    if (bt[idsh(smin) - smin]   != 11) return 47;
    if (bt[idu(1) - 1]          != 11) return 48;
    if (bt[idu(129) - 128]      != 22) return 49;

    if (bt[idl(128) - 128]   != 11) return 50;
    if (bt[idl(129) - 128]   != 22) return 51;
    if (bt[idl(65536) - 65536] != 11) return 52;
    if (bt[idu32(65537) - 65536] != 22) return 53;

    const u8 *p = bt;
    if (p[idi(128) - 128] != 11) return 60;
    if (p[idi(129) - 128] != 22) return 61;
    u8 lc[4];
    lc[0] = 77; lc[1] = 88;
    if (lc[idi(2) - 2] != 77) return 62;
    if (lc[idi(3) - 2] != 88) return 63;

    if (bt[(idi(129) - 128) + 1] != 33) return 70;
    if (bt[(idi(3) - 1) - 1]  != 22) return 71;
    if (bt[idi(5) - 2 - 2]    != 22) return 72;
    if (bt[(u8)(idi(200) - 200)] != 11) return 73;
    if (bt[idi(300) - 300]    != 11) return 74;

    if (bt[idi(256) - 256]         != 11) return 80;
    if (bt[idi(257) - 256]         != 22) return 81;
    if (bt[idi(-254) - (-255)]     != 22) return 82;
    if (bt[idi(-254) - (-256)]     != 33) return 83;
    if (bt[idi(-32767) - (-32767)] != 11) return 84;
    if (bt[idi(-32766) - (-32767)] != 22) return 85;
    if (bt[idu16(32768) - (u16)32768]     != 11) return 86;
    if (bt[idu16(32769) - (u16)32768]     != 22) return 87;
    if (bt[idu16(32768) - (u16)32767]     != 22) return 88;

    if (bt[idl(-65535) - (-65535)]    != 11) return 90;
    if (bt[idl(-65534) - (-65535)]    != 22) return 91;
    if (bt[idl(-65534) - (-65536)]    != 33) return 92;
    if (bt[idl(65536) - 65536]        != 11) return 93;
    if (bt[idl(65538) - 65535]        != 77) return 94;
    if (bt[idl(2147483647L - 255) - (2147483647L - 255)] != 11) return 95;

    // the index comes from a variable, not straight from a call
    gi = idi(129);
    if (bt[gi - 128] != 22) return 100;
    gu = idu16(130);
    if (bt[gu - 128] != 33) return 101;
    gb = idu8(131);
    if (bt[gb - 128] != 77) return 102;
    int li = idi(129);
    if (bt[li - 128] != 22) return 103;
    u16 lu = idu16(130);
    if (bt[lu - 128] != 33) return 104;
    static int si;
    si = idi(131);
    if (bt[si - 128] != 77) return 105;

    garr[0] = idi(129);
    if (bt[garr[0] - 128] != 22) return 110;
    garr[1] = idi(130);
    if (bt[garr[1] - garr[0]]      != 22) return 111;
    gs.a = idi(129);
    if (bt[gs.a - 128] != 22) return 112;
    gs.b = idu16(130);
    if (bt[gs.b - 128] != 33) return 113;
    int *pi = &gi;
    gi = idi(130);
    if (bt[*pi - 128] != 33) return 114;

    static const u8 sbt[4] = { 44, 55, 66, 77 };
    static u8 lbt[4];
    lbt[0] = 44; lbt[1] = 55;
    gi = idi(129);
    if (sbt[gi - 128] != 55) return 120;
    if (lbt[gi - 128] != 55) return 121;
    u8 abt[4];
    abt[0] = 44; abt[1] = 55;
    if (abt[gi - 128] != 55) return 122;

    return 0;
}
