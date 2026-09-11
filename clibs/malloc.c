#include <stdlib.h>
#include <string.h>

//
//  allocation block size: multiple of 2
//    size's b0: 0:used, 1:free
//    size == 0: sentinel, end of blocks
//
struct block {
  size_t size;
};

#define BLOCK_SIZE(sz) ((sz) & ~1)

static struct block *heap = NULL;

static void *heap_base(void)
{
  asm("\tldab #<__bss");
  asm("\tldaa #>__bss");
  asm("\taddb #<__bss_size");
  asm("\tadca #>__bss_size");
}

static void *stack_pointer(void)
{
  asm("\tsts @tmp1");
  asm("\tldab @tmp1+1");
  asm("\tldaa @tmp1");
}

static void malloc_init(void)
{
  heap = (struct block *)heap_base();

  heap->size = 0;
}

void *malloc(size_t size)
{
  if (size == 0 || size > 0xFFFB) {
    return NULL;
  }
  if (!heap) {
    malloc_init();
  }

  size_t required_total = ((size + 1) & ~1) + sizeof(struct block);

  struct block *bp = heap;

  while (bp->size) {       // !sentinel ?
    if ((bp->size & 1)     // free?
    &&  BLOCK_SIZE(bp->size) >= required_total) {  // enough size?

      bp->size &= ~1;    // used flag

      return (void *)((char *)bp + sizeof(struct block));
    }
    bp = (struct block *)((char *)bp + BLOCK_SIZE(bp->size));  // next block
  }

  // can't find free block
  size_t sp = (size_t)stack_pointer();
  if (sp<1024)  return NULL;

  size_t limit = sp - 1024;

  if (limit < (size_t)bp
  ||  limit - (size_t)bp < required_total) {
    return NULL;
  }

  // use sentinel block as new block
  bp->size = required_total;

  // allocate new sentinel
  struct block *sentinel = (struct block *)((char *)bp + required_total);
  sentinel->size = 0;

  return (void *)((char *)bp + sizeof(struct block));
}

void *calloc(size_t nmemb, size_t size)
{
  if (nmemb == 0 || size == 0) {
    return NULL;
  }
  if ((nmemb | size) >= 256 && size > 0xFFFF / nmemb) {
    return NULL;
  }

  size_t n = nmemb * size;
  void *p = malloc(n);

  if (!p) {
    return NULL;
  }
  memset(p, 0, n);
  return p;
}

void *realloc(void *p, size_t new_size)
{
  if (new_size == 0) {
    free(p);
    return NULL;
  }
  if (!p) {
    return malloc(new_size);
  }

  void *new_p = malloc(new_size);
  if (!new_p) {
    return NULL;
  }

  struct block *bp = (struct block *)((char *)p - sizeof(struct block));
  size_t old_size = BLOCK_SIZE(bp->size) - sizeof(struct block);

  memcpy(new_p, p, old_size < new_size ? old_size : new_size);
  free(p);

  return new_p;
}

void free(void *p)
{
  if (!p || !heap) {
    return;
  }

  struct block *bp = (struct block *)((char *)p - sizeof(struct block));

  if (bp->size & 1) { // already free
    return;
  }

  bp->size |= 1;      // set flag free.

  struct block *next = (struct block *)((char *)bp + BLOCK_SIZE(bp->size));

  // append next free block
  while (next->size & 1) {
    bp->size = BLOCK_SIZE(bp->size) + BLOCK_SIZE(next->size);
    next = (struct block *)((char *)bp + BLOCK_SIZE(bp->size));
  }
}
