int main(int argc, char **argv)
{
  // Case 1: Bitfield and char overlay (check lower byte 'c' for MC6800 big
  // endian)
  union U1 {
    unsigned int a : 8;
    struct {
      unsigned char b, c;
    };
  };
  union U1 u1;
  u1.a = 0xAB;
  if (sizeof(union U1) != 2) {
    return 1;
  }
  if (u1.c != 0xAB) {
    return 2;
  }

  u1.c = 0x5C;
  if (u1.a != 0x5C) {
    return 3;
  }

  // Case 2: Multiple bitfields in union
  union U2 {
    unsigned int a : 4;
    unsigned int b : 12;
    unsigned short s;
    struct {
      unsigned char hi, lo;
    };
  };
  union U2 u2;
  u2.a = 0xA;
  if (sizeof(union U2) != 2) {
    return 4;
  }
  if ((u2.s & 0x000F) != 0xA) {
    return 5;
  }
  if ((u2.lo & 0x0F) != 0xA) {
    return 6;
  }

  u2.b = 0xBCD;
  if ((u2.s & 0x0FFF) != 0xBCD) {
    return 7;
  }
  if (((u2.hi << 8) | u2.lo) != u2.s) {
    return 8;
  }

  // Case 3: Bitfield struct in union
  union U3 {
    struct {
      unsigned int a : 8;
      unsigned int b : 8;
    } s;
    unsigned int u;
    struct {
      unsigned char hi, lo;
    };
  };
  union U3 u3;
  u3.s.a = 0x12;
  u3.s.b = 0x34;
  if (sizeof(union U3) != 2) {
    return 9;
  }
  // a: lower 8 bits (lo), b: upper 8 bits (hi), big endian
  if (u3.u != 0x3412) {
    return 10;
  }
  if (u3.hi != 0x34 || u3.lo != 0x12) {
    return 11;
  }

  u3.u = 0xABCD;
  if (u3.s.a != 0xCD || u3.s.b != 0xAB) {
    return 12;
  }

  // Case 4: Mixed size
  union U4 {
    unsigned int a : 16;
    unsigned char b[3];
    struct {
      unsigned char c;
      unsigned int d : 8;
    } s;
  };
  union U4 u4;
  u4.a = 0x1234;
  if (sizeof(union U4) != 3) {
    return 13;
  }
  if (u4.b[0] != 0x12 || u4.b[1] != 0x34) {
    return 14;
  }

  u4.b[0] = 0xAA;
  u4.b[1] = 0xBB;
  u4.b[2] = 0xCC;
  if (u4.a != 0xAABB) {
    return 15;
  }

  // Case 5: Bitfield and struct overlay
  union U5 {
    unsigned int a : 12;
    struct {
      unsigned char b, c;
    };
    unsigned short raw;
  };
  union U5 u5;
  u5.a = 0xABC;
  if (sizeof(union U5) != 2) {
    return 16;
  }
  // a:12 (b0-b11), c: lower 8 bits, b: upper 4 bits (b8-b11)
  if ((u5.c != (0xBC)) || ((u5.b & 0x0F) != 0x0A)) {
    return 17;
  }

  // Case 6: Bitfield self-consistency
  union U6 {
    unsigned int a : 7;
  };
  union U6 u6;
  u6.a = 0x7F;
  if (u6.a != 0x7F) {
    return 18;
  }
  u6.a = 0x55;
  if (u6.a != 0x55) {
    return 19;
  }

  // Case 7: 1-bit width signed/unsigned
  union U7 {
    int a : 1;
    unsigned int b : 1;
  };
  union U7 u7;
  u7.a = 1;
  if (u7.a != -1) {
    return 20;
  }
  u7.b = 1;
  if (u7.b != 1) {
    return 21;
  }

  return 0;
}
