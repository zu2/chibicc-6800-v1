union overlap_long {
  long y;
  struct {
    char pad;
    long x;
  } a;
};

union overlap_float {
  float y;
  struct {
    char pad;
    float x;
  } a;
};

struct four_bytes {
  char b[4];
};

union overlap_struct {
  struct four_bytes y;
  struct {
    char pad;
    struct four_bytes x;
  } a;
};

struct s1 {
  unsigned char b[1];
};

struct s2 {
  unsigned char b[2];
};

struct s3 {
  unsigned char b[3];
};

struct s4 {
  unsigned char b[4];
};

struct s5 {
  unsigned char b[5];
};

struct s255 {
  unsigned char b[255];
};

struct s256 {
  unsigned char b[256];
};

struct s257 {
  unsigned char b[257];
};

union u1 {
  struct s1 y;
  struct s1 z;
  struct {
    char pad;
    struct s1 x;
  } a;
};

union u2 {
  struct s2 y;
  struct s2 z;
  struct {
    char pad;
    struct s2 x;
  } a;
};

union u3 {
  struct s3 y;
  struct s3 z;
  struct {
    char pad;
    struct s3 x;
  } a;
};

union u4 {
  struct s4 y;
  struct s4 z;
  struct {
    char pad;
    struct s4 x;
  } a;
};

union u5 {
  struct s5 y;
  struct s5 z;
  struct {
    char pad;
    struct s5 x;
  } a;
};

union u255 {
  struct s255 y;
  struct s255 z;
  struct {
    char pad;
    struct s255 x;
  } a;
};

union u256 {
  struct s256 y;
  struct s256 z;
  struct {
    char pad;
    struct s256 x;
  } a;
};

union u257 {
  struct s257 y;
  struct s257 z;
  struct {
    char pad;
    struct s257 x;
  } a;
};

union overlap_long ul;
union overlap_float uf;
union overlap_struct us;
union u1 gu1;
union u2 gu2;
union u3 gu3;
union u4 gu4;
union u5 gu5;
union u255 gu255;
union u256 gu256;
union u257 gu257;

static int test_long(void) {
  ul.a.x = 0x12345678L;
  ul.y = ul.a.x;
  return ul.y != 0x12345678L;
}

static int test_float(void) {
  uf.a.x = 1.0f;
  uf.y = uf.a.x;
  return uf.y != 1.0f;
}

static int test_struct(void) {
  us.y.b[0] = 1;
  us.y.b[1] = 2;
  us.y.b[2] = 3;
  us.y.b[3] = 4;
  us.a.x = us.y;
  return us.a.x.b[0] != 1 || us.a.x.b[1] != 2 ||
         us.a.x.b[2] != 3 || us.a.x.b[3] != 4;
}

int main(int argc, char **argv)
{
  int i;

  if (test_long())
    return 1;

  if (test_float())
    return 2;

  if (test_struct())
    return 3;

  for (i = 0; i < 1; i++) {
    gu1.y.b[i] = i + 1;
  }
  gu1.a.x = gu1.y;
  for (i = 0; i < 1; i++) {
    if (gu1.a.x.b[i] != (unsigned char)(i + 1)) return 10;
  }

  for (i = 0; i < 1; i++) {
    gu1.a.x.b[i] = i + 1;
  }
  gu1.y = gu1.a.x;
  for (i = 0; i < 1; i++) {
    if (gu1.y.b[i] != (unsigned char)(i + 1)) return 11;
  }

  for (i = 0; i < 1; i++) {
    gu1.y.b[i] = i + 1;
  }
  gu1.z = gu1.y;
  for (i = 0; i < 1; i++) {
    if (gu1.z.b[i] != (unsigned char)(i + 1)) return 12;
  }

  for (i = 0; i < 2; i++) {
    gu2.y.b[i] = i + 1;
  }
  gu2.a.x = gu2.y;
  for (i = 0; i < 2; i++) {
    if (gu2.a.x.b[i] != (unsigned char)(i + 1)) return 20;
  }

  for (i = 0; i < 2; i++) {
    gu2.a.x.b[i] = i + 1;
  }
  gu2.y = gu2.a.x;
  for (i = 0; i < 2; i++) {
    if (gu2.y.b[i] != (unsigned char)(i + 1)) return 21;
  }

  for (i = 0; i < 2; i++) {
    gu2.y.b[i] = i + 1;
  }
  gu2.z = gu2.y;
  for (i = 0; i < 2; i++) {
    if (gu2.z.b[i] != (unsigned char)(i + 1)) return 22;
  }

  for (i = 0; i < 3; i++) {
    gu3.y.b[i] = i + 1;
  }
  gu3.a.x = gu3.y;
  for (i = 0; i < 3; i++) {
    if (gu3.a.x.b[i] != (unsigned char)(i + 1)) return 30;
  }

  for (i = 0; i < 3; i++) {
    gu3.a.x.b[i] = i + 1;
  }
  gu3.y = gu3.a.x;
  for (i = 0; i < 3; i++) {
    if (gu3.y.b[i] != (unsigned char)(i + 1)) return 31;
  }

  for (i = 0; i < 3; i++) {
    gu3.y.b[i] = i + 1;
  }
  gu3.z = gu3.y;
  for (i = 0; i < 3; i++) {
    if (gu3.z.b[i] != (unsigned char)(i + 1)) return 32;
  }

  for (i = 0; i < 4; i++) {
    gu4.y.b[i] = i + 1;
  }
  gu4.a.x = gu4.y;
  for (i = 0; i < 4; i++) {
    if (gu4.a.x.b[i] != (unsigned char)(i + 1)) return 40;
  }

  for (i = 0; i < 4; i++) {
    gu4.a.x.b[i] = i + 1;
  }
  gu4.y = gu4.a.x;
  for (i = 0; i < 4; i++) {
    if (gu4.y.b[i] != (unsigned char)(i + 1)) return 41;
  }

  for (i = 0; i < 4; i++) {
    gu4.y.b[i] = i + 1;
  }
  gu4.z = gu4.y;
  for (i = 0; i < 4; i++) {
    if (gu4.z.b[i] != (unsigned char)(i + 1)) return 42;
  }

  for (i = 0; i < 5; i++) {
    gu5.y.b[i] = i + 1;
  }
  gu5.a.x = gu5.y;
  for (i = 0; i < 5; i++) {
    if (gu5.a.x.b[i] != (unsigned char)(i + 1)) return 50;
  }

  for (i = 0; i < 5; i++) {
    gu5.a.x.b[i] = i + 1;
  }
  gu5.y = gu5.a.x;
  for (i = 0; i < 5; i++) {
    if (gu5.y.b[i] != (unsigned char)(i + 1)) return 51;
  }

  for (i = 0; i < 5; i++) {
    gu5.y.b[i] = i + 1;
  }
  gu5.z = gu5.y;
  for (i = 0; i < 5; i++) {
    if (gu5.z.b[i] != (unsigned char)(i + 1)) return 52;
  }

  for (i = 0; i < 255; i++) {
    gu255.y.b[i] = i + 1;
  }
  gu255.a.x = gu255.y;
  for (i = 0; i < 255; i++) {
    if (gu255.a.x.b[i] != (unsigned char)(i + 1)) return 60;
  }

  for (i = 0; i < 255; i++) {
    gu255.a.x.b[i] = i + 1;
  }
  gu255.y = gu255.a.x;
  for (i = 0; i < 255; i++) {
    if (gu255.y.b[i] != (unsigned char)(i + 1)) return 61;
  }

  for (i = 0; i < 255; i++) {
    gu255.y.b[i] = i + 1;
  }
  gu255.z = gu255.y;
  for (i = 0; i < 255; i++) {
    if (gu255.z.b[i] != (unsigned char)(i + 1)) return 62;
  }

  for (i = 0; i < 256; i++) {
    gu256.y.b[i] = i + 1;
  }
  gu256.a.x = gu256.y;
  for (i = 0; i < 256; i++) {
    if (gu256.a.x.b[i] != (unsigned char)(i + 1)) return 70;
  }

  for (i = 0; i < 256; i++) {
    gu256.a.x.b[i] = i + 1;
  }
  gu256.y = gu256.a.x;
  for (i = 0; i < 256; i++) {
    if (gu256.y.b[i] != (unsigned char)(i + 1)) return 71;
  }

  for (i = 0; i < 256; i++) {
    gu256.y.b[i] = i + 1;
  }
  gu256.z = gu256.y;
  for (i = 0; i < 256; i++) {
    if (gu256.z.b[i] != (unsigned char)(i + 1)) return 72;
  }

  for (i = 0; i < 257; i++) {
    gu257.y.b[i] = i + 1;
  }
  gu257.a.x = gu257.y;
  for (i = 0; i < 257; i++) {
    if (gu257.a.x.b[i] != (unsigned char)(i + 1)) return 80;
  }

  for (i = 0; i < 257; i++) {
    gu257.a.x.b[i] = i + 1;
  }
  gu257.y = gu257.a.x;
  for (i = 0; i < 257; i++) {
    if (gu257.y.b[i] != (unsigned char)(i + 1)) return 81;
  }

  for (i = 0; i < 257; i++) {
    gu257.y.b[i] = i + 1;
  }
  gu257.z = gu257.y;
  for (i = 0; i < 257; i++) {
    if (gu257.z.b[i] != (unsigned char)(i + 1)) return 82;
  }

  return 0;
}
