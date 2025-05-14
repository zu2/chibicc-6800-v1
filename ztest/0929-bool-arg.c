#include <stdint.h>
#include <stdbool.h>

int sub(char *a, bool b, int c)
{
  return b;
}

int main(int argc, char **argv)
{
  return sub("aaa",true,0)!=1;
}
