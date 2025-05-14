//
//	math.h for chibicc 6800
//
#include "__use_float_for_double.h"

#ifndef MATH_H
#define MATH_H
#include <stdint.h>

#define M_PI 0x1.921fb6p1f  // 3.1415927f
#define M_E  0x1.5bf0a8p1f  // 2.7182817f

#define INFINITY (*(const float*)&(uint32_t){0x7F800000})
#define NAN      (*(const float*)&(uint32_t){0x7FC00000})

int	isnan(float x);
int	isinf(float x);
int	isfinite(float x);

#if 1
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
#endif

float	expf(float x);
float	logf(float x);
float	log10f(float x);

float	powf(float base, float exponent);
float	log1pf(float x);

float	sqrtf(float x);

float	ceilf(float x);
float	floorf(float x);
float	truncf(float x);
float	fabsf(float x);
float	fmaxf(float x, float y);
float	fminf(float x, float y);
float	fmodf(float x, float y);
float	modff(float x, float *iptr);

#ifdef  __use_float_for_double__
#define sqrt(x)		sqrtf(x)
#define ceil(x)		ceilf(x)
#define floor(x)	floorf(x)
#define fabs(x)		fabsf(x)
#define fmax(x,y)	fmaxf(x,y)
#define fmin(x,y)	fminf(x,y)
#define fmod(x,y)	fmodf(x,y)
#define	modf(x,iptr)	modff(x,iptr)
#define log(x)		logf(x)
#define	pow(x,y)	powf(x,y)
#endif

#endif // MATH_H
