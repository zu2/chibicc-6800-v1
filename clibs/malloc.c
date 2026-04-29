#include <string.h>
#include <malloc.h>

struct block {
    size_t size;
};

#define ALIGN_2BYTE(x)      (((x) + 1) & ~1)
#define USER_SIZE(sz)       ((sz) & ~1)
#define HEADER_FROM_PTR(p)  ((struct block *)((char *)(p) - sizeof(struct block)))
#define DATA_FROM_HEADER(h) ((void *)((char *)(h) + sizeof(struct block)))
#define IS_FREE(h)          (((h)->size & 1) != 0)
#define SET_FREE(h)         ((h)->size |= 1)
#define SET_ALLOCATED(h)    ((h)->size &= ~1)
#define IS_SENTINEL(h)      ((h)->size == 0)


static struct block *heap = NULL;

static void *heap_base(void)
{
  asm("\tldab #<__bss");
  asm("\tldaa #>__bss");
  asm("\taddb #__bss_size");
  asm("\tadca #__bss_size");
}

static void malloc_init(void) {
    heap = (struct block *)heap_base();
    heap->size = 0;
}

void *malloc(size_t size) {
    if (size == 0) return NULL;
    if (!heap) malloc_init();

    size_t required_total = ALIGN_2BYTE(ALIGN_2BYTE(size) + sizeof(struct block));

    struct block *cur = heap;

    while (!IS_SENTINEL(cur)) {
        if (IS_FREE(cur) && USER_SIZE(cur->size) >= required_total) {
            SET_ALLOCATED(cur);
            return DATA_FROM_HEADER(cur);
        }
        cur = (struct block *)((char *)cur + USER_SIZE(cur->size));
    }

    cur->size = required_total;
    struct block *sentinel = (struct block *)((char *)cur + required_total);
    sentinel->size = 0;
    return DATA_FROM_HEADER(cur);
}

void *calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;
    void *ptr = malloc(nmemb * size);
    if (!ptr) return NULL;
    memset(ptr, 0, nmemb * size);
    return ptr;
}

void *realloc(void *ptr, size_t new_size) {
    if (new_size == 0) { free(ptr); return NULL; }
    if (!ptr) return malloc(new_size);

    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;

    struct block *cur = HEADER_FROM_PTR(ptr);
    size_t old_size = USER_SIZE(cur->size) - sizeof(struct block);
    memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
    free(ptr);
    return new_ptr;
}

void free(void *ptr) {
    if (!ptr || !heap) return;

    struct block *cur = HEADER_FROM_PTR(ptr);
    if (IS_FREE(cur)) return;

    SET_FREE(cur);

    struct block *next = (struct block *)((char *)cur + USER_SIZE(cur->size));
    while (IS_FREE(next)) {
        cur->size = USER_SIZE(cur->size) + USER_SIZE(next->size);
        next = (struct block *)((char *)cur + USER_SIZE(cur->size));
    }
}
