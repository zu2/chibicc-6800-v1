#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "my_printf.c"

typedef union {
  float f;
  uint32_t u;
} FloatUnion;

typedef struct {
  uint32_t input;
  uint32_t expected;
} SqrtTestCase;

SqrtTestCase test_cases[83] = {
    {0x00000000u, 0x00000000u}, // 0f -> 0f
    {0x80000000u, 0x80000000u}, // -0f -> -0f
    {0x00800000u, 0x20000000u}, // 1.1754944e-38f -> 1.0842022e-19f
    {0x006CE3EEu, 0x1FEC1E4Au}, // 9.9999994e-39f -> 9.9999997e-20f
    {0x1E3CE508u, 0x2EDBE6FFu}, // 9.9999997e-21f -> 1e-10f
    {0x2EDBE6FFu, 0x3727C5ACu}, // 1e-10f -> 9.9999997e-06f
    {0x3727C5ACu, 0x3B4F3E37u}, // 9.9999997e-06f -> 0.0031622776f
    {0x3E800000u, 0x3F000000u}, // 0.25f -> 0.5f
    {0x3F000000u, 0x3F3504F3u}, // 0.5f -> 0.70710677f
    {0x3F3504F3u, 0x3F5744FDu}, // 0.70710677f -> 0.84089643f
    {0x3F7FFFFEu, 0x3F7FFFFFu}, // 0.99999988f -> 0.99999994f
    {0x3F800000u, 0x3F800000u}, // 1f -> 1f
    {0x3F800001u, 0x3F800000u}, // 1.0000001f -> 1f
    {0x3FC00000u, 0x3F9CC471u}, // 1.5f -> 1.2247449f
    {0x3FFFFFFFu, 0x3FB504F3u}, // 1.9999999f -> 1.4142135f
    {0x40000000u, 0x3FB504F3u}, // 2f -> 1.4142135f
    {0x40400000u, 0x3FDDB3D7u}, // 3f -> 1.7320508f
    {0x40800000u, 0x40000000u}, // 4f -> 2f
    {0x41200000u, 0x404A62C2u}, // 10f -> 3.1622777f
    {0x42C80000u, 0x41200000u}, // 100f -> 10f
    {0x461C4000u, 0x42C80000u}, // 10000f -> 100f
    {0x501502F9u, 0x47C35000u}, // 1e+10f -> 100000f
    {0x60AD78ECu, 0x501502F9u}, // 1e+20f -> 1e+10f
    {0x7149F2CAu, 0x58635FA9u}, // 1e+30f -> 9.9999999e+14f
    {0x7E967699u, 0x5F0AC723u}, // 9.9999997e+37f -> 1e+19f
    {0x7F7FFFFFu, 0x5F7FFFFFu}, // 3.4028235e+38f -> 1.8446743e+19f
    {0xBF800000u, 0x7FC00000u}, // -1f -> nanf
    {0x80800000u, 0x7FC00000u}, // -1.1754944e-38f -> nanf
    {0x806CE3EEu, 0x7FC00000u}, // -9.9999994e-39f -> nanf
    {0xC2C80000u, 0x7FC00000u}, // -100f -> nanf
    {0x7F800000u, 0x7F800000u}, // inff -> inff
    {0xFF800000u, 0x7FC00000u}, // -inff -> nanf
    {0x7FC00000u, 0x7FC00000u}, // nanf -> nanf
    {0x4B7FFFFBu, 0x457FFFFDu}, // 16777211f -> 4095.9993f
    {0x4B7FFFFCu, 0x457FFFFEu}, // 16777212f -> 4095.9995f
    {0x4B7FFFFDu, 0x457FFFFEu}, // 16777213f -> 4095.9995f
    {0x4B7FFFFEu, 0x457FFFFFu}, // 16777214f -> 4095.9998f
    {0x4B7FFFFFu, 0x457FFFFFu}, // 16777215f -> 4095.9998f
    {0x4B800000u, 0x45800000u}, // 16777216f -> 4096f
    {0x4B800001u, 0x45800000u}, // 16777218f -> 4096f
    {0x4B800002u, 0x45800001u}, // 16777220f -> 4096.0005f
    {0x3FB504F3u, 0x3F9837F0u}, // 1.4142135f -> 1.1892071f
    {0x3FDDB3D7u, 0x3FA8751Du}, // 1.7320508f -> 1.316074f
    {0x400F1BBDu, 0x3FBF6797u}, // 2.236068f -> 1.4953488f
    {0x401CC471u, 0x3FC854B1u}, // 2.4494898f -> 1.5650846f
    {0x402953FDu, 0x3FD033A9u}, // 2.6457512f -> 1.6265765f
    {0x403504F3u, 0x3FD744FDu}, // 2.8284271f -> 1.6817929f
    {0x40544395u, 0x3FE91BC8u}, // 3.3166249f -> 1.8211603f
    {0x4066C15Au, 0x3FF30CD3u}, // 3.6055512f -> 1.8988289f
    {0x4083F07Bu, 0x4001F46Bu}, // 4.1231055f -> 2.0305431f
    {0x408B7C1Au, 0x40059E7Au}, // 4.3588991f -> 2.0877976f
    {0x40997774u, 0x400C27F5u}, // 4.7958317f -> 2.1899388f
    {0x3FE2DFC5u, 0x3FAA6933u}, // 1.7724539f -> 1.3313354f
    {0x3FD3094Cu, 0x3FA45AF2u}, // 1.6487212f -> 1.2840254f
    {0x3EA1E89Bu, 0x3F0FF59Au}, // 0.31622776f -> 0.56234133f
    {0x3DCCCCCDu, 0x3EA1E89Bu}, // 0.1f -> 0.31622776f
    {0x3F9CC471u, 0x3F8DA7C1u}, // 1.2247449f -> 1.1066819f
    {0x3FCA62C2u, 0x3FA0F394u}, // 1.5811388f -> 1.2574334f
    {0x3FEF7751u, 0x3FAF137Fu}, // 1.8708287f -> 1.3677825f
    {0x3F13CD3Au, 0x3F428497u}, // 0.57735026f -> 0.75983566f
    {0x3EE4F92Eu, 0x3F2B3290u}, // 0.44721359f -> 0.66874027f
    {0x40490FDBu, 0x3FE2DFC5u}, // 3.1415927f -> 1.7724539f
    {0x402DF854u, 0x3FD3094Cu}, // 2.7182817f -> 1.6487212f
    {0x40C90FDBu, 0x40206C99u}, // 6.2831855f -> 2.5066283f
    {0x40ADF854u, 0x401539A8u}, // 5.4365635f -> 2.3316441f
    {0x3FC90FDBu, 0x3FA06C99u}, // 1.5707964f -> 1.2533141f
    {0x3FADF854u, 0x3F9539A8u}, // 1.3591409f -> 1.165822f
    {0x411DE9E6u, 0x40490FDAu}, // 9.8696041f -> 3.1415925f
    {0x40EC7326u, 0x402DF854u}, // 7.3890562f -> 2.7182817f
    {0x403B06ADu, 0x3FDACFDDu}, // 2.9222825f -> 1.7094685f
    {0x404A62C2u, 0x3FE39EA9u}, // 3.1622777f -> 1.7782794f
    {0x3D0186E2u, 0x3E361887u}, // 0.031622775f -> 0.17782794f
    {0x3F13BA4Eu, 0x3F427823u}, // 0.57706153f -> 0.75964564f
    {0x3F50EB29u, 0x3F6743A7u}, // 0.81608826f -> 0.90337604f
    {0x3F9837F0u, 0x3F8B95C2u}, // 1.1892071f -> 1.0905077f
    {0x4B000000u, 0x453504F3u}, // 8388608f -> 2896.3093f
    {0x4AFFFFFEu, 0x453504F2u}, // 8388607f -> 2896.3091f
    {0x4B000001u, 0x453504F4u}, // 8388609f -> 2896.3096f
    {0x4C000000u, 0x45B504F3u}, // 33554432f -> 5792.6187f
    {0x4BFFFFFFu, 0x45B504F3u}, // 33554430f -> 5792.6187f
    {0x49800000u, 0x44800000u}, // 1048576f -> 1024f
    {0x497FFFF0u, 0x447FFFF8u}, // 1048575f -> 1023.9995f
    {0x49800008u, 0x44800004u}, // 1048577f -> 1024.0005f
};

float custom_sqrtf(float x)
{
  if (x < 0.0f) {
    return NAN;
  }
  if (x == 0.0f || isinf(x)) {
    return x;
  }

  union {
    float f;
    uint32_t i;
  } v = {x};
  v.i = (v.i >> 1) + 0x1fc00000;
  float approx = v.f;

  for (int i = 0; i < 3; ++i) {
    approx = 0.5f * (approx + x / approx);
  }

  return approx;
}

int run_tests()
{
  int fail = 0;
  for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); ++i) {
    float x = *(float *)&test_cases[i].input;
    float expected = *(float *)&test_cases[i].expected;
    //  float actual = custom_sqrtf(x);
    float actual = sqrtf(x);
    uint32_t actual_bits = *(uint32_t *)&actual;

    if (actual_bits != test_cases[i].expected) {
      printf("Test failed: input=0x%08lx, expected=0x%08lx, actual=0x%08lx\n",
             test_cases[i].input, test_cases[i].expected, actual_bits);
      printf("             input=%08e, expected=%08e, actual=%08e\n", x,
             expected, actual);
      fail++;
    }
  }
  return fail;
}

int main()
{
  if (run_tests()) {
    return 1;
  }

  return 0;
}
