#include <math.h>
#include <stdint.h>

//
// Cody–Waite range reduction
//
// LN2_HI: top 12 bits of ln2, so n * LN2_HI is exact
// LN2_LO: the remaining part of ln2
//
#define LN2_HI  0x1.62e000p-1f
#define LN2_LO  0x1.0bfbe8p-15f
#define INV_LN2 0x1.715476p+0f

// log1p(f) = f - hfsq + s*(hfsq + R), s = f/(2+f), R = z*L(z), z = s*s
#define L0  0x1.555566p-1f
#define L1  0x1.996298p-2f
#define L2  0x1.32bed2p-2f

// exp(r) = 1 + r + r*r*P(r) on |r| <= ln2/2, minimax fit
#define P0  0x1.000000p-1f
#define P1  0x1.5554dcp-3f
#define P2  0x1.55551ap-5f
#define P3  0x1.120b6cp-7f
#define P4  0x1.6d1106p-10f

// 12 mantissa bits, so a product of two such values is exact
static float hipart(float v)
{
  union {
    float f;
    uint32_t u;
  } c;
  c.f = v;
  c.u &= 0xfffff000u;
  return c.f;
}

static void log_hl(float x, float *hi, float *lo)
{
  int e;
  float m = frexpf(x, &e);

  // Precise range reduction: [0.75, 1.5)
  if (m < 0.75f) {
    m *= 2.0f;
    e--;
  }

  float f = m - 1.0f;
  float s = f / (2.0f + f);
  float z = s * s;
  float hfsq = 0.5f * f * f;
  float R = z * (L0 + z * (L1 + z * L2));
  float e_f = (float)e;

  float t = f - (hfsq - s * (hfsq + R));
  float th = hipart(t);
  float tl = (f - th) - (hfsq - (s * (hfsq + R)));

  float h = e_f * LN2_HI + th;
  *hi = hipart(h);
  *lo = (e_f * LN2_HI - *hi) + th + (tl + e_f * LN2_LO);
}

static float exp_hl(float hi, float lo)
{
  float n_f = roundf((hi + lo) * INV_LN2);
  int n = (int)n_f;
  float r = ((hi - n_f * LN2_HI) - n_f * LN2_LO) + lo;
  float r2 = r * r;
  float p = P0 + r * (P1 + r * (P2 + r * (P3 + r * P4)));
  float t = 1.0f + (r + r2 * p);
  return ldexpf(t, n);
}

float powf(float x, float y)
{
  // C99 F.10.4.4: these two win over a NaN operand
  if (y == 0.0f) {
    return 1.0f;
  }
  if (x == 1.0f) {
    return 1.0f;
  }

  if (isnan(x) || isnan(y)) {
    return NAN;
  }

  int is_int = 0;
  int is_odd = 0;
  float ay = fabsf(y);
  if (ay >= 0x1p24f) {
    is_int = 1;
  } else if (ay == floorf(ay)) {
    is_int = 1;
    is_odd = (int)ay & 1;
  }

  int neg = signbit(x) && is_odd;

  if (x == 0.0f) {
    if (y < 0.0f) {
      return neg ? -INFINITY : INFINITY;
    }
    return neg ? -0.0f : 0.0f;
  }

  if (isinf(y)) {
    if (fabsf(x) == 1.0f) {
      return 1.0f;
    }
    if (fabsf(x) > 1.0f) {
      return (y > 0.0f) ? INFINITY : 0.0f;
    }
    return (y > 0.0f) ? 0.0f : INFINITY;
  }

  if (isinf(x)) {
    if (y > 0.0f) {
      return neg ? -INFINITY : INFINITY;
    }
    return neg ? -0.0f : 0.0f;
  }

  // 1.0, 2.0, 0.5 and -1.0 come out exact this way, and comparing the bit
  // pattern is cheaper than four float compares
  union {
    float f;
    uint32_t u;
  } yb;
  yb.f = y;
  switch (yb.u) {
  case 0x3f800000u:
    return x;
  case 0x40000000u:
    return x * x;
  case 0x3f000000u:
    if (x > 0.0f) {
      return sqrtf(x);
    }
    break;
  case 0xbf800000u:
    return 1.0f / x;
  }

  if (x < 0.0f) {
    if (!is_int) {
      return NAN;
    }
    x = -x;
  }

  float lh, ll;
  log_hl(x, &lh, &ll);

  float yh = hipart(y);
  float yl = y - yh;

  // yh * lh is exact: 12 bits times 12 bits
  float th = yh * lh;
  float tl = yl * lh + y * ll;

  if (th + tl > 88.722839f) {
    return neg ? -INFINITY : INFINITY;
  }
  if (th + tl < -103.972084f) {
    return neg ? -0.0f : 0.0f;
  }

  float r = exp_hl(th, tl);
  return neg ? -r : r;
}
