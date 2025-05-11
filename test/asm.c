#include "test.h"

char *asm_fn1(void) {
  asm("ldab #50\n\t"
      "ldaa #0\n\t"
      "rts");
}

char *asm_fn2(void) {
  asm inline volatile("ldab #55\n\t"
                      "ldaa #0\n\t"
                      "rts");
}

int main() {
  ASSERT(50, asm_fn1());
  ASSERT(55, asm_fn2());

  printf("OK\n");
  return 0;
}
