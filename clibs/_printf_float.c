#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define my_signbit(x) ((*(unsigned char *)&x)&0x80)

// Check if float is NaN or Inf, print and return 1 if true
uint8_t *_check_nan(float val,bool add_plus)
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

void _float_to_exp_str(float val, int precision, bool add_plus, uint8_t *buf)
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
  if (exp < 10) {
    *p++ = '0';
  }
  uitoa((uint16_t)exp, (char *)p, 10);
}

//
// NOTE: This implementation of %f assumes that integer part fits in uint32_t.
// For values near FLT_MAX, %f falls back to %e-style scientific output.
// Therefore, extremely large %f values (like 3.402823466e+38f) appear as
// "3.402824e+38" instead of the full 38-digit integer.
//
// For exact printf-like rounding, a full Ryu/Dragon4-style algorithm is required.
//
void _float_to_str(float val, int precision, bool add_plus, uint8_t *buf)
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
    _float_to_exp_str(val, precision, add_plus, p);
    return;
  }
  *p = '\0';
}


// Convert float to hex float string for %a (add sign if needed)
void _float_to_hex_str(float val, int precision, bool add_plus, uint8_t *buf)
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

  int len = precision;
  if (precision < 0) {
    len = 6;
    uint32_t temp = mant;
    while (len > 0 && (temp & 0xF) == 0) {
      len--;
      temp >>= 4;
    }
  }

  if (len > 0) {
    *p++ = '.';
    for (int i = 0; i < len; i++) {
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
  uitoa((uint16_t)exp, (char *)p, 10);
}
