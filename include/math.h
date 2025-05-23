//
//	math.h for chibicc 6800
//
#include "__use_float_for_double.h"

#ifndef MATH_H
#define MATH_H
#include <stdint.h>

#define M_PI 	0x1.921fb54p+1f // 3.1415927f	pi
#define	M_TWOPI	0x1.921fb54p+2f	// 6.2831855f	2*pi
#define M_PI_2	0x1.921fb54p+0f	// 1.5707964f	pi/2
#define M_PI_4	0x1.921fb54p-1f	// 0.7853982f	pi/4
#define	M_PI_8	0x1.921fb54p-2f	// 0.3926991f	pi/8
#define M_1_PI	0x1.45f307p-2f	// 0.3183099f	1/pi
#define M_2_PI	0x1.45f307p-1f	// 0.6366198f	2/pi
#define M_E  	0x1.bc55864p+0f // 2.7182817f	e
#define M_LOG2E	0x1.7154765p-1f	// 1.4426950f	log_2 e
#define	M_LOG10E 0x1.b8aa3b2p-3f // 0.4342945f	log_10 e
#define	M_LN2	0x1.62e42fep-1f	// 0.6931472f	ln 2
#define	M_LN10	0x1.772b538p+1f	// 2.3025851f	ln 10

#define NAN      (__builtin_nanf(""))
#define INFINITY (__builtin_inff())

int	isnan(float x);
int	isinf(float x);
int	isfinite(float x);
float	copysignf(float x, float y);
int	signbit(float x);

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

float	powf(float base, float exponent);
float	log1pf(float x);

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

#ifdef  __USE_FLOAT_FOR_DOUBLE__
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
