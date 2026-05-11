#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef __CHIBICC__
#include <locale.h>
#endif

static int isdigit_spec(unsigned char c) { return c >= '0' && c <= '9'; }

static int islower_spec(unsigned char c) { return c >= 'a' && c <= 'z'; }

static int isupper_spec(unsigned char c) { return c >= 'A' && c <= 'Z'; }

static int isalpha_spec(unsigned char c)
{
  return islower_spec(c) || isupper_spec(c);
}

static int isalnum_spec(unsigned char c)
{
  return isalpha_spec(c) || isdigit_spec(c);
}

static int isxdigit_spec(unsigned char c)
{
  return isdigit_spec(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static int isspace_spec(unsigned char c)
{
  return (c == ' ' || (c >= 0x09 && c <= 0x0D));
}

static int isblank_spec(unsigned char c) { return (c == ' ' || c == '\t'); }

static int iscntrl_spec(unsigned char c) { return (c <= 0x1F || c == 0x7F); }

static int isprint_spec(unsigned char c) { return (c >= 0x20 && c <= 0x7E); }

static int ispunct_spec(unsigned char c)
{
  return isprint_spec(c) && !isalnum_spec(c) && c != ' ';
}

static unsigned char tolower_spec(unsigned char c)
{
  return (c >= 'A' && c <= 'Z') ? (c + 0x20) : c;
}

static unsigned char toupper_spec(unsigned char c)
{
  return (c >= 'a' && c <= 'z') ? (c - 0x20) : c;
}

int test_predicate(const char *name, int (*fn)(int), int (*spec)(unsigned char))
{
  int ok = 1;

  for (unsigned int i = 0; i <= 255; i++) {
    unsigned char c = (unsigned char)i;

    int got = fn(c) != 0;
    int exp = spec(c) != 0;

    if (got != exp) {
      printf("FAIL %s %02X -> %d (exp:%d)\n", name, c, got, exp);
      ok = 0;
    }
  }
  if (fn(256)) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 256, fn(256), 0);
    ok = 0;
  }
  if (fn(32767)) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 32767, fn(32767), 0);
    ok = 0;
  }
  if (fn(0x8000)) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 0x8000, fn(0x8000), 0);
    ok = 0;
  }
  if (fn(0xffff)) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 0xffff, fn(0xffff), 0);
    ok = 0;
  }

  if (ok) {
    printf("PASS %s\n", name);
  }

  return ok;
}

int test_transform(const char *name, int (*fn)(int),
                   unsigned char (*spec)(unsigned char))
{
  int ok = 1;

  for (int i = 0; i <= 255; i++) {
    unsigned char c = (unsigned char)i;

    unsigned char got = (unsigned char)fn(c);
    unsigned char exp = spec(c);

    if (got != exp) {
      printf("FAIL %s %02X -> %02X (exp:%02X)\n", name, c, got, exp);
      ok = 0;
    }
  }
// Undefined for non-unsigned-char/non-EOF;
// this test expects the input unchanged.
  if (fn(256)!=256) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 256, fn(256), 0);
    ok = 0;
  }
  if (fn(32767)!=32767) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 32767, fn(32767), 0);
    ok = 0;
  }
  if (fn(0x8000)!=0x8000) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 0x8000, fn(0x8000), 0);
    ok = 0;
  }
  if (fn(0xffff)!=0xffff) {
    printf("FAIL %s %04X -> %d (exp:%d)\n", name, 0xffff, fn(0xffff), 0);
    ok = 0;
  }

  if (ok) {
    printf("PASS %s\n", name);
  }

  return ok;
}

int main(int argc, char **argv)
{
#ifndef __CHIBICC__
  setlocale(LC_CTYPE, "C");
#endif

  if (!test_predicate("isdigit", isdigit, isdigit_spec)) {
    return 1;
  }
  if (!test_predicate("islower", islower, islower_spec)) {
    return 1;
  }
  if (!test_predicate("isupper", isupper, isupper_spec)) {
    return 1;
  }
  if (!test_predicate("isalpha", isalpha, isalpha_spec)) {
    return 1;
  }
  if (!test_predicate("isalnum", isalnum, isalnum_spec)) {
    return 1;
  }
  if (!test_predicate("isxdigit", isxdigit, isxdigit_spec)) {
    return 1;
  }
  if (!test_predicate("isspace", isspace, isspace_spec)) {
    return 1;
  }
  if (!test_predicate("isblank", isblank, isblank_spec)) {
    return 1;
  }
  if (!test_predicate("isprint", isprint, isprint_spec)) {
    return 1;
  }
  if (!test_predicate("ispunct", ispunct, ispunct_spec)) {
    return 1;
  }
  if (!test_predicate("iscntrl", iscntrl, iscntrl_spec)) {
    return 1;
  }

  if (!test_transform("tolower", tolower, tolower_spec)) {
    return 1;
  }
  if (!test_transform("toupper", toupper, toupper_spec)) {
    return 1;
  }

  return 0;
}
