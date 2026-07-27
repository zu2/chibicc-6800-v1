//
// 8-bit "compare against zero" must not depend on the incoming carry.
// Each expression is run twice, f=0 after clc and f=1 after sec.
// ldab/andb leave C alone, so any difference is a flag leak.
//
// Exit: 0 = pass, G*10+O = failed after sec, 100+G*10+O = failed after clc.
//
//   G 1 uchar c=5     4 masked=0      7 schar s=5     O 1 <   4 >
//     2 uchar c=0     5 negated c=5   8 schar s=0       2 >=  5 ==
//     3 masked=5      6 negated c=0   9 schar s=-5      3 <=  6 !=
//

#define C(f) if (f) asm(" sec"); else asm(" clc")

unsigned char c;
signed char s;

int u_lt(int f) { C(f); if (c <  0) return 1; return 0; }
int u_ge(int f) { C(f); if (c >= 0) return 1; return 0; }
int u_le(int f) { C(f); if (c <= 0) return 1; return 0; }
int u_gt(int f) { C(f); if (c >  0) return 1; return 0; }
int u_eq(int f) { C(f); if (c == 0) return 1; return 0; }
int u_ne(int f) { C(f); if (c != 0) return 1; return 0; }

int m_lt(int f) { C(f); if ((c & 0x0F) <  0) return 1; return 0; }
int m_ge(int f) { C(f); if ((c & 0x0F) >= 0) return 1; return 0; }
int m_le(int f) { C(f); if ((c & 0x0F) <= 0) return 1; return 0; }
int m_gt(int f) { C(f); if ((c & 0x0F) >  0) return 1; return 0; }
int m_eq(int f) { C(f); if ((c & 0x0F) == 0) return 1; return 0; }
int m_ne(int f) { C(f); if ((c & 0x0F) != 0) return 1; return 0; }

int n_lt(int f) { C(f); if (!(c <  0)) return 1; return 0; }
int n_ge(int f) { C(f); if (!(c >= 0)) return 1; return 0; }
int n_le(int f) { C(f); if (!(c <= 0)) return 1; return 0; }
int n_gt(int f) { C(f); if (!(c >  0)) return 1; return 0; }
int n_eq(int f) { C(f); if (!(c == 0)) return 1; return 0; }
int n_ne(int f) { C(f); if (!(c != 0)) return 1; return 0; }

int s_lt(int f) { C(f); if (s <  0) return 1; return 0; }
int s_ge(int f) { C(f); if (s >= 0) return 1; return 0; }
int s_le(int f) { C(f); if (s <= 0) return 1; return 0; }
int s_gt(int f) { C(f); if (s >  0) return 1; return 0; }
int s_eq(int f) { C(f); if (s == 0) return 1; return 0; }
int s_ne(int f) { C(f); if (s != 0) return 1; return 0; }

int chk(int got0, int got1, int want, int code)
{
  if (got0 != want) return code + 100;
  if (got1 != want) return code;
  return 0;
}

int main(void)
{
  int r;

  c = 5;                                            /* 1 */
  if ((r = chk(u_lt(0), u_lt(1), 0, 11))) return r;
  if ((r = chk(u_ge(0), u_ge(1), 1, 12))) return r;
  if ((r = chk(u_le(0), u_le(1), 0, 13))) return r;
  if ((r = chk(u_gt(0), u_gt(1), 1, 14))) return r;
  if ((r = chk(u_eq(0), u_eq(1), 0, 15))) return r;
  if ((r = chk(u_ne(0), u_ne(1), 1, 16))) return r;

  c = 0;                                            /* 2 */
  if ((r = chk(u_lt(0), u_lt(1), 0, 21))) return r;
  if ((r = chk(u_ge(0), u_ge(1), 1, 22))) return r;
  if ((r = chk(u_le(0), u_le(1), 1, 23))) return r;
  if ((r = chk(u_gt(0), u_gt(1), 0, 24))) return r;
  if ((r = chk(u_eq(0), u_eq(1), 1, 25))) return r;
  if ((r = chk(u_ne(0), u_ne(1), 0, 26))) return r;

  c = 0x25;                                         /* 3 */
  if ((r = chk(m_lt(0), m_lt(1), 0, 31))) return r;
  if ((r = chk(m_ge(0), m_ge(1), 1, 32))) return r;
  if ((r = chk(m_le(0), m_le(1), 0, 33))) return r;
  if ((r = chk(m_gt(0), m_gt(1), 1, 34))) return r;
  if ((r = chk(m_eq(0), m_eq(1), 0, 35))) return r;
  if ((r = chk(m_ne(0), m_ne(1), 1, 36))) return r;

  c = 0x20;                                         /* 4 */
  if ((r = chk(m_lt(0), m_lt(1), 0, 41))) return r;
  if ((r = chk(m_ge(0), m_ge(1), 1, 42))) return r;
  if ((r = chk(m_le(0), m_le(1), 1, 43))) return r;
  if ((r = chk(m_gt(0), m_gt(1), 0, 44))) return r;
  if ((r = chk(m_eq(0), m_eq(1), 1, 45))) return r;
  if ((r = chk(m_ne(0), m_ne(1), 0, 46))) return r;

  c = 5;                                            /* 5 */
  if ((r = chk(n_lt(0), n_lt(1), 1, 51))) return r;
  if ((r = chk(n_ge(0), n_ge(1), 0, 52))) return r;
  if ((r = chk(n_le(0), n_le(1), 1, 53))) return r;
  if ((r = chk(n_gt(0), n_gt(1), 0, 54))) return r;
  if ((r = chk(n_eq(0), n_eq(1), 1, 55))) return r;
  if ((r = chk(n_ne(0), n_ne(1), 0, 56))) return r;

  c = 0;                                            /* 6 */
  if ((r = chk(n_lt(0), n_lt(1), 1, 61))) return r;
  if ((r = chk(n_ge(0), n_ge(1), 0, 62))) return r;
  if ((r = chk(n_le(0), n_le(1), 0, 63))) return r;
  if ((r = chk(n_gt(0), n_gt(1), 1, 64))) return r;
  if ((r = chk(n_eq(0), n_eq(1), 0, 65))) return r;
  if ((r = chk(n_ne(0), n_ne(1), 1, 66))) return r;

  s = 5;                                            /* 7 */
  if ((r = chk(s_lt(0), s_lt(1), 0, 71))) return r;
  if ((r = chk(s_ge(0), s_ge(1), 1, 72))) return r;
  if ((r = chk(s_le(0), s_le(1), 0, 73))) return r;
  if ((r = chk(s_gt(0), s_gt(1), 1, 74))) return r;
  if ((r = chk(s_eq(0), s_eq(1), 0, 75))) return r;
  if ((r = chk(s_ne(0), s_ne(1), 1, 76))) return r;

  s = 0;                                            /* 8 */
  if ((r = chk(s_lt(0), s_lt(1), 0, 81))) return r;
  if ((r = chk(s_ge(0), s_ge(1), 1, 82))) return r;
  if ((r = chk(s_le(0), s_le(1), 1, 83))) return r;
  if ((r = chk(s_gt(0), s_gt(1), 0, 84))) return r;
  if ((r = chk(s_eq(0), s_eq(1), 1, 85))) return r;
  if ((r = chk(s_ne(0), s_ne(1), 0, 86))) return r;

  s = -5;                                           /* 9 */
  if ((r = chk(s_lt(0), s_lt(1), 1, 91))) return r;
  if ((r = chk(s_ge(0), s_ge(1), 0, 92))) return r;
  if ((r = chk(s_le(0), s_le(1), 1, 93))) return r;
  if ((r = chk(s_gt(0), s_gt(1), 0, 94))) return r;
  if ((r = chk(s_eq(0), s_eq(1), 0, 95))) return r;
  if ((r = chk(s_ne(0), s_ne(1), 1, 96))) return r;

  return 0;
}
