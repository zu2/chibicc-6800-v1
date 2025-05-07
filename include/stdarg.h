#ifndef STDARG_H
#define STDARG_H

// va_list is a pointer type used to traverse variable arguments
typedef char *va_list;
//static	char *__builtin_va_start_addr();

// Initialize 'ap' to point to the first variable argument after 'last'.
// __builtin_va_start_addr(last) is a compiler builtin that returns the address
// immediately after the last fixed argument.
#define va_start(ap, last) (ap = __builtin_va_start_addr(last))

// Retrieve the next argument of type 'type' from the argument list.
// Advances 'ap' to the next argument.
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))

// Clean up the va_list. This is usually a no-op, but we reset ap to NULL for safety.
#define va_end(ap) ((void)(ap = (va_list)0))


#endif // STDARG_H
