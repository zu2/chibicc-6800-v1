int main(void)
{
  signed char   a = -1;
  unsigned char b = 255;
  int x;

  if (a==b)      return 1;
  if (!(a!=b))   return 2;
  if (!(a < b))  return 3;
  if (a >= b)    return 4;
  if (!(b > a))  return 5;
  if (b <= a)    return 6;

  signed char   c = -128;
  unsigned char d = 128;

  if (c == d)    return 7;
  if (!(c != d)) return 8;
  if (!(c < d))  return 9;
  if (c >= d)    return 10;
  if (!(d > c))  return 11;
  if (d <= c)    return 12;

  signed char sc;
  unsigned char uc;

  if ((uc=a&b)!=255) return 21;
  if ((uc=a|b)!=255) return 22;
  if ((uc=a^b)!=0)   return 23;

  if ((sc=a&b)!=-1)  return 31;
  if ((sc=a|b)!=-1)  return 32;
  if ((sc=a^b)!=0)   return 33;

  if ((uc=c&d)!=128) return 41;
  if ((uc=c|d)!=128) return 22;
  if ((uc=c^d)!=0)   return 23;

  if ((sc=c&d)!=-128) return 31;
  if ((sc=c|d)!=-128) return 32;
  if ((sc=c^d)!=0)    return 33;

  return 0;
}
