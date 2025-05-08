int main(int argc, char **argv)
{
  static const int   ci = 1;
  static const float cf = 1.0f;

  if (ci != 1)
    return 1;

  if (cf != 1.0f)
    return 2;

  return 0;
}

