#include "common.h"

#define uint32_t	unsigned long

// === ビットパターン定数 ===
#define HEX_FLT_TRUE_MIN   (0x00000001)  // 最小サブノーマル
#define HEX_FLT_TRUE_MIN2  (0x00000002)  // サブノーマル*2
#define HEX_FLT_MIN        (0x00800000)  // 最小正規化数
#define HEX_FLT_MAX        (0x7f7fffff)  // 最大正規化数
#define HEX_NEG_MAX        (0xff7fffff)  // -FLT_MAX

// === ビット操作用ユニオン ===
typedef union {
    float f;
    uint32_t u;
} float_union;

// === ユーティリティ関数 ===
float to_float(uint32_t x) {
    return *(float *)&x;
    float_union fu;
    fu.u = x;
    return fu.f;
}

int float_signbit(float x) {
    float_union fu = {x};
    return (fu.u >> 31) & 1;
}

int float_isnan(float x) {
    float_union fu = {x};
    return ((fu.u & 0x7F800000) == 0x7F800000) && (fu.u & 0x007FFFFF);
}

int float_isinf(float x) {
    float_union fu = {x};
    return ((fu.u & 0x7F800000) == 0x7F800000) && !(fu.u & 0x007FFFFF);
}

int compare_float(float a, float b) {
    if (float_isnan(a) && float_isnan(b)) return 1;
    if (float_isnan(a) || float_isnan(b)) return 0;
    if (float_isinf(a) && float_isinf(b) && (float_signbit(a) == float_signbit(b))) return 1;
    if (float_isinf(a) || float_isinf(b)) return 0;

    float_union fa = {a}, fb = {b};
    uint32_t diff = (fa.u > fb.u) ? fa.u - fb.u : fb.u - fa.u;
    return (diff <= 1) || (diff < 0x00800000);
}

// === 加算テスト (1-20) ===
int test_addition() {
    // 基本加算
//    if (3.0f != 1.0f + 2.0f) return 1;
  
    // サブノーマル加算
    if (to_float(HEX_FLT_TRUE_MIN2) != to_float(HEX_FLT_TRUE_MIN) + to_float(HEX_FLT_TRUE_MIN)) return 2;

    // オーバーフロー
//    if (!float_isinf(to_float(HEX_FLT_MAX) + to_float(HEX_FLT_MAX))) return 3;

    // 正規化←→サブノーマル境界
//    if (to_float(HEX_FLT_MIN) != to_float(HEX_FLT_TRUE_MIN) + to_float(HEX_FLT_MIN)) return 4;

    return 0;
}

// === 減算テスト (21-40) ===
int test_subtraction() {
#if 0
    // 基本減算
    if (2.0f != 5.0f - 3.0f) return 21;

    // サブノーマル減算
    if (to_float(HEX_FLT_TRUE_MIN) != to_float(HEX_FLT_TRUE_MIN2) - to_float(HEX_FLT_TRUE_MIN)) return 22;

    // 負方向オーバーフロー
    if (!float_isinf(to_float(HEX_NEG_MAX) - to_float(HEX_FLT_MAX))) return 23;
#endif
    // 正規化→サブノーマル遷移
    if (to_float(HEX_FLT_MIN/2) != to_float(HEX_FLT_MIN) - to_float(HEX_FLT_MIN/2)) return 24;

    return 0;
}

// === 乗算テスト (41-60) ===
int test_multiplication() {
    // 基本乗算
    if (6.0f != 2.0f * 3.0f) return 41;

    // サブノーマル生成
    if (to_float(0x00400000) != to_float(HEX_FLT_MIN) * 0.5f) return 42;

    // オーバーフロー
    if (!float_isinf(to_float(HEX_FLT_MAX) * 2.0f)) return 43;

    // オーバーフロー2 (指数だけでは判断できない場合）
    if (!float_isinf(to_float(HEX_FLT_MAX)*to_float(0x3FFFFFFF))) return 44;

    // サブノーマル→正規化
    if (to_float(HEX_FLT_MIN) != to_float(HEX_FLT_TRUE_MIN) * 8388608.0f) return 45;

    return 0;
}

// === 除算テスト (61-80) ===
int test_division() {
    // 基本除算
    if (3.0f != 6.0f / 2.0f) return 61;

    // サブノーマル生成
    if (to_float(HEX_FLT_TRUE_MIN) != to_float(HEX_FLT_MIN) / 8388608.0f) return 62;

    // ゼロ除算
    if (!float_isnan(0.0f / 0.0f)) return 63;

    // オーバーフロー除算
    if (!float_isinf(to_float(HEX_FLT_MAX) / to_float(HEX_FLT_TRUE_MIN))) return 64;

    return 0;
}

int main(int argc, char **argv) {
    int code;

//    if ((code = test_addition())) return code;
//    if ((code = test_subtraction())) return code;
    if ((code = test_multiplication())) return code;
    if ((code = test_division())) return code;
    return 0;
}

