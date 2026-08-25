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

// log1p(f) = f - hfsq + s*(hfsq + R), s = f/(2+f), R = z*L(z), z = s*s
#define L0  0x1.555566p-1f
#define L1  0x1.996298p-2f
#define L2  0x1.32bed2p-2f

// 2^r - 1 = r*C(r) on |r| <= 1/32, Taylor in ln2
#define KEXP 4
// 1/ln2 split so that th * INV_HI is exact: 12 bits times 12 bits
#define INV_HI  0x1.7140000000000p+0f
#define INV_LO  0x1.47652c0000000p-12f
#define INV_LN2 0x1.7154760000000p+0f
#define KSCALE  0x1.p+4f
#define KSTEP   0x1.p-4f
static const float ehi[16] = {
  0x1.p+0f, 0x1.0b55860000000p+0f, 0x1.172b840000000p+0f, 0x1.2387a60000000p+0f,
  0x1.306fe00000000p+0f, 0x1.3dea640000000p+0f, 0x1.4bfdae0000000p+0f, 0x1.5ab07e0000000p+0f,
  0x1.6a09e60000000p+0f, 0x1.7a11480000000p+0f, 0x1.8ace540000000p+0f, 0x1.9c49180000000p+0f,
  0x1.ae89fa0000000p+0f, 0x1.c199be0000000p+0f, 0x1.d5818e0000000p+0f, 0x1.ea4afa0000000p+0f,
};
// 表 64 バイト

// 12 mantissa bits, so a product of two such values is exact
#define CLEAR_LOW12(v) (*(uint32_t *)&(v) &= 0xfffff000u)

// log2 x = lh + ll
//
// e is an integer, so it joins the result with an exact add and no split of
// ln2 is needed. The price is the change of base below.
static float lh, ll;

static void log2_hl(float x)
{
  static int e;
  static float m, f, s, z, hfsq, th, h, u, ph;
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
  u = hfsq - s * (hfsq + z * (L0 + z * (L1 + z * L2)));
  th = f - u;
  CLEAR_LOW12(th);

  // log m = th + ((f - th) - u), in natural units; change the base
  ph = th * INV_HI;
  // z is free now, so it holds the low half of the change of base
  z = th * INV_LO + ((f - th) - u) * INV_LN2;

  // m is free now, so it holds (float)e
  m = (float)e;
  h = m + ph;
  lh = h;
  CLEAR_LOW12(lh);
  ll = ((m - lh) + ph) + z;
}

// 2^(hi + lo)
//
// n2 and n2/2^KEXP are both exact, so the range reduction costs one multiply
// and one subtract. ehi[j] stays unrounded; only ehi[j] * u is rounded.
static float exp2_hl(float hi, float lo)
{
  static float n_f, r, c;
  static int n2, j;

  n_f = roundf((hi + lo) * KSCALE);
  n2 = (int)n_f;
  j = n2 & 15;
  n2 >>= KEXP;

  r = (hi - n_f * KSTEP) + lo;
  c = ehi[j];
  return ldexpf(c + c * (r * (0x1.62e4300000000p-1f + r * (0x1.ebfbe00000000p-3f + r * (0x1.c6b08e0000000p-5f)))), n2);
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

  // every exit below shares one tail instead of branching on the sign
  static float sb;
  HIGH16(sb) = (signbit(x) && is_odd) ? 0x8000u : 0u;
  static float ret;

  // if (x == 0.0f) {
  if (ax == 0u && LOW16(x) == 0u) {
    // if (y < 0.0f) {
    ret = signbit(y) ? INFINITY : 0.0f;
    goto signed_ret;
  }

  // if (isinf(y)) {
  if (ay_w == W_INF && LOW16(y) == 0u) {
    // if (fabsf(x) == 1.0f) {
    if (ax == W_ONE && LOW16(x) == 0u) {
      return 1.0f;
    }
    // |x| == 1 left above, so ax >= W_ONE now means |x| > 1
    if (ax >= W_ONE) {
      return signbit(y) ? 0.0f : INFINITY;
    }
    return signbit(y) ? INFINITY : 0.0f;
  }

  // if (isinf(x)) {
  if (ax == W_INF && LOW16(x) == 0u) {
    // if (y > 0.0f) { the result is an infinity with the sign of neg }
    ret = signbit(y) ? 0.0f : INFINITY;
    goto signed_ret;
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

  log2_hl(x);

  // float yh = hipart(y);
  static float yh;
  yh = y;
  CLEAR_LOW12(yh);

  // yh * lh is exact: 12 bits times 12 bits
  static float th, tl;
  th = yh * lh;
  tl = (y - yh) * lh + y * ll;

  // if (th + tl > 88.722839f) / if (th + tl < -103.972084f)
  static float sum;
  sum = th + tl;
  // log2(FLT_MAX)
  if (sum > 128.0f) {
    ret = INFINITY;
    goto signed_ret;
  }
  // log2 of half the smallest subnormal: below this 2^v underflows to 0
  if (sum < -150.0f) {
    ret = 0.0f;
    goto signed_ret;
  }

  ret = exp2_hl(th, tl);

signed_ret:
  return copysignf(ret, sb);
}
