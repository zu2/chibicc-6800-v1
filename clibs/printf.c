#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void putspace(int n)
{
  while (n-->0) {
    putchar(' ');
  }
}

static void putstring(const uint8_t *s)
{
  while (*s) {
    putchar(*s++);
  }
}

static int justify(const uint8_t *s,int left_justify, int width)
{
  int len = strlen(s);
  int n = 0;
  int sp;

  if ((sp = width-len) > 0) {
    if (!left_justify) {
      putspace(sp);
    }
    putstring(s);
    if (left_justify) {
      putspace(sp);
    }
    return width;
  }
  putstring(s);
  return len;
}

// Check if float is NaN or Inf, print and return 1 if true
static uint8_t *check_nan(float val)
{
  if (isnan(val)) {
    return (uint8_t *)"NaN";
  } else if (isinf(val)) {
    if (val > 0) {
      return (uint8_t *)"+Inf";
    } else {
      return (uint8_t *)"-Inf";
    }
  }
  return NULL;
}

// Convert float to string for %f (add sign if needed)
static void float_to_str(float val, int precision, bool add_plus,
                         uint8_t *buf)
{
  uint8_t *p = buf;

  if (val < 0) {
    *p++ = '-';
    val = fabsf(val);
  } else if (add_plus) {
    *p++ = '+';
  }
  float int_part;
  float frac_part = modff(val, &int_part);
//uint8_t tmp[64];

  // NOTE: integer part must fit in uint32_t (max ~4.3e9).
  // Values >= 1e10 will produce incorrect output.
  ultoa((uint32_t)int_part, p, 10);
  p += strlen(p);
  if (precision>0) *p++ = '.';

  uint32_t denominator = 1UL<<24;
  uint32_t numerator = (uint32_t)(frac_part * (float)denominator);;
  for (int i = 0; i < precision; i++) {
    numerator = numerator * 10;
    int digit = numerator / denominator;
    *p++ = '0' + digit;
    numerator -= digit * denominator;
  }
  *p = '\0';
}

// Convert float to scientific notation string for %e (add sign if needed)
static void float_to_exp_str(float val, int precision, bool add_plus,
                             uint8_t *buf)
{
 uint8_t *p = buf;

  if (val < 0) {
    *p++ = '-';
    val = fabsf(val);
  } else if (add_plus) {
    *p++ = '+';
  }
  int exp = 0;
  while (val >= 10.0f) {
    val = val / 10.0f;
    exp++;
  }
  while (val < 1.0f && val != 0.0f) {
    val = val * 10.0f;
    exp--;
  }
  float int_part;
  float frac_part = modff(val, &int_part);
  ultoa((uint32_t)int_part, p, 10);
  p += strlen(p);
  if (precision>0) *p++ = '.';

  for (int i = 0; i < precision; i++) {
    frac_part = frac_part * 10.0f;
    int digit = (int)frac_part;
    *p++ = '0' + digit;
    frac_part -= digit;
  }
  *p++ = 'e';
  if (exp < 0) {
    *p++ = '-';
    exp = -exp;
  } else {
    *p++ = '+';
  }
  itoa(exp, p, 10);
}

// Convert float to hex float string for %a (add sign if needed)
static void float_to_hex_str(float val, bool add_plus, uint8_t *buf)
{
  uint8_t tmp[32];
  uint8_t *p = buf;

  if (signbit(val)) {
    *p++ = '-';
    val = fabsf(val);
  } else if (add_plus) {
    *p++ = '+';
  }
  if (val==0.0f) {
    strcpy(p, "0x0.000000p+0");
    return;
  }
  int exp;
  float frac = frexpf(val,&exp);

  // Normalize value to [1.0, 2.0)
  frac = frac * 2.0f;
  exp -= 1;

  *p++ = '0';
  *p++ = 'x';
  *p++ = '1';
  *p++ = '.';

  uint32_t mant = (*(long *)&frac) & 0x007fffff;
  ultoa(mant,tmp,16);
  int len = strlen(tmp);
  for (int i=0; i<6-len; i++) {
    *p++ = '0';
  }
  strcpy(p,tmp);
  p += len;

  *p++ = 'p';
  if (exp < 0) {
    *p++ = '-';
    exp = -exp;
  } else {
    *p++ = '+';
  }
  itoa(exp, p, 10);
}

// Output unsigned integer
static int output_uint(uint32_t val, int radix, bool add_plus,
                       bool left_justify, int width, bool uppercase,
                       uint8_t *buf)
{
  uint8_t *p = buf;
  if (add_plus) *p++ = '+';
  ultoa(val, (char *)p, radix);
  if (uppercase) {
    while (*p) { *p = (uint8_t)toupper(*p); p++; }
  }
  return justify(buf, left_justify, width);
}

static int output_int(long val, bool add_plus,
                      bool left_justify, int width,
                      uint8_t *buf)
{
  if (val < 0) {
    buf[0] = '-';
    ultoa((uint32_t)labs(val), (char *)(buf + 1), 10);
    return justify(buf, left_justify, width);
  }
  return output_uint((uint32_t)val, 10, add_plus,
                     left_justify, width, false, buf);
}


// printf-like function (float only, no double, + and - flags as bool)
int printf(const uint8_t *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int total = 0;
  uint8_t buf[64];  // Main buffer for conversions

  while (*fmt) {
    if (*fmt != '%') {
      putchar(*fmt);
      total++;
      fmt++;
      continue;
    }
    fmt++;
    int width = 0;
    int precision = -1;
    bool left_justify = false;
    bool add_plus = false;
    bool is_long = false;
    // Parse flags
    while (*fmt == '-' || *fmt == '+') {
      if (*fmt == '-') {
        left_justify = true;
      }else if (*fmt == '+') {
        add_plus = true;
      }
      fmt++;
    }
    // Parse width
    while (isdigit(*fmt)) {
      width = width * 10 + (*fmt - '0');
      fmt++;
    }
    // Parse precision
    if (*fmt == '.') {
      fmt++;
      precision = 0;
      while (isdigit(*fmt)) {
        precision = precision * 10 + (*fmt - '0');
        fmt++;
      }
    }
    // Handle long format specifiers (%ld, %lx, %lX)
    if (*fmt == 'l') {
      is_long = true;
      fmt++;
    }
    // Handle format specifier
    switch (*fmt) {
    case 'c': {
      int val = va_arg(args, int);
      putchar(val);
      total++;
      break;
    }
    case 'd': {
      long val = is_long? va_arg(args, long): va_arg(args, int);
      total += output_int(val, add_plus, left_justify, width, buf);
      break;
    }
    case 'u': {
      uint32_t val = is_long? va_arg(args, uint32_t)
                                 : va_arg(args, unsigned int);
      total += output_uint(val, 10, false, left_justify, width, false, buf);
      break;
    }
    case 'x':
    case 'X': {
      uint32_t val = is_long? va_arg(args, uint32_t)
                                 : va_arg(args, unsigned int);
      total += output_uint(val, 16, add_plus, left_justify, width, *fmt == 'X', buf);
      break;
    }
    case 'p': {
      void *val = va_arg(args, void *);
      buf[0] = '0';
      buf[1] = 'x';
      uitoa((unsigned int)val, buf+2, 16);
      total += justify(buf, left_justify, width);
      break;
    }
    case 'f': {
      float val = (float)va_arg(args, float);
      uint8_t *p;
      if ((p = check_nan(val))!=NULL) {
        total += justify(p,left_justify,width);
        break;
      }
      if (precision < 0) {
        precision = 6;
      }else if (precision > 9) {
        precision = 9;
      }
      float_to_str(val, precision, add_plus, buf);
      total += justify(buf, left_justify, width);
      break;
    }
    case 'e': {
      float val = (float)va_arg(args, float);
      uint8_t *p;
      if (p = check_nan(val)) {
        total += justify(p,left_justify,width);
        break;
      }
      if (precision < 0) {
        precision = 6;
      }
      float_to_exp_str(val, precision, add_plus, buf);
      total += justify(buf, left_justify, width);
      break;
    }
    case 'a': {
      float val = (float)va_arg(args, float);
      uint8_t *p;
      if (p = check_nan(val)) {
        total += justify(p,left_justify,width);
        break;
      }
      float_to_hex_str(val, add_plus, buf);
      total += justify(buf, left_justify, width);
      break;
    }
    case 's': {
      const uint8_t *p =
          (const uint8_t *)va_arg(args, const uint8_t *);
      if (p == NULL) {
        p = (const uint8_t *)"(null)"; // NULL対策
      }
      if (precision==0) {
        putspace(width); total += width;
      }else if (precision>0 && strlen(p)>precision) {
        for (int i=0; i<precision; i++) {
          putchar(*p++);
        }
        total += precision;
      }else{
        total += justify(p, left_justify, width);
      }
      break;
    }
    case '%':
      putchar('%');
      total++;
      break;
    default:
      putchar('%');
      putchar(*fmt);
      total += 2;
      break;
    }
    fmt++;
  }
  va_end(args);
  return total;
}

#if defined(PRINTF_TEST)
// Example main function
int main(void)
{
  float f = 3.141592f;
  int i = 123;
  long l = 123456789L;
  void *p = (void *)0x1234abcd;
  printf("Float: %+-.3f, Int: %+5d, Long: %ld, HexLong: %lx, Ptr: %p, Exp: %e, "
         "HexFloat: %a, Uint: %u, UpperHex: %X, Str: %s\n",
         f, i, l, l, p, f, f, 456u, i, "Hello, world!");
  return 0;
#endif
