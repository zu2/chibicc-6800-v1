#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define my_signbit(x) ((*(unsigned char *)&x)&0x80)

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
    for (int i=0; i<sp; i++) {
      putchar_p(pad_char);
    }
  }
  for (int i=sign_len; i<len; i++) {
    putchar_p(*s);
    s++;
  }
  if (left_justify && sp>0) {
    for (int i=0; i<sp; i++) {
      putchar_p(' ');
    }
  }
  return (width > len ? width : len);
}

// Check if float is NaN or Inf, print and return 1 if true
static uint8_t *check_nan(float val,bool add_plus)
{
  if (isnan(val)) {
    if (my_signbit(val)) {
      return (uint8_t *)"-nan";
    }else if (add_plus) {
      return (uint8_t *)"+nan";
    }
    return (uint8_t *)"nan";
  } else if (isinf(val)) {
    if (my_signbit(val)) {
      return (uint8_t *)"-inf";
    }else if (add_plus) {
      return (uint8_t *)"+inf";
    }
    return (uint8_t *)"inf";
  }
  return NULL;
}

static uint8_t *format_float_core(uint8_t *p, float val, int precision)
{
  float int_part;
  float frac_part = modff(val, &int_part);

  ultoa((uint32_t)int_part, (char *)p, 10);
  p += strlen((char *)p);

  if (precision > 0) {
    *p++ = '.';
    for (int i = 0; i < precision; i++) {
      frac_part = frac_part * 10.0f;
      int digit = (int)frac_part;
      *p++ = digit + '0';
      frac_part -= (float)digit;
    }
  }
  return p;
}

//
// get_round_add:
//  Adds 0.5 at the last decimal place for simple rounding.
//  This is NOT a full round-half-to-even implementation.
//  Expect small differences vs. standard printf at large floats.
//

static float get_round_add(int precision)
{
  float r = 0.5f;
  for (int i = 0; i < precision; i++) r /= 10.0f;
  return r;
}

static void float_to_exp_str(float val, int precision, bool add_plus, uint8_t *buf)
{
  uint8_t *p = buf;
  if (my_signbit(val)) { *p++ = '-'; val = fabsf(val); }
  else if (add_plus) { *p++ = '+'; }

  int exp = 0;
  while (val >= 10.0f) { val /= 10.0f; exp++; }
  while (val < 1.0f && val != 0.0f) { val *= 10.0f; exp--; }

  val += get_round_add(precision);
  if (val >= 10.0f) { val /= 10.0f; exp++; }

  p = format_float_core(p, val, precision);

  *p++ = 'e';
  if (exp < 0) { *p++ = '-'; exp = -exp; }
  else { *p++ = '+'; }
  *p++ = (exp / 10) + '0';
  *p++ = (exp % 10) + '0';
  *p = '\0';
}

//
// NOTE: This implementation of %f assumes that integer part fits in uint32_t.
// For values near FLT_MAX, %f falls back to %e-style scientific output.
// Therefore, extremely large %f values (like 3.402823466e+38f) appear as
// "3.402824e+38" instead of the full 38-digit integer.
//
// For exact printf-like rounding, a full Ryu/Dragon4-style algorithm is required.
//
static void float_to_str(float val, int precision, bool add_plus, uint8_t *buf)
{
  uint8_t *p = buf;
  float int_part, frac_part;

  if (my_signbit(val)) { *p++ = '-'; val = fabsf(val); }
  else if (add_plus) { *p++ = '+'; }

  frac_part = modff(val,&int_part);
  if (int_part <= 4294967296.0f) {
    // Simple 0.5 add; not full round-half-to-even
    val += get_round_add(precision);

    p = format_float_core(p, val, precision);
  }else{
    float_to_exp_str(val, precision, add_plus, p);
    return;
  }
  *p = '\0';
}


// Convert float to hex float string for %a (add sign if needed)
static void float_to_hex_str(float val, int precision, bool add_plus, uint8_t *buf)
{
  uint8_t *p = buf;

  if (my_signbit(val)) {
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

  uint32_t mant = (*(uint32_t *)&frac) & 0x007fffff;
  mant <<= 1;

  if (precision < 0) {
    if (mant != 0) {
      *p++ = '.';
      int len = 6;
      uint32_t temp = mant;
      while (len > 0 && (temp & 0xF) == 0) {
        len--;
        temp >>= 4;
      }
      for (int i = 0; i < len; i++) {
        uint8_t d = (mant >> 20) & 0xF;
        *p++ = d < 10 ? d+'0' : d+'a'-10;
        mant <<= 4;
      }
    }
  } else if (precision > 0) {
    *p++ = '.';
    for (int i = 0; i < precision; i++) {
      uint8_t d = (mant >> 20) & 0xF;
      *p++ = d < 10 ? d+'0' : d+'a'-10;
      mant <<= 4;
    }
  }

  *p++ = 'p';

  if (exp < 0) {
    *p++ = '-';
    exp = -exp;
  } else {
    *p++ = '+';
  }
  itoa(exp, (char *)p, 10); 
}

// printf-like function (float only, no double, + and - flags as bool)
int vsnprintf_core(const uint8_t *fmt, va_list args)
{
  uint8_t buf[32];  // Main buffer for conversions

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
    case 'u': {
      uint32_t val = is_long? va_arg(args, uint32_t) : va_arg(args, uint16_t);
      if (add_plus) {
        buf[0] = '+';
        ultoa(val, (char *)(buf+1), 10);
      }else{
        ultoa(val, (char *)(buf), 10);
      }
      justify_mem(buf,strlen(buf),left_justify, zero_pad, width);
      break;
    }
    case 'x':
    case 'X': {
      uint32_t val = is_long? va_arg(args, uint32_t) : va_arg(args, uint16_t);

      if (add_plus) {
        buf[0] = '+';
        ultoa(val, (char *)(buf+1), 16);
      }else{
        ultoa(val, (char *)(buf), 16);
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
      if ((p = check_nan(val,add_plus)) != NULL) {
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
        float_to_str(val, precision, add_plus, buf);
        break;
      case 'e':
        float_to_exp_str(val, precision, add_plus, buf);
        break;
      case 'a':
        float_to_hex_str(val, precision, add_plus, buf);
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
