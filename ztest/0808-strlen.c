#include "common.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *input;
  size_t expected;
} TestCase;

int main(void)
{
  static const TestCase tests[] = {
      {"", 0},
      {"a", 1},
      {"abc", 3},
      {"Hello, world!", 13},
      {"C programming", 13},
      {" ", 1},
      {"1234567890", 10},
      {"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",300},
      {NULL, 0}
  };

  for (int i = 0; tests[i].input != NULL; i++) {
    size_t result = strlen(tests[i].input);
    if (result != tests[i].expected) {
      return i + 1; 
    }
  }

  putstr("All tests passed.\n");
  return 0;
}
