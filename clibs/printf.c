#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static  uint8_t *out_buf; // NULL: to stdout, other: to buffer
static  int     out_size;
static  int     out_count;
static  void    (*putchar_p)(uint8_t c);

static  void  putchar_to_buffer(uint8_t c)
{
  if (out_count < out_size) {
    *out_buf++ = c;
  }
  out_count++;
}

static  void  putchar_to_console(uint8_t c)
{
  putchar(c);
  out_count++;
}

static void putchar_rep(uint8_t c, uint8_t  n)
{
  while (n) {
    putchar_p(c);
    n--;
  }
}

static int justify_mem(const uint8_t *s, int len, bool left_justify, bool zero_pad, int width)
{
  int sp = width - len;
  uint8_t pad_char = zero_pad? '0': ' ';
  int sign_len = 0;

  if (zero_pad
  &&  !left_justify
  &&  len > 0) {
    switch(*s) {
    case '+':
    case '-':
    case ' ':
      putchar_p(*s);
      sign_len = 1;
      s++;
    }
  }

  if (!left_justify && sp>0) {
    putchar_rep(pad_char,sp);
  }
  for (int i=sign_len; i<len; i++) {
    putchar_p(*s);
    s++;
  }
  if (left_justify && sp>0) {
    putchar_rep(' ',sp);
  }
  return (width > len ? width : len);
}


uint8_t *_check_nan(float val, bool add_plus);
void _float_to_str(float val, int precision, bool add_plus, uint8_t *buf);
void _float_to_exp_str(float val, int precision, bool add_plus, uint8_t *buf);
void _float_to_hex_str(float val, int precision, bool add_plus, uint8_t *buf);

// printf-like function (float only, no double, + and - flags as bool)
int vsnprintf_core(const uint8_t *fmt, va_list args)
{
  uint8_t buf[128];  // Main buffer for conversions

  while (*fmt) {
    if (*fmt != '%') {
      putchar_p(*fmt);
      fmt++;
      continue;
    }
    fmt++;
    int width = 0;
    int precision = -1;
    bool left_justify = false;
    bool add_plus = false;
    bool zero_pad = false;
    bool is_long = false;

    // Parse flags
    while (*fmt) {
      switch(*fmt) {
      case '-': left_justify = true;  break;
      case '+': add_plus = true;      break;
      case '0': zero_pad = true;      break;
      default:
        goto end_flags;
      }
      fmt++;
    }
end_flags:
    if (left_justify) {
      zero_pad = false;
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
      uint8_t  val = (uint8_t)va_arg(args, int);

      justify_mem(&val,1,left_justify, zero_pad, width);
      break;
    }
    case 'd': {
      long val = is_long? va_arg(args, int32_t): va_arg(args, int16_t);

      if (val < 0) {
        buf[0] = '-';
        ultoa((uint32_t)labs(val), (char *)(buf+1), 10);
      }else{
        if (add_plus) {
          buf[0] = '+';
          ultoa((uint32_t)val, (char *)(buf+1), 10);
        }else{
          ultoa((uint32_t)val, (char *)(buf), 10);
        }
      }
      justify_mem(buf,strlen(buf),left_justify, zero_pad, width);
      break;
    }
    case 'x':
    case 'X':
    case 'u': {
      uint32_t val = is_long? va_arg(args, uint32_t) : va_arg(args, uint16_t);
      int base = (*fmt=='u')? 10:16;

      if (add_plus) {
        buf[0] = '+';
        ultoa(val, (char *)(buf+1), base);
      }else{
        ultoa(val, (char *)(buf), base);
      }
      if (*fmt == 'X') {
        char *p = buf;
        while (*p) { *p = (uint8_t)toupper(*p); p++; }
      }
      justify_mem(buf,strlen(buf),left_justify, zero_pad, width);
      break;
    }
    case 'p': {
      void *val = va_arg(args, void *);
      buf[0] = '0';
      buf[1] = 'x';
      uitoa((uint16_t)val, buf+2, 16);
      justify_mem(buf,strlen(buf),left_justify, zero_pad, width);
      break;
    }
    case 'f':
    case 'e':
    case 'a': {
      float val = (float)va_arg(args, float);
      uint8_t *p;
      if ((p = _check_nan(val,add_plus)) != NULL) {
        justify_mem(p, strlen(p),left_justify, false, width);
        break;
      }
      if (*fmt == 'a') {
        precision = -1;
      }else if (precision < 0){
        precision = 6;
      } else if (precision > 9){
        precision = 9;
      }
      switch(*fmt) {
      case 'f':
        _float_to_str(val, precision, add_plus, buf);
        break;
      case 'e':
        _float_to_exp_str(val, precision, add_plus, buf);
        break;
      case 'a':
        _float_to_hex_str(val, precision, add_plus, buf);
        break;
      }
      justify_mem(buf, strlen(buf), left_justify, zero_pad, width);
      break;
    }
    case 's': {
      const uint8_t *p = (const uint8_t *)va_arg(args, const uint8_t *);
      if (p == NULL) {
        p = (const uint8_t *)"(null)"; // NULL対策
      }
      int len = strlen(p);
      if (precision>=0 && len>precision) {
        len = precision;
      }
      justify_mem(p, len, left_justify, zero_pad, width);
      break;
    }
    case '%':
      putchar_p('%');
      break;
    default:
      putchar_p('%');
      putchar_p(*fmt);
      break;
    }
    fmt++;
  }

  return out_count;
}

int printf(const char *fmt, ...)
{
  out_count = 0;
  putchar_p = putchar_to_console;

  va_list args;
  va_start(args, fmt);
  vsnprintf_core(fmt, args);
  va_end(args);

  return out_count;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
  out_buf = str;
  out_size = (int)size-1;
  out_count = 0;
  putchar_p = putchar_to_buffer;

  va_list args;
  va_start(args, fmt);
  vsnprintf_core(fmt, args);
  va_end(args);

  if (size > 0) {
    str[(out_count < size) ? out_count : size - 1] = '\0';
  }
  return out_count;
}

int sprintf(char *str, const char *fmt, ...)
{
  out_buf = str;
  out_size = 1024-1;  // TODO:
  out_count = 0;
  putchar_p = putchar_to_buffer;

  va_list args;
  va_start(args, fmt);
  vsnprintf_core(fmt, args);
  va_end(args);

  str[out_count] = '\0';

  return out_count;
}
