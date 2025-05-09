#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

int putstr(const char *s, int len) {
  int count = 0;
  const char *end = s + len;
  while (s < end && *s) {
    putchar(*s++);
    count++;
  }
  return count;
}

void utoa(unsigned int value, char *buf) {
  char tmp[20];
  int i = 0;
  if (value == 0) {
    buf[0] = '0'; buf[1] = '\0';
    return;
  }
  while (value > 0) {
    tmp[i++] = '0' + (value % 10);
    value /= 10;
  }
  for (int j = 0; j < i; j++) buf[j] = tmp[i - j - 1];
  buf[i] = '\0';
}

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  const char *p = format;
  char buf[128];
  int total = 0;

  while (*p) {
    if (*p == '%') {
      p++;
      int left_justify = 0;
      if (*p == '-') { left_justify = 1; p++; }

      int width = 0;
      const char *wp = p;
      while (isdigit(*wp)) wp++;
      if (wp != p) {
        width = atoi(p);
        p = wp;
      }

      int precision = -1;
      if (*p == '.') {
        p++;
        const char *pp = p;
        while (isdigit(*pp)) pp++;
        if (pp != p) {
          precision = atoi(p);
          p = pp;
        } else {
          precision = 0;
        }
      }

      int is_long = 0;
      if (*p == 'l') { is_long = 1; p++; }

      char spec = *p++;
      switch (spec) {
	case 'p':
	case 'x':
        case 'd': {
          long val = is_long ? va_arg(args, long) : va_arg(args, int);
	  switch (spec) {
	  case 'p':
	     buf[0] = '0';
	     buf[1] = 'x';
	     ltoa(val, buf+2, 16);
	     break;
	  case 'x':
	     ltoa(val, buf, 16);
	     break;
	  default:
	     ltoa(val, buf, 10);
	     break;
	  }
          int len = strlen(buf);
          int pad = width > len ? width - len : 0;
          if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          total += putstr(buf, len);
          if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          break;
        }
        case 'u': {
          unsigned int val = va_arg(args, unsigned int);
          utoa(val, buf);
          int len = strlen(buf);
          int pad = width > len ? width - len : 0;
          if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          total += putstr(buf, len);
          if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          break;
        }
        case 'f': {
          double val = va_arg(args, double);

	  if (isnan(val)) {
	    total += putstr("NaN", 3);
	    break;
	  }else if (isinf(val)) {
            if (val>0) {
	      total += putstr("+Inf", 4);
	    }else{
	      total += putstr("-Inf", 4);
	    }
	    break;
	  }
          int int_part;
          double frac_part = modff(val, &int_part);
          if (precision < 0) precision = 6;
          itoa(int_part, buf, 10);
          int len = strlen(buf) + 1 + precision;
          int pad = width > len ? width - len : 0;
          if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          total += putstr(buf, strlen(buf));
          putchar('.'); total++;
          frac_part = fabsf(frac_part);
          for (int i = 0; i < precision; i++) {
            frac_part *= 10;
            int digit = (int)frac_part;
            putchar('0' + digit); total++;
            frac_part -= digit;
          }
          if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          break;
        }
        case 'e': {
          double val = va_arg(args, double);
          if (precision < 0) precision = 6;
          int exp = 0;
          double v = val;
          int sign = v < 0 ? -1 : 1;
          v = fabsf(v);
          if (v == 0.0) {
            int len = 2 + precision + 4;
            int pad = width > len ? width - len : 0;
            if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
            putchar('0'); total++; putchar('.'); total++;
            for (int i = 0; i < precision; i++) { putchar('0'); total++; }
            total += putstr("e+00", 4);
            if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          } else {
            while (v >= 10.0) { v /= 10.0; exp++; }
            while (v < 1.0) { v *= 10.0; exp--; }
            int int_part = (int)v;
            double frac_part = v - int_part;
            int len = (sign < 0 ? 1 : 0) + 1 + 1 + precision + 4;
            int pad = width > len ? width - len : 0;
            if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
            if (sign < 0) { putchar('-'); total++; }
            itoa(int_part, buf, 10);
            total += putstr(buf, strlen(buf));
            putchar('.'); total++;
            for (int i = 0; i < precision; i++) {
              frac_part *= 10;
              int digit = (int)frac_part;
              putchar('0' + digit); total++;
              frac_part -= digit;
            }
            putchar('e'); total++;
            putchar(exp >= 0 ? '+' : '-'); total++;
            int abs_exp = abs(exp);
            putchar('0' + (abs_exp / 10)); total++;
            putchar('0' + (abs_exp % 10)); total++;
            if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          }
          break;
        }
        case 's': {
          char *str = va_arg(args, char *);
          if (!str) str = "(null)";
          int len = strlen(str);
          if (precision >= 0 && precision < len) len = precision;
          int pad = width > len ? width - len : 0;
          if (!left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          total += putstr(str, len);
          if (left_justify) for (int i = 0; i < pad; i++) { putchar(' '); total++; }
          break;
        }
        case 'c': {
          int ch = va_arg(args, int);
          putchar(ch); total++;
          break;
        }
        case '%': {
          putchar('%'); total++;
          break;
        }
        default: {
          putchar('%'); total++;
          putchar(spec); total++;
          break;
        }
      }
    } else {
      putchar(*p++); total++;
    }
  }
  va_end(args);
  return total;
}
