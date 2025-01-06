;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;

;
;	Note: his program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;
	.zp
	.data
	.export	_fabs
	.data
__sign:	.byte	0	; sign of reslut
__texp:	.byte	0	; TOS's exp
__lexp:	.byte	0	; @long's exp
	.code
;
;	@long = fabs(@long)
;
_fabs:
	tsx
	inx
	inx
	jsr	__load32x	; @long = TOS
	asl	@long		; sign bit = 0
	lsr	@long
	rts
