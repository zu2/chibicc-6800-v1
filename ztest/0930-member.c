typedef struct {
	int a, b, c, d, e;
} S;

void func1(S *s) {
	s->e = 0;
}

void func2(S *p) {
  S *s = p;
	s->e = 0;
}

int main(int argc, char **argv)
{
  S s;

  s.e = 999;

  func1(&s);
  if (s.e)
    return 1;

  s.e = 999;
  func2(&s);
  if (s.e)
    return 2;

  return 0;
}
