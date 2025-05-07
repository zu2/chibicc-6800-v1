// stddef.h for MC6800 C compiler
// Defines size_t, ptrdiff_t, NULL, and offsetof

#ifndef _STDDEF_H_
#define _STDDEF_H_

// size_t: unsigned integer type for sizes (16bit)
typedef unsigned int size_t;

// ptrdiff_t: signed integer type for pointer differences (16bit)
typedef int ptrdiff_t;

// NULL pointer constant
#define NULL ((void*)0)

// offsetof macro: get offset of member in struct
#define offsetof(type, member) ((size_t)&(((type *)0)->member))

#endif // _STDDEF_H_
