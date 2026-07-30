//
// Test for compare operators. The result is stored in an int.
// Returns 0 if all tests pass, or the number of the test that failed.
//
// Run it with -O and without -O.
//
// Test numbers are grouped by the shape of the compare. Each group gets
// 20 numbers, split into two halves of 10, and the last numbers of each
// half are free. So new tests can be added without moving the old ones.
//
//     1 -  20  signed char, right side is not direct
//    21 -  40  unsigned char, right side is not direct
//    41 -  60  signed char, neither side is direct (uses sba)
//    61 -  80  unsigned char, neither side is direct (uses sba)
//    81 - 100  expressions on both sides (uses sba)
//   101 - 120  char, right side is direct
//   121 - 140  char against 0
//   141 - 160  int, right side is not direct
//   161 - 180  int and unsigned int against 0
//   181 - 200  unsigned int
//   201 - 220  carry flag is dirty before the compare
//   221 - 240  mixed signs
//

typedef signed char   s8;
typedef unsigned char u8;

// A function call is not a direct operand.
s8  sid(s8  x) { return x; }
u8  uid(u8  x) { return x; }
int iid(int x) { return x; }
unsigned int uiid(unsigned int x) { return x; }

// 1 - 10 : signed char, right side is not direct.
// The compiler may swap the two sides here, because the left side is direct.
int t_s8_call(void) {
  s8 a5 = 5, a3 = 3;
  int r;

  r = (a5 == sid(a5));         if (r != 1) return 1;
  r = (a5 == sid(a3));         if (r != 0) return 2;
  r = (a5 != sid(a5));         if (r != 0) return 3;
  r = (a5 != sid(a3));         if (r != 1) return 4;
  r = (a3 <  sid(a5));         if (r != 1) return 5;
  r = (a5 <  sid(a3));         if (r != 0) return 6;
  r = (a5 <= sid(a5));         if (r != 1) return 7;
  r = (a5 >= sid(a5));         if (r != 1) return 8;
  return 0;
}

// 11 - 20 : signed char, edge values.
int t_s8_edge(void) {
  s8 am1 = -1, am128 = -128, a127 = 127, z = 0, a5 = 5, a3 = 3;
  int r;

  r = (z == sid(z));           if (r != 1) return 11;
  r = (am128 == sid(am128));   if (r != 1) return 12;
  r = (a127  == sid(a127));    if (r != 1) return 13;
  r = (am1 != sid(z));         if (r != 1) return 14;
  r = (am1 <  sid(z));         if (r != 1) return 15;
  r = (am128 < sid(a127));     if (r != 1) return 16;
  r = (a127 > sid(am128));     if (r != 1) return 17;
  r = (a5 > sid(a3));          if (r != 1) return 18;
  return 0;
}

// 21 - 30 : unsigned char, right side is not direct.
int t_u8_call(void) {
  u8 b5 = 5, b3 = 3, b200 = 200, b255 = 255, b0 = 0;
  int r;

  r = (b5   == uid(b5));       if (r != 1) return 21;
  r = (b200 == uid(b200));     if (r != 1) return 22;
  r = (b255 == uid(b255));     if (r != 1) return 23;
  r = (b0   == uid(b0));       if (r != 1) return 24;
  r = (b5   == uid(b3));       if (r != 0) return 25;
  r = (b5   != uid(b5));       if (r != 0) return 26;
  r = (b5   != uid(b3));       if (r != 1) return 27;
  // 200 looks negative as a signed byte
  r = (b3   <  uid(b200));     if (r != 1) return 28;
  return 0;
}

// 31 - 40 : unsigned char, the rest of the operators.
int t_u8_call2(void) {
  u8 b5 = 5, b3 = 3, b200 = 200, b255 = 255, b0 = 0;
  int r;

  r = (b200 <  uid(b3));       if (r != 0) return 31;
  r = (b0   <  uid(b255));     if (r != 1) return 32;
  r = (b3   <= uid(b3));       if (r != 1) return 33;
  r = (b255 <= uid(b3));       if (r != 0) return 34;
  r = (b255 >  uid(b3));       if (r != 1) return 35;
  r = (b3   >  uid(b200));     if (r != 0) return 36;
  r = (b255 >= uid(b255));     if (r != 1) return 37;
  r = (b3   >= uid(b200));     if (r != 0) return 38;
  return 0;
}

// 41 - 50 : both sides are calls, so neither side is direct.
// The compiler cannot swap its way out, so it must use sba.
int t_sba_s8(void) {
  s8 a5 = 5, a3 = 3, am128 = -128, a127 = 127, z = 0;
  int r;

  r = (sid(a5) == sid(a5));         if (r != 1) return 41;
  r = (sid(a5) == sid(a3));         if (r != 0) return 42;
  r = (sid(z)  == sid(z));          if (r != 1) return 43;
  r = (sid(am128) == sid(am128));   if (r != 1) return 44;
  r = (sid(a127)  == sid(a127));    if (r != 1) return 45;
  r = (sid(a5) != sid(a5));         if (r != 0) return 46;
  r = (sid(a5) != sid(a3));         if (r != 1) return 47;
  r = (sid(a3) <  sid(a5));         if (r != 1) return 48;
  return 0;
}

// 51 - 60 : both sides are calls, the rest.
int t_sba_s8b(void) {
  s8 a5 = 5, a3 = 3, am1 = -1, am128 = -128, a127 = 127, z = 0;
  int r;

  r = (sid(a5) <  sid(a3));         if (r != 0) return 51;
  r = (sid(am128) < sid(a127));     if (r != 1) return 52;
  r = (sid(a5) <= sid(a5));         if (r != 1) return 53;
  r = (sid(a5) <= sid(a3));         if (r != 0) return 54;
  r = (sid(a5) >  sid(a3));         if (r != 1) return 55;
  r = (sid(a3) >  sid(a5));         if (r != 0) return 56;
  r = (sid(a5) >= sid(a5));         if (r != 1) return 57;
  r = (sid(am1) >= sid(z));         if (r != 0) return 58;
  return 0;
}

// 61 - 70 : both sides are calls, unsigned char.
int t_sba_u8(void) {
  u8 b5 = 5, b3 = 3, b200 = 200, b255 = 255, b0 = 0;
  int r;

  r = (uid(b5)   == uid(b5));       if (r != 1) return 61;
  r = (uid(b200) == uid(b200));     if (r != 1) return 62;
  r = (uid(b255) == uid(b255));     if (r != 1) return 63;
  r = (uid(b0)   == uid(b0));       if (r != 1) return 64;
  r = (uid(b5)   == uid(b3));       if (r != 0) return 65;
  r = (uid(b5)   != uid(b5));       if (r != 0) return 66;
  r = (uid(b5)   != uid(b3));       if (r != 1) return 67;
  r = (uid(b3)   <  uid(b200));     if (r != 1) return 68;
  return 0;
}

// 71 - 80 : both sides are calls, unsigned char, the rest.
int t_sba_u8b(void) {
  u8 b5 = 5, b3 = 3, b200 = 200, b255 = 255, b0 = 0;
  int r;

  r = (uid(b200) <  uid(b3));       if (r != 0) return 71;
  r = (uid(b0)   <  uid(b255));     if (r != 1) return 72;
  r = (uid(b200) <= uid(b255));     if (r != 1) return 73;
  r = (uid(b255) <= uid(b3));       if (r != 0) return 74;
  r = (uid(b255) >  uid(b3));       if (r != 1) return 75;
  r = (uid(b3)   >  uid(b200));     if (r != 0) return 76;
  r = (uid(b255) >= uid(b255));     if (r != 1) return 77;
  r = (uid(b3)   >= uid(b200));     if (r != 0) return 78;
  return 0;
}

// 81 - 90 : expressions instead of calls.
int t_sba_expr(void) {
  s8 a5 = 5, a3 = 3;
  u8 b5 = 5, b3 = 3;
  int r;

  r = ((s8)(a3 ^ 6) == (s8)(a3 ^ 6));   if (r != 1) return 81;
  r = ((s8)(a3 ^ 6) == (s8)(a5 ^ 6));   if (r != 0) return 82;
  r = ((u8)(b3 ^ 6) == (u8)(b3 ^ 6));   if (r != 1) return 83;
  r = ((s8)(a3 ^ 6) != (s8)(a3 ^ 6));   if (r != 0) return 84;
  r = ((s8)(a3 ^ 7) <  (s8)(a3 ^ 6));   if (r != 1) return 85;
  r = ((s8)(a3 ^ 6) <= (s8)(a3 ^ 6));   if (r != 1) return 86;
  // one call and one expression
  r = (sid(a5) == (s8)(a3 ^ 6));        if (r != 1) return 87;
  r = ((u8)(b3 ^ 6) == uid(b5));        if (r != 1) return 88;
  return 0;
}

// 101 - 110 : right side is direct. these should pass already.
int t_direct_s8(void) {
  s8 a5 = 5, a3 = 3, am128 = -128, a127 = 127;
  int r;

  r = (a5 == a5);              if (r != 1) return 101;
  r = (a5 == 5);               if (r != 1) return 102;
  r = (a5 == a3);              if (r != 0) return 103;
  r = (a5 != a3);              if (r != 1) return 104;
  r = (a3 <  a5);              if (r != 1) return 105;
  r = (a5 <= a5);              if (r != 1) return 106;
  r = (a5 >  a3);              if (r != 1) return 107;
  r = (am128 < a127);          if (r != 1) return 108;
  return 0;
}

// 111 - 120 : right side is direct, unsigned char.
int t_direct_u8(void) {
  u8 b5 = 5, b3 = 3, b200 = 200, b255 = 255, b0 = 0;
  int r;

  r = (b200 == b200);          if (r != 1) return 111;
  r = (b200 == 200);           if (r != 1) return 112;
  r = (b5   != b3);            if (r != 1) return 113;
  r = (b3   <  b200);          if (r != 1) return 114;
  r = (b200 <= b255);          if (r != 1) return 115;
  r = (b255 >  b5);            if (r != 1) return 116;
  r = (b200 >= b200);          if (r != 1) return 117;
  r = (b3   >  b200);          if (r != 0) return 118;
  return 0;
}

// 121 - 130 : signed char against 0.
int t_zero_s8(void) {
  s8 a5 = 5, am1 = -1, am128 = -128, z = 0;
  int r;

  r = (a5 == 0);               if (r != 0) return 121;
  r = (z  == 0);               if (r != 1) return 122;
  r = (a5 != 0);               if (r != 1) return 123;
  r = (z  != 0);               if (r != 0) return 124;
  r = (am1   <  0);            if (r != 1) return 125;
  r = (am128 <  0);            if (r != 1) return 126;
  r = (z  <= 0);               if (r != 1) return 127;
  r = (a5 >  0);               if (r != 1) return 128;
  r = (am1 >= 0);              if (r != 0) return 129;
  return 0;
}

// 131 - 140 : unsigned char against 0.
// "< 0" is always false and ">= 0" is always true.
int t_zero_u8(void) {
  u8 b200 = 200, b0 = 0;
  int r;

  r = (b0   == 0);             if (r != 1) return 131;
  r = (b200 != 0);             if (r != 1) return 132;
  r = (b0   != 0);             if (r != 0) return 133;
  r = (b200 <  0);             if (r != 0) return 134;
  r = (b0   <= 0);             if (r != 1) return 135;
  r = (b200 <= 0);             if (r != 0) return 136;
  r = (b200 >  0);             if (r != 1) return 137;
  r = (b0   >= 0);             if (r != 1) return 138;
  return 0;
}

// 141 - 150 : int, right side is not direct.
int t_int(void) {
  int i5 = 5, i3 = 3, im1 = -1, i0 = 0;
  int big = 32767, small = -32768;
  int r;

  r = (i5 == iid(i5));         if (r != 1) return 141;
  r = (i5 == iid(i3));         if (r != 0) return 142;
  r = (i0 == iid(i0));         if (r != 1) return 143;
  r = (i5 != iid(i5));         if (r != 0) return 144;
  r = (i3 <  iid(i5));         if (r != 1) return 145;
  r = (im1 < iid(i0));         if (r != 1) return 146;
  r = (small < iid(big));      if (r != 1) return 147;
  r = (i5 >= iid(i5));         if (r != 1) return 148;
  return 0;
}

// 151 - 160 : int, byte boundary. hi has a zero low byte.
int t_int_bytes(void) {
  int hi = 0x0100, lo = 0x00ff;
  int r;

  r = (hi == iid(hi));         if (r != 1) return 151;
  r = (hi == iid(lo));         if (r != 0) return 152;
  r = (lo == iid(lo));         if (r != 1) return 153;
  r = (hi != iid(lo));         if (r != 1) return 154;
  r = (lo != iid(lo));         if (r != 0) return 155;
  r = (lo <  iid(hi));         if (r != 1) return 156;
  r = (hi <= iid(lo));         if (r != 0) return 157;
  r = (hi >  iid(lo));         if (r != 1) return 158;
  r = (lo >= iid(hi));         if (r != 0) return 159;
  return 0;
}

// 161 - 170 : int against 0.
int t_zero_int(void) {
  int i5 = 5, im1 = -1, i0 = 0;
  int hi = 0x0100;
  int r;

  r = (i0  == 0);              if (r != 1) return 161;
  r = (i5  == 0);              if (r != 0) return 162;
  r = (hi  == 0);              if (r != 0) return 163;
  r = (i0  != 0);              if (r != 0) return 164;
  r = (hi  != 0);              if (r != 1) return 165;
  r = (im1 <  0);              if (r != 1) return 166;
  r = (i0  <= 0);              if (r != 1) return 167;
  r = (i5  >  0);              if (r != 1) return 168;
  r = (im1 >= 0);              if (r != 0) return 169;
  return 0;
}

// 171 - 180 : unsigned int against 0.
int t_zero_uint(void) {
  unsigned int u0 = 0, u1 = 1;
  int r;

  r = (u0 == 0);               if (r != 1) return 171;
  r = (u1 != 0);               if (r != 1) return 172;
  r = (u0 != 0);               if (r != 0) return 173;
  r = (u0 <= 0);               if (r != 1) return 174;
  r = (u1 <= 0);               if (r != 0) return 175;
  r = (u1 >  0);               if (r != 1) return 176;
  r = (u1 <  0);               if (r != 0) return 177;
  r = (u0 >= 0);               if (r != 1) return 178;
  return 0;
}

// 181 - 190 : unsigned int. a signed compare gives the wrong answer here.
int t_uint(void) {
  unsigned int u1 = 1, u0 = 0;
  unsigned int ubig = (unsigned int)0xffff;
  unsigned int uhi  = (unsigned int)0x8000;
  int r;

  r = (u1   == uiid(u1));      if (r != 1) return 181;
  r = (ubig == uiid(ubig));    if (r != 1) return 182;
  r = (u1   != uiid(ubig));    if (r != 1) return 183;
  r = (u1   <  uiid(ubig));    if (r != 1) return 184;
  r = (ubig <  uiid(u1));      if (r != 0) return 185;
  r = (uhi  >  uiid(u1));      if (r != 1) return 186;
  r = (u0   <= uiid(u0));      if (r != 1) return 187;
  r = (ubig >= uiid(uhi));     if (r != 1) return 188;
  return 0;
}

// 201 - 210 : some helpers read the carry flag. It must come from the
// compare itself, not from an older instruction.
int t_carry_dirty(void) {
  s8 a5 = 5, z = 0;
  u8 b5 = 5, b3 = 3, b0 = 0;
  int i0 = 0;
  unsigned int u1 = 1, ubig = (unsigned int)0xffff;
  int r;

  asm(" sec"); r = (a5 == sid(a5));    if (r != 1) return 201;
  asm(" clc"); r = (a5 == sid(a5));    if (r != 1) return 202;
  asm(" sec"); r = (b3 <  uid(b5));    if (r != 1) return 203;
  asm(" clc"); r = (b3 <  uid(b5));    if (r != 1) return 204;
  asm(" sec"); r = (b5 >= uid(b3));    if (r != 1) return 205;
  asm(" sec"); r = (z  == 0);          if (r != 1) return 206;
  asm(" clc"); r = (b0 <= 0);          if (r != 1) return 207;
  asm(" sec"); r = (i0 == 0);          if (r != 1) return 208;
  asm(" sec"); r = (u1 <  uiid(ubig)); if (r != 1) return 209;
  return 0;
}

// 221 - 230 : mixed signs. C changes both sides to int first,
// so -1 is still less than 200.
int t_mixed_sign(void) {
  s8 s_m1 = -1, s_1 = 1;
  u8 u_200 = 200, u_1 = 1;
  int r;

  r = (s_m1  <  u_200);        if (r != 1) return 221;
  r = (s_m1  <  u_1);          if (r != 1) return 222;
  r = (u_200 >  s_m1);         if (r != 1) return 223;
  r = (s_m1  <= u_200);        if (r != 1) return 224;
  r = (s_m1  >= u_200);        if (r != 0) return 225;
  r = (s_1   <  u_200);        if (r != 1) return 226;
  r = (s_m1  == u_200);        if (r != 0) return 227;
  r = (s_m1  != u_200);        if (r != 1) return 228;
  return 0;
}

int main(void) {
  int r;

  if ((r = t_s8_call())     != 0) return r;
  if ((r = t_s8_edge())     != 0) return r;
  if ((r = t_u8_call())     != 0) return r;
  if ((r = t_u8_call2())    != 0) return r;
  if ((r = t_sba_s8())      != 0) return r;
  if ((r = t_sba_s8b())     != 0) return r;
  if ((r = t_sba_u8())      != 0) return r;
  if ((r = t_sba_u8b())     != 0) return r;
  if ((r = t_sba_expr())    != 0) return r;
  if ((r = t_direct_s8())   != 0) return r;
  if ((r = t_direct_u8())   != 0) return r;
  if ((r = t_zero_s8())     != 0) return r;
  if ((r = t_zero_u8())     != 0) return r;
  if ((r = t_int())         != 0) return r;
  if ((r = t_int_bytes())   != 0) return r;
  if ((r = t_zero_int())    != 0) return r;
  if ((r = t_zero_uint())   != 0) return r;
  if ((r = t_uint())        != 0) return r;
  if ((r = t_carry_dirty()) != 0) return r;
  if ((r = t_mixed_sign())  != 0) return r;

  return 0;
}
