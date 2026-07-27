int   n;
int   mark;
int  *m;
int **p;
int  *r0, *r1;

//
// detect optimize bug in codegen.c
//
// q0/q1: whichever lands at offset 0 triggers the bug.
//
int main(void)
{
  int *q0;
  int *q1;

  m = &mark;
  p = &m;

  q0 = &n;
  q1 = &n;              // IX_Dest = IX_BP
  if (p[0]) r0 = q0;

  q0 = &n;              // back to IX_BP
  if (p[0]) r1 = q1;

  if (r0 != &n) return 1;
  if (r1 != &n) return 2;
  return 0;
}
