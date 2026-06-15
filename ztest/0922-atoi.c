#include "common.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  if (atoi("+0") != 0) {
    return 1;
  }
  if (atoi("-0") != 0) {
    return 2;
  }
  if (atoi("1") != 1) {
    return 3;
  }
  if (atoi("-1") != -1) {
    return 4;
  }
  if (atoi("12345") != 12345) {
    return 5;
  }
  if (atoi("-12345") != -12345) {
    return 6;
  }
  if (atoi("32767") != 32767) {
    return 7;
  }
  if (atoi("-32767") != -32767) {
    return 8;
  }
  if (atoi("32768") != -32768) {
    return 9;
  }
  if (atoi("-32768") != -32768) {
    return 10;
  }

  if (atoi("123abc") != 123) {
    return 31;
  }
  if (atoi("123 456") != 123) {
    return 32;
  }
  if (atoi("123+456") != 123) {
    return 33;
  }

  if (atoi("abc") != 0) {
    return 41;
  }
  if (atoi("+") != 0) {
    return 42;
  }
  if (atoi("-") != 0) {
    return 43;
  }
  if (atoi("+ 123") != 0) {
    return 44;
  }
  if (atoi("++123") != 0) {
    return 45;
  }
  if (atoi("--123") != 0) {
    return 46;
  }
  if (atoi("+-123") != 0) {
    return 46;
  }
  if (atoi("-+123") != 0) {
    return 46;
  }

  if (atoi("-42abc") != -42) {
    return 51;
  }
  if (atoi("+0") != 0) {
    return 52;
  }
  if (atoi(" + 0") != 0) {
    return 53;
  }
  if (atoi("-00123") != -123) {
    return 54;
  }

  if (atoi("") != 0) {
    return 61;
  }
  if (atoi(" ") != 0) {
    return 62;
  }
  if (atoi("\n\t123") != 123) {
    return 63;
  }

  return 0;
}
