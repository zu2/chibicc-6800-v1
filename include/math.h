//
//	math.h for chibicc 6800
//

#ifndef MATH_H
#define MATH_H
#include <stdint.h>
#include <limits.h>

#define	M_PI       0x1.921fb6p+1f	// 3.1415927f 	pi
#define	M_TWOPI    0x1.921fb6p+2f	// 6.2831855f 	2*pi
#define	M_PI_2     0x1.921fb6p+0f	// 1.5707964f 	pi/2
#define	M_PI_4     0x1.921fb6p-1f	// 0.78539819f	pi/4
#define	M_PI_8     0x1.921fb6p-2f	// 0.39269909f	pi/8
#define	M_1_PI     0x1.45f306p-2f	// 0.31830987f	1/pi
#define	M_2_PI     0x1.45f306p-1f	// 0.63661975f	2/pi
#define	M_2_SQRTPI 0x1.20dd76p+0f	// 1.1283792f 	2/sqrt(pi)
#define	M_E        0x1.5bf0a8p+1f	// 2.7182817f 	e
#define	M_LOG2E    0x1.715476p+0f	// 1.442695f  	log_2 e
#define	M_LOG10E   0x1.bcb7b2p-2f	// 0.43429449f	log_10 e
#define	M_LN2      0x1.62e430p-1f	// 0.69314718f	ln 2
#define	M_LN10     0x1.26bb1cp+1f	// 2.3025851f 	ln 10
#define	M_SQRT2    0x1.6a09e6p+0f	// 1.4142135f 	sqrt(2)
#define	M_SQRT1_2  0x1.6a09e6p-1f	// 0.70710677f	1/sqrt(2)

#define NAN      (__builtin_nanf(""))
#define INFINITY (__builtin_inff())

#define HUGE_VAL  (__builtin_inff())
#define HUGE_VALF (__builtin_inff())

#define FP_ILOGB0   (INT_MIN)
#define FP_ILOGBNAN (INT_MAX)

int	isnan(float x);
int	isinf(float x);
int	isfinite(float x);
float	copysignf(float x, float y);
int	signbit(float x);
float	nextafterf(float x, float y);
int	ilogbf(float x);

float	sinf(float x);
float	cosf(float x);
float	tanf(float x);
float	asinf(float x);
float	acosf(float x);
float	atanf(float x);
float	atan2f(float y, float x);

float	sinhf(float x);
float	coshf(float x);
float	tanhf(float x);

float	expf(float x);
float	logf(float x);
float	log10f(float x);
float	exp2f(float x);
float	log2f(float x);

float	powf(float base, float exponent);
float	log1pf(float x);
float	expm1f(float x);

float	sqrtf(float x);

float	ceilf(float x);
float	floorf(float x);
float	truncf(float x);
float	roundf(float x);
float	nearbyintf(float x);
float	fabsf(float x);
float	fmaxf(float x, float y);
float	fminf(float x, float y);
float	fmodf(float x, float y);
float	modff(float x, float *iptr);
float	frexpf(float x, int *expptr);
float	ldexpf(float x, int exp);

#ifdef  __CHIBICC_6800_DOUBLE_AS_FLOAT__
#define	sin(x)		sinf(x)
#define	cos(x)		cosf(x)
#define	tan(x)		tanf(x)
#define	atan(x)		atanf(x)
//
#define	exp(x)		expf(x)
#define log(x)		logf(x)
#define log10(x)	log10f(x)
#define	pow(x,y)	powf(x,y)
#define	log1p(x,y)	log1pf(x,y)
//
#define sqrt(x)		sqrtf(x)
//
#define ceil(x)		ceilf(x)
#define floor(x)	floorf(x)
#define trunc(x)	truncf(x);
#define round(x)	roundf(x);
#define fabs(x)		fabsf(x)
#define fmax(x,y)	fmaxf(x,y)
#define fmin(x,y)	fminf(x,y)
#define fmod(x,y)	fmodf(x,y)
#define	modf(x,iptr)	modff(x,iptr)
#endif

#endif // MATH_H
