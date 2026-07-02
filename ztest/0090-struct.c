//
// Tests struct return in the compiler
//
// Each call must return its own struct value.
// Returned values must not be affected by other active calls,
// including nested calls and recursion.
//
// C standard (C17):
// - 6.8.6.4 Return statement:
//   "A return statement with an expression shall evaluate the expression
//    and return its value."
// - 6.5 Expressions:
//   "The value of an expression is determined by its evaluation."
//
// chibicc-6800:
// Uses caller-allocated return buffer (hidden pointer).
// This test checks that the buffer is not reused across calls.
//

struct complex {
  int r;
  int i;
};

struct large_complex {
  int a[10];
  int r;
  int i;
  int b[10];
};

struct complex sub(int a, int b)
{
  struct complex com_1;

  com_1.r = a;
  com_1.i = b;

  return com_1;
}

struct large_complex large_sub(int a, int b)
{
  struct large_complex com_1;

  com_1.r = a;
  com_1.i = b;

  return com_1;
}

struct S {
  unsigned char a,b,c,sum;
};

struct S make_s(unsigned char n)
{
  struct S s;

  s.a = n;
  s.b = n^0xa5;
  s.c = (n<<1) | (n>>7);
  s.sum = s.a + s.b + s.c;

  return s;
}

int test_s(struct S s, unsigned char n)
{
  if (s.a == n
  &&  s.b == (n^0xa5)
  &&  s.c == ((n<<1) | (n>>7))
  &&  s.sum == (unsigned char)(s.a + s.b + s.c)) {
    return 1;
  }
  return 0;
}

struct S combine(struct S x, struct S y) {
  if (!test_s(x, 1)) {
    x.a = 0xEE;
  }

  struct S r;
  r.a = x.a ^ y.a;
  r.b = x.b ^ y.b;
  r.c = x.c ^ y.c;
  r.sum = r.a + r.b + r.c;

  return r;
}

int test_nested(void) {
  struct S r = combine(make_s(1), make_s(2));

  if (!test_s(make_s(1), 1)) return 1;

  return (r.a == 0xEE) ? 2 : 0;
}

int test_recursive(unsigned char n, int depth)
{
  struct S r = make_s(n);

  if (!test_s(r, n))
    return 1;

  if (depth > 0) {
    if (test_recursive(n + 1, depth - 1))
      return 2;
  }

  if (!test_s(r, n))
    return 3;

  return 0;
}

int
main(int argc, char **argv)
{
  struct complex r0;
  struct S r1,r2;
  struct S r3;
  struct large_complex r4;
  int ret;

  r0 = sub(0x1234,0x5678);

  if (r0.r != 0x1234) {
    return 1;
  }
  if (r0.i != 0x5678) {
    return 2;
  }

  r1 = make_s(10);
  r2 = make_s(20);

  if (!test_s(r1,10))
    return 11;
  if (!test_s(r2,20))
    return 12;

  r3 = combine(make_s(1), make_s(2));
  if (!test_s(make_s(1),1))
    return 21;
  if (r3.a == 0xEE) {
    return 22;
  }

  if (test_recursive(0,3)) {
    return 30;
  }
  if (test_recursive(1,3)) {
    return 31;
  }
  if (test_recursive(2,3)) {
    return 32;
  }
  if (test_recursive(3,3)) {
    return 33;
  }

  r4 = large_sub(0x1234,0x5678);

  if (r4.r != 0x1234) {
    return 1;
  }
  if (r4.i != 0x5678) {
    return 2;
  }

  return 0;
}
