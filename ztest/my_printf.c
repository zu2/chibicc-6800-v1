#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Output a string of length n (using putchar for demo)
int putnstr(const unsigned char *s, int n)
{
  int count = 0;
  while (n-- > 0 && *s) {
    putchar(*s++);
    count++;
  }
  return count;
}

// Check if float is NaN or Inf, print and return 1 if true
static int check_nan(float val, int *total)
{
  if (isnan(val)) {
    *total += putnstr((const unsigned char *)"NaN", 3);
    return 1;
  } else if (isinf(val)) {
    if (val > 0) {
      *total += putnstr((const unsigned char *)"+Inf", 4);
    } else {
      *total += putnstr((const unsigned char *)"-Inf", 4);
    }
    return 1;
  }
  return 0;
}

// Convert float to string for %f (add sign if needed)
static void float_to_str(float val, int precision, bool add_plus,
                         unsigned char *buf)
{
  int pos = 0;
  if (val < 0) {
    buf[pos++] = '-';
    val = -val;
  } else if (add_plus) {
    buf[pos++] = '+';
  }
  float int_part;
  float frac_part = modff(val, &int_part);
  char tmp[64];
  ltoa((long)int_part, tmp, 10);
  strcpy((char *)(buf + pos), tmp);
  pos += strlen(tmp);
  buf[pos++] = '.';
  for (int i = 0; i < precision; i++) {
    frac_part *= 10.0f;
    int digit = (int)frac_part;
    buf[pos++] = '0' + digit;
    frac_part -= digit;
  }
  buf[pos] = '\0';
}

// Convert float to scientific notation string for %e (add sign if needed)
static void float_to_exp_str(float val, int precision, bool add_plus,
                             unsigned char *buf)
{
  int pos = 0;
  if (val < 0) {
    buf[pos++] = '-';
    val = -val;
  } else if (add_plus) {
    buf[pos++] = '+';
  }
  int exp = 0;
  while (val >= 10.0f) {
    val /= 10.0f;
    exp++;
  }
  while (val < 1.0f && val != 0.0f) {
    val *= 10.0f;
    exp--;
  }
  float int_part;
  float frac_part = modff(val, &int_part);
  char tmp[64];
  ltoa((long)int_part, tmp, 10);
  strcpy((char *)(buf + pos), tmp);
  pos += strlen(tmp);
  buf[pos++] = '.';
  for (int i = 0; i < precision; i++) {
    frac_part *= 10.0f;
    int digit = (int)frac_part;
    buf[pos++] = '0' + digit;
    frac_part -= digit;
  }
  buf[pos++] = 'e';
  if (exp < 0) {
    buf[pos++] = '-';
    exp = -exp;
  } else {
    buf[pos++] = '+';
  }
  ltoa(exp, tmp, 10);
  strcpy((char *)(buf + pos), tmp);
}

// Convert float to hex float string for %a (add sign if needed)
static void float_to_hex_str(float val, bool add_plus, unsigned char *buf)
{
  int pos = 0;
  if (val < 0) {
    buf[pos++] = '-';
    val = -val;
  } else if (add_plus) {
    buf[pos++] = '+';
  }
  buf[pos++] = '0';
  buf[pos++] = 'x';
  unsigned int u = *(unsigned int *)&val;
  int exp = ((u >> 23) & 0xFF) - 127;
  int mant = (u & 0x7FFFFF);
  buf[pos++] = '1';
  buf[pos++] = '.';
  for (int i = 0; i < 6; i++) {
    mant <<= 4;
    int digit = (mant >> 23) & 0xF;
    buf[pos++] = "0123456789abcdef"[digit];
    mant &= 0x7FFFFF;
  }
  buf[pos++] = 'p';
  if (exp < 0) {
    buf[pos++] = '-';
    exp = -exp;
  } else {
    buf[pos++] = '+';
  }
  char tmp[32];
  ltoa(exp, tmp, 10);
  strcpy((char *)(buf + pos), tmp);
}

// Justify a string (for width and left_justify)
static int justify(const unsigned char *buf, bool left_justify, int width)
{
  int len = strlen((const char *)buf);
  if (width <= len) {
    return putnstr(buf, len);
  }
  int count = 0;
  if (!left_justify) {
    for (int i = 0; i < width - len; i++) {
      putchar(' ');
      count++;
    }
  }
  count += putnstr(buf, len);
  if (left_justify) {
    for (int i = 0; i < width - len; i++) {
      putchar(' ');
      count++;
    }
  }
  return count;
}

// Convert string to uppercase (for %X, %lX)
static void str_toupper(unsigned char *s)
{
  while (*s = toupper(*s)) {
    s++;
  }
}

// Output integer as string with sign and justify
static int output_int(long val, int radix, bool add_plus, bool left_justify,
                      int width, bool uppercase, unsigned char *buf)
{
  char tmp[64];
  if (val < 0) {
    ltoa(val, tmp, radix);
  } else if (add_plus) {
    tmp[0] = '+';
    ltoa(val, tmp + 1, radix);
  } else {
    ltoa(val, tmp, radix);
  }
  strcpy((char *)buf, tmp);
  if (uppercase) {
    str_toupper(buf);
  }
  return justify(buf, left_justify, width);
}

// printf-like function (float only, no double, + and - flags as bool)
int printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int total = 0;
  unsigned char buf[64];  // Main buffer for conversions
  unsigned char buf2[64]; // Sub buffer if needed

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
    // Parse flags
    while (*fmt == '-' || *fmt == '+') {
      if (*fmt == '-') {
        left_justify = true;
      }
      if (*fmt == '+') {
        add_plus = true;
      }
      fmt++;
    }
    // Parse width
    while (*fmt >= '0' && *fmt <= '9') {
      width = width * 10 + (*fmt - '0');
      fmt++;
    }
    // Parse precision
    if (*fmt == '.') {
      fmt++;
      precision = 0;
      while (*fmt >= '0' && *fmt <= '9') {
        precision = precision * 10 + (*fmt - '0');
        fmt++;
      }
    }
    // Handle long format specifiers (%ld, %lx, %lX)
    if (*fmt == 'l') {
      fmt++;
      if (*fmt == 'd') {
        long val = va_arg(args, long);
        total += output_int(val, 10, add_plus, left_justify, width, false, buf);
        fmt++;
        continue;
      } else if (*fmt == 'x' || *fmt == 'X') {
        unsigned long val = va_arg(args, unsigned long);
        total += output_int((long)val, 16, add_plus, left_justify, width,
                            *fmt == 'X', buf);
        fmt++;
        continue;
      } else {
        putchar('%');
        putchar('l');
        putchar(*fmt);
        total += 3;
        fmt++;
        continue;
      }
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
      int val = va_arg(args, int);
      total += output_int(val, 10, add_plus, left_justify, width, false, buf);
      break;
    }
    case 'u': {
      unsigned int val = va_arg(args, unsigned int);
      total += output_int(val, 10, false, left_justify, width, false, buf);
      break;
    }
    case 'x':
    case 'X': {
      unsigned int val = va_arg(args, unsigned int);
      total +=
          output_int(val, 16, add_plus, left_justify, width, *fmt == 'X', buf);
      break;
    }
    case 'p': {
      void *val = va_arg(args, void *);
      char tmp[32];
      ltoa((unsigned int)val, tmp, 16);
      buf2[0] = '0';
      buf2[1] = 'x';
      strcpy((char *)(buf2 + 2), tmp);
      total += justify(buf2, left_justify, width);
      break;
    }
    case 'f': {
      float val = (float)va_arg(args, double);
      if (check_nan(val, &total)) {
        break;
      }
      if (precision < 0) {
        precision = 6;
      }
      float_to_str(val, precision, add_plus, buf);
      total += justify(buf, left_justify, width);
      break;
    }
    case 'e': {
      float val = (float)va_arg(args, double);
      if (check_nan(val, &total)) {
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
      float val = (float)va_arg(args, double);
      if (check_nan(val, &total)) {
        break;
      }
      float_to_hex_str(val, add_plus, buf);
      total += justify(buf, left_justify, width);
      break;
    }
    case 's': {
      const unsigned char *val =
          (const unsigned char *)va_arg(args, const char *);
      if (val == NULL) {
        val = (const unsigned char *)"(null)"; // NULL対策
      }
      int len = strlen((const char *)val);
      if (width > len) {
        if (!left_justify) {
          for (int i = 0; i < width - len; i++) {
            putchar(' ');
            total++;
          }
        }
        total += putnstr(val, len);
        if (left_justify) {
          for (int i = 0; i < width - len; i++) {
            putchar(' ');
            total++;
          }
        }
      } else {
        total += putnstr(val, len);
      }
      break;
    }
    case '%':
      putchar('%');
      total += 1;
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
}
#endif
