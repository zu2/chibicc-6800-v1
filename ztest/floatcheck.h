//
//
//
//
#define HEX_FLT_TRUE_MIN   (0x00000001)  // minimum subnormal number
#define HEX_FLT_TRUE_MIN2  (0x00000002)  // ↑ *2
#define HEX_FLT_MIN        (0x00800000)  // minimum normal number
#define HEX_FLT_MIN_DIV2   (0x00400000)	 // ↑ /2
#define HEX_FLT_MIN_DIV4   (0x00200000)	 // ↑↑ /4
#define HEX_FLT_MIN_DIV8   (0x00100000)	 // ↑↑ /8
#define HEX_FLT_MAX        (0x7f7fffff)  // maximum normal number
#define HEX_NEG_MAX        (0xff7fffff)  // -FLT_MAX
#define pZERO		   (0x00000000)
#define mZERO		   (0x80000000)
#define pINFINITY	   (0x7f800000)
#define mINFINITY	   (0xFf800000)

#if defined(__clang__) || defined(__GNUC__)
#else
extern	int	isinf(float x);
extern	int	isnan(float x);
extern	float	fabsf(float x);
extern  float	floorf(float x);
extern  float	truncf(float x);
extern	float	fmodf(float x, float y);
#endif

float to_float(unsigned long x) {
    return *(float *)&x;
}

int
cmpf(float f, float g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
	return 0;
}

int
cmpfl(float f, unsigned long g)
{
	int i;
	unsigned char *p = (unsigned char *)&f;
	unsigned char *q = (unsigned char *)&g;

	for (i=0; i<4; ++i,++p,++q){
		if (*p != *q){
			return	i+1;
		}
	}
	return 0;
}
