#include <math.h>


// log2(m) = s*Q(s*s), s = (m-1)/(m+1), minimax fit over the reduced range
#define L0  0x1.715476p+1f
#define L1  0x1.ec70aep-1f
#define L2  0x1.275c92p-1f
#define L3  0x1.b697aap-2f

static const float LL[4] = {
  L3, L2, L1, L0,
};

#define HIGH16(v) (*(unsigned int *)&(v))
#define LOW16(v)  (*((unsigned int *)&(v) + 1))

#define W_INF     0x7f80u   // NaN and infinity are at or above this
#define W_NORMAL  0x0080u   // smallest normal number
#define W_SQRT2   0x3fb5u   // the split point, rounded down to a 16 bit boundary
#define W_ONE     0x3f80u   // 1.0
#define W_STEP    0x0080u   // one step of the exponent field

float log2f(float x)
{
  unsigned int w, a;

  w = HIGH16(x);
  a = w & 0x7fffu;
  if (a >= W_INF) {
    if (a == W_INF && LOW16(x) == 0u && (w & 0x8000u) == 0u) {
      return INFINITY;
    }
    return NAN;
  }
  if (a == 0u && LOW16(x) == 0u) {
    return -INFINITY;
  }
  if (w & 0x8000u) {
    return NAN;
  }

  int e = ilogbf(x);
  float m = (a < W_NORMAL) ? ldexpf(x, -e) : x;

  // Precise range reduction: [0x1.6ap-1, 0x1.6ap+0)
  HIGH16(m) = (HIGH16(m) & 0x007fu) | W_ONE;
  if (HIGH16(m) >= W_SQRT2) {
    HIGH16(m) -= W_STEP;
    e++;
  }

  float s = (m - 1.0f) / (m + 1.0f);
  float p = s * __polyf(s * s, LL, 3);

  if (e == 0) {
    return p;
  }
  return (float)e + p;
}
