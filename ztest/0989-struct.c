union overlap {
  long y;
  struct {
    char pad;
    long x;
  } a;
};

union overlap u;

int
main(int argc, char **atgv)
{
  u.a.x = 0x12345678L;
  u.y = u.a.x;

  return u.y != 0x12345678L;
}
