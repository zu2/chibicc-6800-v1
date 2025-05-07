// stdlib.h for chibicc-6800-v1 C compiler
// Standard library: memory allocation, conversions, random, etc.

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h> // for size_t

#define RAND_MAX 32767

// Convert string to int
int atoi(const char *nptr);

// Convert string to long
long atol(const char *nptr);

// Convert string to int (base 10 or autodetect)
int strtol(const char *nptr, char **endptr, int base);

// Convert string to unsigned long
unsigned long strtoul(const char *nptr, char **endptr, int base);

// Absolute value of int
int abs(int n);

// Absolute value of long
long labs(long n);

// Pseudo-random number generator
int rand(void);

// Seed for rand()
void srand(unsigned int seed);

// Memory allocation (if supported)
//void *malloc(size_t size);
//void free(void *ptr);
//void *calloc(size_t nmemb, size_t size);
//void *realloc(void *ptr, size_t size);

// Program termination
void exit(int status);

// Program abort (abnormal termination)
void abort(void);

// Environment support (may be stubbed)
//char *getenv(const char *name);

// Sorting and searching
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

char *itoa(int value, char *buffer, int radix);
char *ltoa(long value, char *buffer, int radix);
#endif // _STDLIB_H_
