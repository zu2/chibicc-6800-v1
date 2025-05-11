// string.h for chibicc-6800-v1 C compiler
// Basic string and memory functions

#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h> // for size_t

// Copy n bytes from src to dest (memory copy)
void *memcpy(void *dest, const void *src, size_t n);

// Set n bytes of s to c (memory set)
void *memset(void *s, int c, size_t n);

// Compare n bytes of s1 and s2 (memory compare)
int memcmp(const void *s1, const void *s2, size_t n);
//
// Move  n bytes of s1 and s2 (memory move)
void *memmove(void *s1, const void *s2, size_t n);

// Copy src string to dest (including '\0')
char *strcpy(char *dest, const char *src);

// Copy up to n chars from src to dest
//char *strncpy(char *dest, const char *src, size_t n);

// Concatenate src to dest
char *strcat(char *dest, const char *src);

// Concatenate up to n chars from src to dest
//char *strncat(char *dest, const char *src, size_t n);

// Compare two strings
int strcmp(const char *s1, const char *s2);

// Compare up to n chars of two strings
int strncmp(const char *s1, const char *s2, size_t n);

// Get length of string (not including '\0')
size_t strlen(const char *s);

// Find first occurrence of c in s
char *strchr(const char *s, int c);

// Find last occurrence of c in s
//char *strrchr(const char *s, int c);

// Find first occurrence of needle in haystack
//char *strstr(const char *haystack, const char *needle);

// Find first occurrence of any char from accept in s
//char *strpbrk(const char *s, const char *accept);

// Get length of initial segment containing only chars in accept
//size_t strspn(const char *s, const char *accept);

// Get length of initial segment containing no chars in reject
//size_t strcspn(const char *s, const char *reject);

// Tokenize string (not thread-safe)
//char *strtok(char *str, const char *delim);

#endif // _STRING_H_

