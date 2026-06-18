int main(int argc, char **argv)
{
  int x, y, z;
  int ix, iy;
  char cx, cy;
  unsigned char ux, uy;

  for (int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      cx = x;
      cy = y;
      ix = cx;
      iy = cy;

      if ((cx & cy) != (ix & iy)) {
        return 1;
      }
      if ((cx | cy) != (ix | iy)) {
        return 2;
      }
      if ((cx ^ cy) != (ix ^ iy)) {
        return 3;
      }
    }
  }

  for (int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      ux = x;
      uy = y;
      ix = ux;
      iy = uy;

      if ((ux & uy) != (ix & iy)) {
        return 11;
      }
      if ((ux | uy) != (ix | iy)) {
        return 12;
      }
      if ((ux ^ uy) != (ix ^ iy)) {
        return 13;
      }
    }
  }

  for (int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      cx = x;
      uy = y;
      ix = cx;
      iy = uy;

      if ((cx & uy) != (ix & iy)) {
        return 21;
      }
      if ((cx | uy) != (ix | iy)) {
        return 22;
      }
      if ((cx ^ uy) != (ix ^ iy)) {
        return 23;
      }
    }
  }

  for (int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      ux = x;
      cy = y;
      ix = ux;
      iy = cy;

      if ((ux & cy) != (ix & iy)) {
        return 31;
      }
      if ((ux | cy) != (ix | iy)) {
        return 32;
      }
      if ((ux ^ cy) != (ix ^ iy)) {
        return 33;
      }
    }
  }

  for (int x = -128; x < 128; x++) {
    cx = x;
    ix = cx;

    if ((cx & -128) != (ix & -128)) {
      return 101;
    }
    if ((cx | -128) != (ix | -128)) {
      return 102;
    }
    if ((cx ^ -128) != (ix ^ -128)) {
      return 103;
    }

    if ((cx & -1) != (ix & -1)) {
      return 111;
    }
    if ((cx | -1) != (ix | -1)) {
      return 112;
    }
    if ((cx ^ -1) != (ix ^ -1)) {
      return 113;
    }

    if ((cx & 0) != (ix & 0)) {
      return 121;
    }
    if ((cx | 0) != (ix | 0)) {
      return 122;
    }
    if ((cx ^ 0) != (ix ^ 0)) {
      return 123;
    }

    if ((cx & 1) != (ix & 1)) {
      return 131;
    }
    if ((cx | 1) != (ix | 1)) {
      return 132;
    }
    if ((cx ^ 1) != (ix ^ 1)) {
      return 133;
    }

    if ((cx & 127) != (ix & 127)) {
      return 141;
    }
    if ((cx | 127) != (ix | 127)) {
      return 142;
    }
    if ((cx ^ 127) != (ix ^ 127)) {
      return 143;
    }

    if ((cx & 128) != (ix & 128)) {
      return 146;
    }
    if ((cx | 128) != (ix | 128)) {
      return 147;
    }
    if ((cx ^ 128) != (ix ^ 128)) {
      return 148;
    }

    if ((cx & 255) != (ix & 255)) {
      return 151;
    }
    if ((cx | 255) != (ix | 255)) {
      return 152;
    }
    if ((cx ^ 255) != (ix ^ 255)) {
      return 153;
    }
  }

  for (int x = -128; x < 128; x++) {
    ux = x;
    ix = ux;

    if ((ux & -128) != (ix & -128)) {
      return 201;
    }
    if ((ux | -128) != (ix | -128)) {
      return 202;
    }
    if ((ux ^ -128) != (ix ^ -128)) {
      return 203;
    }

    if ((ux & -1) != (ix & -1)) {
      return 211;
    }
    if ((ux | -1) != (ix | -1)) {
      return 212;
    }
    if ((ux ^ -1) != (ix ^ -1)) {
      return 213;
    }

    if ((ux & 0) != (ix & 0)) {
      return 221;
    }
    if ((ux | 0) != (ix | 0)) {
      return 222;
    }
    if ((ux ^ 0) != (ix ^ 0)) {
      return 223;
    }

    if ((ux & 1) != (ix & 1)) {
      return 231;
    }
    if ((ux | 1) != (ix | 1)) {
      return 232;
    }
    if ((ux ^ 1) != (ix ^ 1)) {
      return 233;
    }

    if ((ux & 127) != (ix & 127)) {
      return 241;
    }
    if ((ux | 127) != (ix | 127)) {
      return 242;
    }
    if ((ux ^ 127) != (ix ^ 127)) {
      return 243;
    }

    if ((ux & 128) != (ix & 128)) {
      return 246;
    }
    if ((ux | 128) != (ix | 128)) {
      return 247;
    }
    if ((ux ^ 128) != (ix ^ 128)) {
      return 248;
    }

    if ((ux & 255) != (ix & 255)) {
      return 251;
    }
    if ((ux | 255) != (ix | 255)) {
      return 252;
    }
    if ((ux ^ 255) != (ix ^ 255)) {
      return 253;
    }
  }

  return 0;
}
