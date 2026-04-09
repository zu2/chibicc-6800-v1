//
//	LIFE GAME
//

#include  <string.h>

void
plot(unsigned char x, unsigned char y, unsigned char z)
{
        unsigned char *p;
        unsigned char c;        // char of dot position
        unsigned char cx;
        unsigned char cy;
        unsigned char w;

        x &= 63;
        while (y>47){
                y -= 48;
        }
        cx = x&1;
        cy = y&1;
        w = cx+cx + cy;
        switch(w){
        case 0: w=1; break;
        case 1: w=2; break;
        case 2: w=4; break;
        case 3: w=8; break;
        }
        p = (((47-y)>>1)*32) +(x>>1) + 0x0100;
        c = *p;
        if (c>15) {
                c = 0;
        }
        c = (c*11) & 0x0f;
        if (z==0) {
                c &= ~w;
        }else if (z==255) {
                c ^= w;
        }else{
                c |= w;
        }
        c = (c+c+c)&0x0f;
        *p = c;
}

unsigned char a1[66][64];
unsigned char a2[66][64];

unsigned long rnd;

unsigned long xorshift32()
{
	rnd ^= rnd << 13;
	rnd ^= rnd >> 17;
	rnd ^= rnd << 5;
	return rnd;
}

void
init()
{
	unsigned char x,y;
	rnd = *((int *)0x0b);
	while (rnd==0) {
		rnd += *((int *)0x0b)+1;
	}

	for (y=0; y<64; y++){ 
		for (x=0; x<66; x++){
			a1[x][y] = 0;
			a2[x][y] = 0;
		}
	}
	for (y=1; y<=48; y++){ 
		for (x=1; x<=64; x++){
			if (xorshift32()&1){
				a1[x][y] = 1;
			}
		}
	}
	for (y=1; y<=48; y++) {
		for (x=1; x<=64; x++){
			plot(x-1,y-1,a1[x][y]);
		}
	}
}

int
main(int argc, char **argv)
{
	unsigned char x,y;
	unsigned char alive;
	unsigned char x1,y1,x2,y2;

	init();
	while(1){
    memset(a2,0,sizeof(a2));
		for (x=1; x<=64; x++){
      unsigned char *p = &a2[x][1];
		  for (y=1; y<=48; y++) {
        if (a1[x][y]) {
#if 1
          p[-65]++;
          p[-64]++;
          p[-63]++;
          p[-1]++;
          p[1]++;
          p[63]++;
          p[64]++;
          p[65]++;
#else
          a2[x-1][y-1]++;
          a2[x][y-1]++;
          a2[x+1][y-1]++;
          a2[x-1][y]++;
          a2[x+1][y]++;
          a2[x-1][y+1]++;
          a2[x][y+1]++;
          a2[x+1][y+1]++;
#endif
        }
        p++;
      }
    }
		for (y=1; y<=48; y++) {
			for (x=1; x<=64; x++){
        switch(a2[x][y]) {
        case 3:
          a1[x][y] = 1;
				  plot(x-1,y-1,1);
          break;
        case 2:
				  plot(x-1,y-1,a1[x][y]);
          break;
        default:
          a1[x][y] = 0;
				  plot(x-1,y-1,0);
          break;
        }
			}
		}
	}
	return 0;
}
