//
// limits.h for chibicc-6800-v1 C compiler
// char: 8bit, short/int: 16bit, long: 32bit
// char is signed. long long: not supported
//

#ifndef _LIMITS_H_
#define _LIMITS_H_

#define CHAR_BIT    8

#define SCHAR_MIN   (-128)
#define SCHAR_MAX   127
#define UCHAR_MAX   255

#define CHAR_MIN    SCHAR_MIN
#define CHAR_MAX    SCHAR_MAX

#define MB_LEN_MAX  1

#define SHRT_MIN    (-32768)
#define SHRT_MAX    32767
#define USHRT_MAX   65535U

#define INT_MIN     (-32768)
#define INT_MAX     32767
#define UINT_MAX    65535U

#define LONG_MIN    (-2147483647L - 1L)
#define LONG_MAX    2147483647L
#define ULONG_MAX   4294967295UL

// LLONG_MIN
// LLONG_MAX
// ULLONG_MAX

#endif // _LIMITS_H_
