#include <string.h>
#include <stdint.h>

extern void print(int x);

uint8_t *sub(uint8_t *str,size_t size)
{
    uint8_t buf[256];
    uint8_t *s = str;

//  s = s + size;	// It's OK
    s += size;

    return s;
}


int main(int argc, char **argv)
{
    uint8_t p = 0;

    if (sub(&p,10)!=(&p+10))
      return 1;

    return 0;
}
