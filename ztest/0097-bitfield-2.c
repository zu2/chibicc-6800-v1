// Bitfield edge case test for MC6800-style packing

struct S1 {
  unsigned int a : 15;
  unsigned int b : 1;
  unsigned char c;
  unsigned int d : 4;
  unsigned int e : 12;
};

struct S2 {
  unsigned char a;
  unsigned int b : 4;
  unsigned int c : 4;
  unsigned int d : 8;
};

struct S3 {
  unsigned int a : 3;
  unsigned char b;
  unsigned int c : 5;
  unsigned char d;
  unsigned int e : 8;
};

struct S4 {
  unsigned char a;
  unsigned char b;
  unsigned int c : 16;
};

struct S5 {
  unsigned int a : 16;
  unsigned char b;
  unsigned int c : 1;
  unsigned int d : 15;
};

int main(int argc, char **argv)
{
  // S1: bitfield(16bits) + char + bitfield(4+12=16bits)
//  struct S1 s1 = {0x1234, 1, 0xAA, 0xB, 0xFED};
  struct S1 s1;

  s1.a = 0x1234;
  s1.b = 1;
  s1.c = 0xAA;
  s1.d = 0xB;
  s1.e = 0xFED;

  if (s1.a != 0x1234) {
    return 11;
  }
  if (s1.b != 1) {
    return 12;
  }
  if (s1.c != 0xAA) {
    return 13;
  }
  if (s1.d != 0xB) {
    return 14;
  }
  if (s1.e != 0xFED) {
    return 15;
  }

  // S2: char + bitfield crossing byte boundary
  struct S2 s2 = {0x12, 0xA, 0xB, 0xCD};
  if (s2.a != 0x12) {
    return 21;
  }
  if (s2.b != 0xA) {
    return 22;
  }
  if (s2.c != 0xB) {
    return 23;
  }
  if (s2.d != 0xCD) {
    return 24;
  }

  // S3: bitfield + char + bitfield + char + bitfield
  struct S3 s3 = {5, 0x34, 0x1E, 0x56, 0xAB};
  if (s3.a != 5) {
    return 31;
  }
  if (s3.b != 0x34) {
    return 32;
  }
  if (s3.c != 0x1E) {
    return 33;
  }
  if (s3.d != 0x56) {
    return 34;
  }
  if (s3.e != 0xAB) {
    return 35;
  }

  // S4: char + char + 16bit bitfield
  struct S4 s4 = {0x11, 0x22, 0xABCD};
  if (s4.a != 0x11) {
    return 41;
  }
  if (s4.b != 0x22) {
    return 42;
  }
  if (s4.c != 0xABCD) {
    return 43;
  }

  // S5: 16bit bitfield + char + 1bit bitfield + 15bit bitfield
  struct S5 s5 = {0xBEEF, 0x77, 1, 0x1234};
  if (s5.a != 0xBEEF) {
    return 51;
  }
  if (s5.b != 0x77) {
    return 52;
  }
  if (s5.c != 1) {
    return 53;
  }
  if (s5.d != 0x1234) {
    return 54;
  }

  // Size checks (optional, implementation-dependent, but should match MC6800
  // packing)
  if (sizeof(struct S1) != 5) {
    return 101; // 16bit+8bit+16bit=5bytes
  }
  if (sizeof(struct S2) != 3) {
    return 102; // 8bit+8bit+8bit
  }
  if (sizeof(struct S3) != 8) {
    return 103;
  }
  if (sizeof(struct S4) != 4) {
    return 104;
  }
  if (sizeof(struct S5) != 5) {
    return 105;
  }

  return 0;
}
