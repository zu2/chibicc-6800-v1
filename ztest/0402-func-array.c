#include "common.h"

typedef void (*FuncPtr)(void);

int var=1;

static void func_a(void) {
	var++;
	putstr("func_a:");
	print(var);
}
static void func_b(void) {
	var--;
	putstr("func_b:");
	print(var);
}

FuncPtr table[] = { func_a, func_b };

int main(int argc, char **argv)
{

  (*table[0])();
  (*table[1])();
  (*table[1])();

  return var;
}
