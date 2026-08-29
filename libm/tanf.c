#include <math.h>

// tan(r) = r + r*u*A(u)/B(u) on |r| < W_TAN_SMALL, u = r*r, minimax fit
// Few 1 bits on purpose: the software multiply costs 25 cycles per 1 bit
#define A0  0x1.555554p-2f
#define A1  -0x1.e8a580p-7f
#define B1  -0x1.c76924p-2f
#define B2  0x1.05fc40p-6f

// cot(r) = 1/r - r*C(u) on |r| < 0.7735, u = r*r, minimax fit
#define C0  0x1.555556p-2f
#define C1  0x1.6c169ap-6f
#define C2  0x1.15762ep-9f
#define C3  0x1.b78192p-13f
#define C4  0x1.a4dccap-16f

// The band tests read the top 16 bits of x, so every threshold sits on a
// 16 bit boundary
#define W_TAN_SMALL  0x3f86u   // 1.0469, well past pi/4: see the fit range
#define W_INF        0x7f80u   // NaN and infinity are at or above this


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
  unsigned int w;

  w = *(unsigned int *)&x & 0x7fffu;
  if (w >= W_INF) {
    return (w == W_INF && *((unsigned int *)&x + 1) == 0u) ? NAN : x;
  }

  // Below pi/4 the quadrant is 0, so the reduction would only subtract zeros
  if (w < W_TAN_SMALL) {
    return tan_small(x);
  }
  float r;
  int n = __reduce_pio2f(x, w, &r);

  if (n & 1) {
    return cot_small(r);
  }
  return tan_small(r);
}
