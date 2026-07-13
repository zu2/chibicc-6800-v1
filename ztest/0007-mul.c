#include "common.h"

// 16-bit signed integer multiplication
int mul(int a, int b) {
    return a * b;
}  

int main(void)
{
    // Group 1
    if (mul(0, 0) != 0) return 10;
    if (mul(1, 1) != 1) return 11;
    if (mul(-1, -1) != 1) return 12;
    if (mul(1, -1) != -1) return 13;
    if (mul(-1, 1) != -1) return 14;
    if (mul(2, 2) != 4) return 15;
    if (mul(2, -2) != -4) return 16;
    if (mul(-2, 2) != -4) return 17;
    if (mul(-2, -2) != 4) return 18;
    if (mul(3, 3) != 9) return 19;
    if (mul(3, -3) != -9) return 20;
    if (mul(-3, 3) != -9) return 21;
    if (mul(-3, -3) != 9) return 22;
    if (mul(4, 4) != 16) return 23;
    if (mul(4, -4) != -16) return 24;
    if (mul(-4, 4) != -16) return 25;
    if (mul(-4, -4) != 16) return 26;
    // Group 2
    if (mul(32767, 1) != 32767) return 30;
    if (mul(1, 32767) != 32767) return 31;
    if (mul(-32768, 1) != -32768) return 32;
    if (mul(1, -32768) != -32768) return 33;
    if (mul(32767, -1) != -32767) return 34;
    if (mul(-32768, -1) != -32768) return 35;
    if (mul(32767, 32767) != 1) return 36;
    if (mul(-32768, -32768) != 0) return 37;
    if (mul(32767, -32767) != -1) return 38;
    if (mul(-32767, 32767) != -1) return 39;
    if (mul(32766, 32766) != 4) return 40;
    if (mul(-32766, -32766) != 4) return 41;
    if (mul(32767, 2) != -2) return 42;
    if (mul(2, 32767) != -2) return 43;
    if (mul(-32768, 2) != 0) return 44;
    if (mul(2, -32768) != 0) return 45;
    if (mul(32767, 3) != 32765) return 46;
    if (mul(-32766, 2) != 4) return 47;
    if (mul(32765, 2) != -6) return 48;
    if (mul(2, 32765) != -6) return 49;
    // Group 3
    if (mul(65280, 255) != 256) return 50;
    if (mul(255, 65280) != 256) return 51;
    if (mul(32768, 1) != -32768) return 52;
    if (mul(1, 32768) != -32768) return 53;
    if (mul(32512, 1) != 32512) return 54;
    if (mul(1, 32512) != 32512) return 55;
    if (mul(255, 255) != -511) return 56;
    if (mul(65280, 2) != -512) return 57;
    if (mul(2, 65280) != -512) return 58;
    if (mul(255, -2) != -510) return 59;
    if (mul(255, 1) != 255) return 60;
    if (mul(1, 255) != 255) return 61;
    if (mul(255, 7) != 1785) return 62;
    if (mul(4, 256) != 1024) return 63;
    // Group 4
    if (mul(32767, 32766) != -32766) return 70;
    if (mul(-32768, 32767) != -32768) return 71;
    if (mul(32766, -32768) != 0) return 72;
    if (mul(-32767, -32768) != -32768) return 73;
    if (mul(0, 32767) != 0) return 74;
    if (mul(32767, 0) != 0) return 75;
    if (mul(0, -32768) != 0) return 76;
    if (mul(-32768, 0) != 0) return 77;
    if (mul(-32768, 3) != -32768) return 78;
    if (mul(3, -32768) != -32768) return 79;
    if (mul(255, -32768) != -32768) return 80;
    if (mul(-32768, 255) != -32768) return 81;
    if (mul(127, -255) != -32385) return 82;
    if (mul(-255, 127) != -32385) return 83;
    // Group 5
    if (mul(11479, 1159) != 353) return 90;
    if (mul(-13129, 15149) != 10539) return 91;
    if (mul(15572, 11096) != -31520) return 92;
    if (mul(-30522, -2412) != 22136) return 93;
    if (mul(7112, -16481) != 31032) return 94;
    if (mul(-26223, 20584) != -19736) return 95;
    if (mul(14172, 6480) != 18624) return 96;
    if (mul(-21734, -8956) != 7784) return 97;
    if (mul(-16414, -28703) != -7262) return 98;
    if (mul(-6165, 10053) != 20311) return 99;
    if (mul(-32333, -1978) != -8462) return 100;
    if (mul(6534, 6697) != -19850) return 101;
    if (mul(21636, 19739) != -25108) return 102;
    if (mul(-5229, 27825) != -7005) return 103;
    if (mul(16305, 21310) != -12322) return 104;
    if (mul(-448, -25459) != 2368) return 105;
    if (mul(1109, 15777) != -1419) return 106;
    if (mul(3782, 16656) != 12896) return 107;
    if (mul(-2823, -4205) != 8699) return 108;
    if (mul(-12966, 23415) != 29398) return 109;
    if (mul(-25577, 16533) != -26269) return 110;
    if (mul(-28805, 10353) != -29365) return 111;
    if (mul(20474, 14593) != -1542) return 112;
    if (mul(-20274, 21637) != 29446) return 113;
    if (mul(25077, 10637) != 12529) return 114;
    if (mul(-24937, -17500) != -6724) return 115;
    if (mul(-9214, -24605) != 21446) return 116;
    if (mul(5325, 9918) != -8666) return 117;
    if (mul(-5447, -31343) != 4041) return 118;
    if (mul(11066, -30489) != -11946) return 119;
    if (mul(-1647, -221) != -29229) return 120;
    if (mul(14337, 16028) != 24220) return 121;
    if (mul(23059, -26794) != 30562) return 122;
    if (mul(-13631, -22926) != 28658) return 123;
    if (mul(372, 1670) != 31416) return 124;
    if (mul(-31475, 26715) != -27745) return 125;
    if (mul(2002, -15065) != -13570) return 126;
    if (mul(-21413, -1510) != 24382) return 127;
    if (mul(8436, 12868) != 26832) return 128;
    if (mul(24759, 28484) != 2460) return 129;

    return 0;
}
