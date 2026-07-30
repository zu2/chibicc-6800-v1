//
// compare & carry test
//
// A compare against 0 may be emitted as "ldab lo; ldaa hi; jsr __xx16".
//
// Loads do not touch C, so C still holds whatever the previous code left.
// Helpers like __ge16u read C.
//
// Each pair differs only in the carry. Both members must return the same value.
//
unsigned int   u;
unsigned long  ul;
unsigned char  uc;
int            i;

// 1. carry dependence, unsigned int
int t1(void) { u = 5; asm(" sec"); return u >= 0; }   /* 1 */
int t2(void) { u = 5; asm(" clc"); return u >= 0; }   /* 1 */
int t3(void) { u = 5; asm(" sec"); return u <  0; }   /* 0 */
int t4(void) { u = 5; asm(" clc"); return u <  0; }   /* 0 */
int t5(void) { u = 5; asm(" sec"); return u >  0; }   /* 1 */
int t6(void) { u = 5; asm(" clc"); return u >  0; }   /* 1 */
int t7(void) { u = 5; asm(" sec"); return u <= 0; }   /* 0 */
int t8(void) { u = 5; asm(" clc"); return u <= 0; }   /* 0 */

// 2. sign bit set. wrong even if C is correct
int t9(void)  { u = 0x8000; asm(" clc"); return u >= 0; }  /* 1 */
int t10(void) { u = 0x8000; asm(" clc"); return u <  0; }  /* 0 */
int t11(void) { u = 0x8000; asm(" clc"); return u >  0; }  /* 1 */

// 3. other widths
int t12(void) { ul = 5;  asm(" sec"); return ul >= 0; }  /* 1 */
int t13(void) { ul = 5;  asm(" clc"); return ul >= 0; }  /* 1 */
int t14(void) { uc = 5;  asm(" sec"); return uc >= 0; }  /* 1 */
int t15(void) { uc = 5;  asm(" clc"); return uc >= 0; }  /* 1 */
int t16(void) { ul = 0x80000000; asm(" clc"); return ul >= 0; }  /* 1 */
int t17(void) { uc = 0x80; asm(" clc"); return uc >= 0; }        /* 1 */

// 4. control: BLT / BGE read N and V, not C
int c1(void) { i =  5; asm(" sec"); return i >= 0; }  /* 1 */
int c2(void) { i =  5; asm(" clc"); return i >= 0; }  /* 1 */
int c3(void) { i = -5; asm(" sec"); return i <  0; }  /* 1 */
int c4(void) { i = -5; asm(" clc"); return i <  0; }  /* 1 */

// 5. control: a real subtraction should set C itself
int c5(void) { u = 5; asm(" sec"); return u == 0; }   /* 0 */
int c6(void) { u = 5; asm(" clc"); return u == 0; }   /* 0 */
int c7(void) { u = 5; asm(" sec"); return u >= 3; }   /* 1 */
int c8(void) { u = 5; asm(" clc"); return u >= 3; }   /* 1 */

int main(void)
{
  if (t1()  != 1) return 1;
  if (t2()  != 1) return 2;
  if (t3()  != 0) return 3;
  if (t4()  != 0) return 4;
  if (t5()  != 1) return 5;
  if (t6()  != 1) return 6;
  if (t7()  != 0) return 7;
  if (t8()  != 0) return 8;
  if (t9()  != 1) return 9;
  if (t10() != 0) return 10;
  if (t11() != 1) return 11;
  if (t12() != 1) return 12;
  if (t13() != 1) return 13;
  if (t14() != 1) return 14;
  if (t15() != 1) return 15;
  if (t16() != 1) return 16;
  if (t17() != 1) return 17;
  if (c1()  != 1) return 21;
  if (c2()  != 1) return 22;
  if (c3()  != 1) return 23;
  if (c4()  != 1) return 24;
  if (c5()  != 0) return 25;
  if (c6()  != 0) return 26;
  if (c7()  != 1) return 27;
  if (c8()  != 1) return 28;
  return 0;
}
