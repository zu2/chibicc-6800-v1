#include <math.h>


// 2^r = 1 + r*P(r) on |r| <= 0.5, minimax fit
#define P0  0x1.62e430p-1f
#define P1  0x1.ebfbccp-3f
#define P2  0x1.c6af86p-5f
#define P3  0x1.3b3b8ap-7f
#define P4  0x1.5f08e4p-10f
#define P5  0x1.3820d6p-13f

static const float PP[6] = {
  P5, P4, P3, P2, P1, P0,
};

#define HIGH16(v) (*(unsigned int *)&(v))

// p holds exponent 126 or 127, so n<<7 keeps the field inside 1..254 only here
#define N_LO  (-125)
#define N_HI  127

float exp2f(float x)
{
  if (isnan(x)) {
    return x;
  }
  if (x >= 128.0f) {
    return INFINITY;
  }
  if (x < -150.0f) {
    return 0.0f;
  }

  float n_f = roundf(x);
  int n = (int)n_f;
  float r = x - n_f;

  float p = 1.0f + r * __polyf(r, PP, 5);

  if (n < N_LO || n > N_HI) {
    return ldexpf(p, n);
  }
  HIGH16(p) += (unsigned int)(n << 7);
  return p;
}
