#include  <math.h>
#include  <float.h>

float log1pf(float x)
{
    if (fabsf(x) > 0.375f) {
        return logf(1.0f + x);
    } else {
        // Use a 6-term Taylor series for better accuracy with small x
        float term = x;
        float sum = x;
        for (int n = 2; n <= 6; ++n) {
            term *= -x;
            sum += term / n;
        }
        return sum;
    }
}
