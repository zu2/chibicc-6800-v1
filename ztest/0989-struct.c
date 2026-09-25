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

union overlap_long ul;
union overlap_float uf;
union overlap_struct us;

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
  if (test_long())
    return 1;

  if (test_float())
    return 2;

  if (test_struct())
    return 3;

  return 0;
}
