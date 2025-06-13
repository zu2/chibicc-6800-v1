;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;

;
;	Note: This program was created for testing chibicc-6800-v1,
;	and does not pay attention to speed, accuracy, or exception handling.
;

;
;	TODO:
;
;		more test
;		refactoring
;		Exception handling
;		Speed up
;
;
;       float fmodf (float x,float y);
;
;       @long:  x
;       +0:     return address
;       +2:     y
;
;
	.export _fmodf
;
        .data
__zin:	.byte	0	; TOS & @long are Zero? Inf? NaN?
__yadrs:.word   0
	.code
;
_fmodf:
        tsx
        jsr     __setup_zin
;
	bitb	#$3F			; TOS or @long is NaN,Inf,0.0 ?
	beq	__fmodf_01		; No, do normal calculation
;
	bitb	#$03
	jne	__fmodf_NaN		; TOS or @long is NaN, return NaN
;
	bitb	#$04			; @long is Inf ?
	bne	__fmodf_NaN		; Yes, Inf%any returns NaN
;
	bitb	#$08			; TOS is Inf?
	bne	__fmodf32_s15		; Yes, return num(@long) it self.
;
	bitb	#$20			; TOS is 0.0 ?
	bne	__fmodf_NaN		; Yes, any/0.0 return NaN
;
                                        ; here, @long is 0.0, return it self.
;
__fmodf32_s15:
	rts				; returns num(@long) it self.
__fmodf_NaN:
	jmp	__f32NaN
;
;
__fmodf_01:
        tsx
        inx
        inx
        stx __yadrs                     ; save y's address
;
        jsr __push32                    ; push x
;
        ldx __yadrs
        jsr __push32x                   ; push y
        ldx __yadrs
        jsr __push32x                   ; push y
;
	jsr __divf32tos                 ; @long = @long / TOS  = x / y  , discard TOS y
;
        jsr __f32isNaNorInf             ; Z=1 Inf, C=1 NaN
        bhi __fmodf_ok                  ; jump if Z=0 and C=0
;
        ins                             ; discard TOS y
        ins
        ins
        ins
        ins                             ; discard TOS x
        ins
        ins
        ins
        bra __fmodf_NaN
;
__fmodf_ok:
	jsr _truncf                     ; @long = truncf(x/y)
;
	jsr __mulf32tos                 ; @long = y * truncf(x/y)       , discard TOS y
;
        jsr __subf32tos                 ; @long = @long - TOS = y*trucf(x/y) - x, dis x
;
        ldaa @long                      ; flip sign
        eora #$80
        staa @long
;
	rts
;
