//
// test lowzero
// constants whose low byte is 0
//
unsigned int  u;
int           i;
unsigned long ul;

// comparison
int t1(void) { u = 300; asm(" sec"); return u >= 256; }   /* 1 */
int t2(void) { u = 300; asm(" clc"); return u >= 256; }   /* 1 */
int t3(void) { i = 300; asm(" sec"); return i >= 256; }   /* 1 */
int t4(void) { i = 300; asm(" clc"); return i >= 256; }   /* 1 */
int t5(void) { u = 100; asm(" sec"); return u <  256; }   /* 1 */
int t6(void) { u = 100; asm(" clc"); return u <  256; }   /* 1 */

// arithmetic
int t7(void)  { u = 300; asm(" sec"); return u - 256; }   /* 44 */
int t8(void)  { u = 300; asm(" clc"); return u - 256; }   /* 44 */
int t9(void)  { u = 300; asm(" sec"); return u + 256; }   /* 556 */
int t10(void) { u = 300; asm(" clc"); return u + 256; }   /* 556 */

//
int t11(void) { u = 0x1234; asm(" sec"); return u - 0x1000; }  /* 0x234 */
int t12(void) { u = 0x1234; asm(" clc"); return u - 0x1000; }  /* 0x234 */

int t13(void) { i = 256; return i >  0; }   /* 1 */
int t14(void) { i = 256; return i <= 0; }   /* 0 */
int t15(void) { i = 0;   return i >  0; }   /* 0 */
int t16(void) { i = 0;   return i <= 0; }   /* 1 */
int t17(void) { i = 5;   return i >  0; }   /* 1 */
int t18(void) { i = -256;return i <= 0; }   /* 1 */

int main(void)
{
  if (t1() != 1) return 1;
  if (t2() != 1) return 2;
  if (t3() != 1) return 3;
  if (t4() != 1) return 4;
  if (t5() != 1) return 5;
  if (t6() != 1) return 6;
  if (t7()  != 44)  return 7;
  if (t8()  != 44)  return 8;
  if (t9()  != 556) return 9;
  if (t10() != 556) return 10;
  if (t11() != 0x234) return 11;
  if (t12() != 0x234) return 12;
  if (t13() != 1) return 13;
  if (t14() != 0) return 14;
  if (t15() != 0) return 15;
  if (t16() != 1) return 16;
  if (t17() != 1) return 17;
  if (t18() != 1) return 18;

  return 0;
}
