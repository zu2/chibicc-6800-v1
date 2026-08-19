//
//	float library
//

#include <math.h>
#include <stdint.h>

#define to_bits(x) (*(const uint32_t*)&(float){x})

// A hex float literal for a float carries 6 digits after the point. A 7th
// digit is below the 24 bit significand and is dropped, so the value stops
// matching the decimal in the comment
int main(int argc, char **argv)
{
	if (to_bits(M_PI) != 0x40490fdbUL)
		return 10;
	if (to_bits(M_TWOPI) != 0x40c90fdbUL)
		return 20;
	if (to_bits(M_PI_2) != 0x3fc90fdbUL)
		return 30;
	if (to_bits(M_PI_4) != 0x3f490fdbUL)
		return 40;
	if (to_bits(M_PI_8) != 0x3ec90fdbUL)
		return 50;
	if (to_bits(M_1_PI) != 0x3ea2f983UL)
		return 60;
	if (to_bits(M_2_PI) != 0x3f22f983UL)
		return 70;
	if (to_bits(M_2_SQRTPI) != 0x3f906ebbUL)
		return 80;
	if (to_bits(M_E) != 0x402df854UL)
		return 90;
	if (to_bits(M_LOG2E) != 0x3fb8aa3bUL)
		return 100;
	if (to_bits(M_LOG10E) != 0x3ede5bd9UL)
		return 110;
	if (to_bits(M_LN2) != 0x3f317218UL)
		return 120;
	if (to_bits(M_LN10) != 0x40135d8eUL)
		return 130;
	if (to_bits(M_SQRT2) != 0x3fb504f3UL)
		return 140;
	if (to_bits(M_SQRT1_2) != 0x3f3504f3UL)
		return 150;

	return 0;
}
