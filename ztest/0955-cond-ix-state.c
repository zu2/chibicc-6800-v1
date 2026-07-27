//
//  detect code generator's bug
//

char *p;
char *q;

char f(void)
{
  return (p && q) ? 'x' : *q;
}

int main(void)
{
  p = 0;
  q = "A";;
  return f() != 'A';
}
