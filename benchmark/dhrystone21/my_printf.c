#include <stdio.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int my_puts(char *s)
{
  int count = 0;

  while (*s) {
    putchar(*s);
    s++;
    count++;
  }
  return count;
}

int printf(const unsigned char *format, ...)
{
  va_list args;
  va_start(args, format);

  int count = 0;
  int radix;
  union value {
    unsigned char c;
    int   i;
    long  l;
    float f;
    unsigned char *s;
  } value;
  float frac_part, int_part;
  long lv;
  int len;
  int c;
  int i,j;
  unsigned char *p;
  unsigned char *buf;
  unsigned   char buffer[30];

  buf = buffer;
  for (p=format; *p; p++) {
    c = *p;
    if (c != '%') {
      putchar(c);
      count++;
      continue;
    }
    p++; c = *p;
    if (c=='\0') {
      putchar('%');
      count++;
      break;
    }
    switch (c) {
    case 'c':
      value.i = va_arg(args, unsigned char);
      putchar(value.c);
      count++;
      break;
    case 'l':
      p++ ; c = *p;
      switch (c) {
      case 'x':
      case 'd':
        value.l = va_arg(args, unsigned long);
	ltoa(value.l, buffer, (c=='d')?10 :16);
        count = my_puts(buffer) + count;
        break;
      default:
	putchar('%');
	putchar('l');
	putchar(c);
	count = count + 3;
	break;
      }
      break;
    case 'p':
      putchar('0');putchar('x');
      count = count + 2;
    case 'x':
    case 'd':
      value.i = va_arg(args, unsigned int);
      itoa(value.i, buffer, (c=='d')?10 :16);

      count = my_puts(buffer) + count;
      break;
    case 'f':
      value.f = va_arg(args, float);

      if (value.f < 0) {
          putchar('-');
          count++;
          value.f = fabsf(value.f);
      }

      frac_part = modff(value.f,&int_part);
      ltoa((unsigned long)int_part, buffer, 10); 

      count = my_puts(buffer) + count;

      putchar('.');
      count++;

      for (j = 0; j < 6; j++) {
        frac_part *= 10.0;
        int digit = truncf(frac_part);
        putchar('0' + digit);
        count++;
        frac_part -= digit;
      }
      break;
    case 'e': {
      float value = va_arg(args, float);
      int negative = 0;

      if (value < 0) {
	negative = 1;
	value = -value;
      }

      int exponent = 0;
      if (value != 0.0) {
	if (value >= 10.0) {
          while (value >= 10.0) {
	    value /= 10.0;
	    exponent++;
	  }
	} else if (value < 1.0) {
	  while (value < 1.0) {
	    value *= 10.0;
            exponent--;
	  }
	}
      }

      if (negative) {
        count++;
      }

      int intPart = (int)value;
      putchar('0' + intPart);
      count++;

      putchar('.');
      count++;

      float fracPart = value - intPart;
      for (int j = 0; j < 6; j++) {
	fracPart *= 10;
	int digit = (int)fracPart;
	putchar('0' + digit);
	count++;
	fracPart -= digit;
      }

      putchar('e');
      count++;

      if (exponent >= 0) {
	putchar('+');
      } else {
	putchar('-');
	exponent = -exponent;
      }
      count++;

      if (exponent < 10) {
	putchar('0');
	count++;
      }

      itoa(exponent, buffer, 10);
      int len = strlen(buffer);
      for (int j = 0; j < len; j++) {
	putchar(buffer[j]);
      }
      count += len;
      break;
    } // 'e'
    case 's':
      value.s = va_arg(args, char *);
      if (value.s) {
        count = my_puts(value.s) + count;
      }
      break;
    case '%':
      putchar('%');
      count++;
      break;
    default:
      putchar('%');
      putchar(c);
      count += 2;
      break;
    }
  }
  va_end(args);
  return count;
}
