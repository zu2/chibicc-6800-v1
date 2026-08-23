//
// powf(x, y) = exp(y * log x)
//
// exp magnifies errors in its argument into errors in the result
// One float is not accurate enough for y * log x
// |y * log x| can reach about 104,
//   so one float rounds with an error of about 2^-17
// So log x and y * log x are kept as two floats, hi + lo
// CLEAR_LOW12 is a Veltkamp split that keeps 12 high bits
// 12 bits times 12 bits fits in 24 bits, so yh * lh is exact
//

#include <math.h>
#include <stdint.h>

union fword {
  float f;
  unsigned int w[2];
};

// 6800 stores the high half first, so word 0 holds the sign and the exponent
#define HIGH16(v) (*(unsigned int *)&(v))
#define LOW16(v) (*((unsigned int *)&(v) + 1))

#define W_INF   0x7f80u  // INFINITY
#define W_ONE   0x3f80u  // 1.0f
#define W_TWO   0x4000u  // 2.0f
#define W_HALF  0x3f00u  // 0.5f
#define W_MONE  0xbf80u  // -1.0f
#define W_2P24  0x4b80u  // 0x1p24f

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
#define CLEAR_LOW12(v) (*(uint32_t *)&(v) &= 0xfffff000u)

// log x = *hi + *lo
static void log_hl(float x, float *hi, float *lo)
{
  static int e;
  static float m, f, s, z, hfsq, R, e_f, t, th, tl, h, sq, eh, u;
  m = frexpf(x, &e);

  // Precise range reduction: [0.75, 1.5)
  if (m < 0.75f) {
    m *= 2.0f;
    e--;
  }

  f = m - 1.0f;
  s = f / (2.0f + f);
  z = s * s;
  hfsq = 0.5f * f * f;
  R = z * (L0 + z * (L1 + z * L2));
  e_f = (float)e;

  // t = f - (hfsq - s * (hfsq + R));
  sq = s * (hfsq + R);
  u = hfsq - sq;
  t = f - u;
  // th = hipart(t);
  th = t;
  CLEAR_LOW12(th);
  // tl = (f - th) - (hfsq - (s * (hfsq + R)));
  tl = (f - th) - u;

  // h = e_f * LN2_HI + th;
  eh = e_f * LN2_HI;
  h = eh + th;
  // *hi = hipart(h);
  *hi = h;
  CLEAR_LOW12(*hi);
  // *lo = (e_f * LN2_HI - *hi) + th + (tl + e_f * LN2_LO);
  *lo = (eh - *hi) + th + (tl + e_f * LN2_LO);
}

// exp(hi + lo)
static float exp_hl(float hi, float lo)
{
  static float n_f, r, r2, p, t;
  static int n;
  n_f = roundf((hi + lo) * INV_LN2);
  n = (int)n_f;
  r = ((hi - n_f * LN2_HI) - n_f * LN2_LO) + lo;
  r2 = r * r;
  p = P0 + r * (P1 + r * (P2 + r * (P3 + r * P4)));
  t = 1.0f + (r + r2 * p);
  return ldexpf(t, n);
}

float powf(float x, float y)
{
  static union fword ab;
  static unsigned int ax, ay_w;

  ax   = HIGH16(x) & 0x7fffu;
  ay_w = HIGH16(y) & 0x7fffu;

  // C99 F.10.4.4: these two win over a NaN operand
  // if (y == 0.0f) {
  if (ay_w == 0u && LOW16(y) == 0u) {
    return 1.0f;
  }
  // if (x == 1.0f) {
  if (HIGH16(x) == W_ONE && LOW16(x) == 0u) {
    return 1.0f;
  }

  // if (isnan(x) || isnan(y)) {
  if (ax > W_INF || (ax == W_INF && LOW16(x) != 0u)
  ||  ay_w > W_INF || (ay_w == W_INF && LOW16(y) != 0u)) {
    return NAN;
  }

  static int is_int, is_odd;
  is_int = 0;
  is_odd = 0;
  // float ay = fabsf(y);
  ab.w[0] = ay_w;
  ab.w[1] = LOW16(y);
  static float ay;
  ay = ab.f;
  // if (ay >= 0x1p24f) {
  if (ay_w >= W_2P24) {
    is_int = 1;
  } else if (ay == floorf(ay)) {
    is_int = 1;
    // was is_odd = (int)ay & 1; (int) saturates at 32767 on 6800
    is_odd = (long)ay & 1L;
  }

  static int neg;
  neg = signbit(x) && is_odd;

  // if (x == 0.0f) {
  if (ax == 0u && LOW16(x) == 0u) {
    // if (y < 0.0f) {
    if (signbit(y)) {
      return neg ? -INFINITY : INFINITY;
    }
    return neg ? -0.0f : 0.0f;
  }

  // if (isinf(y)) {
  if (ay_w == W_INF && LOW16(y) == 0u) {
    // if (fabsf(x) == 1.0f) {
    if (ax == W_ONE && LOW16(x) == 0u) {
      return 1.0f;
    }
    // if (fabsf(x) > 1.0f) {
    if (ax > W_ONE || (ax == W_ONE && LOW16(x) != 0u)) {
      // return (y > 0.0f) ? INFINITY : 0.0f;
      return signbit(y) ? 0.0f : INFINITY;
    }
    // return (y > 0.0f) ? 0.0f : INFINITY;
    return signbit(y) ? INFINITY : 0.0f;
  }

  // if (isinf(x)) {
  if (ax == W_INF && LOW16(x) == 0u) {
    // if (y > 0.0f) { return neg ? -INFINITY : INFINITY; }
    if (signbit(y)) {
      return neg ? -0.0f : 0.0f;
    }
    return neg ? -INFINITY : INFINITY;
  }

  // 1.0, 2.0, 0.5 and -1.0 come out exact this way, and comparing the bit
  // pattern is cheaper than four float compares
  // yb.f = y; switch (yb.u) {
  if (LOW16(y) == 0u) {
    switch (HIGH16(y)) {
    case W_ONE:
      return x;
    case W_TWO:
      return x * x;
    case W_HALF:
      // if (x > 0.0f) {
      if (!signbit(x)) {
        return sqrtf(x);
      }
      break;
    case W_MONE:
      return 1.0f / x;
    }
  }

  // if (x < 0.0f) {
  if (signbit(x)) {
    if (!is_int) {
      return NAN;
    }
    // x = -x;
    HIGH16(x) = ax;
  }

  static float lh, ll;
  log_hl(x, &lh, &ll);

  // float yh = hipart(y);
  static float yh;
  yh = y;
  CLEAR_LOW12(yh);
  static float yl;
  yl = y - yh;

  // yh * lh is exact: 12 bits times 12 bits
  static float th;
  th = yh * lh;
  static float tl;
  tl = yl * lh + y * ll;

  // if (th + tl > 88.722839f) / if (th + tl < -103.972084f)
  static float sum;
  sum = th + tl;
  // ln(FLT_MAX)
  if (sum > 88.722839f) {
    return neg ? -INFINITY : INFINITY;
  }
  // ln(2^-150), half the smallest subnormal: below this exp underflows to 0
  if (sum < -103.972084f) {
    return neg ? -0.0f : 0.0f;
  }

  static float r;
  r = exp_hl(th, tl);
  return neg ? -r : r;
}
