#include <math.h>

#define TWO_OVER_PI  0x1.45f306p-1f

// Cody-Waite: PIO2_0 to PIO2_2 hold 8 bits each, so n * PIO2_k stays exact
// while n < 2^16. PIO2_3 carries the tail
#define PIO2_0  0x1.920000p+0f
#define PIO2_1  0x1.fa0000p-12f
#define PIO2_2  0x1.540000p-20f
#define PIO2_3  0x1.10b462p-30f

// tan(r) = r + r*u*A(u)/B(u) on |r| < W_TAN_SMALL, u = r*r, minimax fit
// Few 1 bits on purpose: the software multiply costs 25 cycles per 1 bit
#define A0  0x1.555554p-2f
#define A1  -0x1.e8a580p-7f
#define B1  -0x1.c76924p-2f
#define B2  0x1.05fc40p-6f

// cot(r) = 1/r - r*C(u) on |r| < W_COT_SMALL - pi/2, u = r*r, minimax fit
#define C0  0x1.555556p-2f
#define C1  0x1.6c169ap-6f
#define C2  0x1.15762ep-9f
#define C3  0x1.b78192p-13f
#define C4  0x1.a4dccap-16f

// The band tests read the top 16 bits of x, so every threshold sits on a
// 16 bit boundary
#define W_TAN_SMALL  0x3f86u   // 1.0469, well past pi/4: see the fit range
#define W_COT_SMALL  0x4016u   // 2.34375, the last x that reduces with n == 1
#define W_PH         0x47c9u   // n nears 2^16, where Cody-Waite stops being exact
#define W_INF        0x7f80u   // NaN and infinity are at or above this

int __rem_pio2f(float x, float *rp);
float __polyf(float x, const float *c, int n);

union fword {
  float f;
  unsigned int w[2];
};

static const float AA[2] = {
  A1, A0,
};

static const float BB[3] = {
  B2, B1, 1.0f,
};

static float tan_small(float r)
{
  static float u, b, w;

  u = r * r;
  b = __polyf(u, BB, 2);
  w = u * __polyf(u, AA, 1);
  return r + r * (w / b);
}

static const float CC[5] = {
  C4, C3, C2, C1, C0,
};

static float cot_small(float r)
{
  static float u;

  u = r * r;
  return -(1.0f / r) + r * __polyf(u, CC, 4);
}

float tanf(float x)
{
  union fword b;
  unsigned int w;

  b.f = x;
  w = b.w[0] & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && b.w[1] == 0u) ? NAN : x;
  }

  // Below pi/4 the quadrant is 0, so the reduction would only subtract zeros
  if (w < W_TAN_SMALL) {
    return tan_small(x);
  }
  // Up to 3*pi/4 the quadrant is 1, so roundf and the four products drop out
  if (w < W_COT_SMALL) {
    float r1 = x - copysignf(PIO2_0, x);
    r1 = r1 - copysignf(PIO2_1, x);
    r1 = r1 - copysignf(PIO2_2, x);
    r1 = r1 - copysignf(PIO2_3, x);
    return cot_small(r1);
  }

  float r;
  long n;

  if (w >= W_PH) {
    n = __rem_pio2f(x, &r);
  } else {
    float n_f = roundf(x * TWO_OVER_PI);
    n = (long)n_f;
    r = x - n_f * PIO2_0;
    r = r - n_f * PIO2_1;
    r = r - n_f * PIO2_2;
    r = r - n_f * PIO2_3;
  }

  if (n & 1) {
    return cot_small(r);
  }
  return tan_small(r);
}
