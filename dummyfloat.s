;
;	MC6800 dummy floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
	.zp
	.data
	.export	__i16tof32
	.export	__u16tof32
	.export	__i32tof32
	.export	__u32tof32
	.export	__f32tou32
	.export	__f32toi32
	.export	__f32tou16
	.export	__f32toi16
	.export	__f32tou8
	.export	__f32toi8
	.export	__addf32tos
	.export __subf32tos
	.export __mulf32tos
	.export __divf32tos
	.export __cmpf32tos
	.export __cmpf32x
	.export __load32x_addf
	.export __load32x_subf
	.export __load32x_mulf
	.export __load32x_divf
	.export __load32x_cmpf
	.export __f32iszero
;	.export __f32iszerox
	.export __f32isNaNorInf
;	.export __f32isNaNorInfx
	.export __fdiv32x32
	.export	__setup_long
	.export __f32retpZero
	.export __setup_zin
	.export __f32NaN
	.export __f32NaNx
	.export __f32zerox
	.export __f32zeros
	.export __f32ones
	.export	__f32Infs
	.export __adj_subnormal
	.export __asl8_both
;
	.export	_fabsf
	.export	_isinf
	.export	_isnan
	.export	_isfinite
        .export _signbit
        .export _frexpf
        .export _modff
;
__i16tof32:
__u16tof32:
__i32tof32:
__u32tof32:
__f32tou32:
__f32toi32:
__f32tou16:
__f32toi16:
__f32tou8:
__f32toi8:
__addf32tos:
__subf32tos:
__mulf32tos:
__divf32tos:
__cmpf32tos:
__cmpf32x:
__load32x_addf:
__load32x_subf:
__load32x_mulf:
__load32x_divf:
__load32x_cmpf:
__f32iszero:
__f32iszerox:
__f32isNaNorInf:
__f32isNaNorInfx:
__fdiv32x32:
__setup_long:
__f32retpZero:
__setup_zin:
__f32NaN:
__f32NaNx:
__f32zerox:
__f32zeros:
__f32ones:
__f32Infs:
__adj_subnormal:
__asl8_both:
;
_fabsf:
_isinf:
_isnan:
_isfinite:
_signbit:
_frexpf:
_modff:
;
        ldx #dummy_mes
        bra mes_start
loop:
        jsr _putchar
        inx
mes_start:
        ldab 0,x
        bne loop
        ldab #$ff
        jmp _abort
;
dummy_mes:
        .ascii "Dummy floating-point library called."
        .byte   0
        
