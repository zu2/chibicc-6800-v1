int i;

int main(int argc, char **argv)
{
  for (i=0; i<10; i++) {
    if (i>0) {
      if (i==0)
        return 1;
    }
  }
  return 0;
}
