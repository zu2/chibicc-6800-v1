// stdint.h for chibicc-6800-v1 C compiler
// char: 8bit, short/int: 16bit, long/float: 32bit
// long long, double: not supported

// Exact-width integer types
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef signed int      int16_t;
typedef unsigned int    uint16_t;
typedef signed long     int32_t;
typedef unsigned long   uint32_t;

// Minimum-width integer types
typedef int8_t          int_least8_t;
typedef uint8_t         uint_least8_t;
typedef int16_t         int_least16_t;
typedef uint16_t        uint_least16_t;
typedef int32_t         int_least32_t;
typedef uint32_t        uint_least32_t;

// Fastest minimum-width integer types
typedef int8_t          int_fast8_t;
typedef uint8_t         uint_fast8_t;
typedef int16_t         int_fast16_t;
typedef uint16_t        uint_fast16_t;
typedef int32_t         int_fast32_t;
typedef uint32_t        uint_fast32_t;

// Integer limits
#define INT8_MIN        (-128)
#define INT8_MAX        127
#define UINT8_MAX       255
#define INT16_MIN       (-32768)
#define INT16_MAX       32767
#define UINT16_MAX      65535U
#define INT32_MIN       (-2147483647L-1)
#define INT32_MAX       2147483647L
#define UINT32_MAX      4294967295UL

// Pointer types (assuming 16-bit pointers)
typedef int16_t         intptr_t;
typedef uint16_t        uintptr_t;

