//
// bool conversion test.
//
unsigned char c;
int i;
long l;
char *p;
_Bool b;

// && and || yield int 0 or 1 (C99 6.5.13p3, 6.5.14p3)
int t1(void) { c = 3; return 1 && c; }          // 1
int t2(void) { c = 0; return 1 && c; }          // 0
int t3(void) { c = 3; return 0 || c; }          // 1
int t4(void) { c = 0; return 0 || c; }          // 0
int t5(void) { i = 256; return 1 && i; }        // 1
int t6(void) { i = 0; return 0 || i; }          // 0
int t7(void) { l = 65536; return 1 && l; }      // 1
int t8(void) { p = (char *)&c; return 1 && p; } // 1
int t9(void) { p = 0; return 0 || p; }          // 0

// scalar to _Bool normalizes to 0 or 1 (C99 6.3.1.2)
int t10(void) { c = 3; return (_Bool)c; }       // 1
int t11(void) { i = 256; return (_Bool)i; }     // 1
int t12(void) { l = 65536; return (_Bool)l; }   // 1
int t13(void) { p = (char *)&c; return (_Bool)p; } // 1
int t14(void) { c = 3; b = c; return b; }       // 1
int t15(void) { c = 0; b = c; return b; }       // 0
int t16(void) { c = 3; b = c; return b ? 10 : 20; } // 10
int t17(void) { c = 3; _Bool x = c; return x; } // 1
int t18(void) { c = 2; b = (c < 3); return b; } // 1

// control
int t19(void) { c = 200; return c; }            // 200, zero-extended
int t20(void) { c = 3; return !!c; }            // 1
int t21(void) { c = 3; return c && 1; }         // 1

int main(int argc, char **argv)
{
  if (t1() != 1) return 1;
  if (t2() != 0) return 2;
  if (t3() != 1) return 3;
  if (t4() != 0) return 4;
  if (t5() != 1) return 5;
  if (t6() != 0) return 6;
  if (t7() != 1) return 7;
  if (t8() != 1) return 8;
  if (t9() != 0) return 9;
  if (t10() != 1) return 10;
  if (t11() != 1) return 11;
  if (t12() != 1) return 12;
  if (t13() != 1) return 13;
  if (t14() != 1) return 14;
  if (t15() != 0) return 15;
  if (t16() != 10) return 16;
  if (t17() != 1) return 17;
  if (t18() != 1) return 18;
  if (t19() != 200) return 19;
  if (t20() != 1) return 20;
  if (t21() != 1) return 21;

  return 0;
}
