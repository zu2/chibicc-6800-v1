#include <ctype.h>
#include <stdio.h>
#ifndef __CHIBICC__
#include <locale.h>
#endif

static int is_digit_spec(unsigned char c) { return c >= '0' && c <= '9'; }

static int is_lower_spec(unsigned char c) { return c >= 'a' && c <= 'z'; }

static int is_upper_spec(unsigned char c) { return c >= 'A' && c <= 'Z'; }

static int is_alpha_spec(unsigned char c)
{
  return is_lower_spec(c) || is_upper_spec(c);
}

static int is_alnum_spec(unsigned char c)
{
  return is_alpha_spec(c) || is_digit_spec(c);
}

static int is_xdigit_spec(unsigned char c)
{
  return is_digit_spec(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static int is_space_spec(unsigned char c)
{
  return (c == ' ' || (c >= 0x09 && c <= 0x0D));
}

static int is_blank_spec(unsigned char c) { return (c == ' ' || c == '\t'); }

static int is_cntrl_spec(unsigned char c) { return (c <= 0x1F || c == 0x7F); }

static int is_print_spec(unsigned char c) { return (c >= 0x20 && c <= 0x7E); }

static int is_punct_spec(unsigned char c)
{
  return is_print_spec(c) && !is_alnum_spec(c) && c != ' ';
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

  for (int i = 0; i <= 255; i++) {
    unsigned char c = (unsigned char)i;

    int got = fn(c) != 0;
    int exp = spec(c) != 0;

    if (got != exp) {
      printf("FAIL %s %02X -> %d (exp:%d)\n", name, c, got, exp);
      ok = 0;
    }
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

  if (!test_predicate("isdigit", isdigit, is_digit_spec)) {
    return 1;
  }
  if (!test_predicate("islower", islower, is_lower_spec)) {
    return 1;
  }
  if (!test_predicate("isupper", isupper, is_upper_spec)) {
    return 1;
  }
  if (!test_predicate("isalpha", isalpha, is_alpha_spec)) {
    return 1;
  }
  if (!test_predicate("isalnum", isalnum, is_alnum_spec)) {
    return 1;
  }
  if (!test_predicate("isxdigit", isxdigit, is_xdigit_spec)) {
    return 1;
  }
  if (!test_predicate("isspace", isspace, is_space_spec)) {
    return 1;
  }
  if (!test_predicate("isblank", isblank, is_blank_spec)) {
    return 1;
  }
  if (!test_predicate("isprint", isprint, is_print_spec)) {
    return 1;
  }
  if (!test_predicate("ispunct", ispunct, is_punct_spec)) {
    return 1;
  }
  if (!test_predicate("iscntrl", iscntrl, is_cntrl_spec)) {
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
