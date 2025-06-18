;
;	MC6800 floating point arithmetic library
;
;	Copyright (c) 2025 by ZUKERAN, shin
;	Released under the MIT license
;
;	https://github.com/zu2/chibicc-6800-v1?tab=License-1-ov-file#readme
;
;	Note: This program was created for testing chibicc-6800-v1, 
;	and does not pay attention to speed, accuracy, or exception handling.
;
;
        .export	_nextafterf
        .code
;
_nextafterf:
        tsx
        jsr     __setup_zin
        tsx
        bitb    #$01                    ; @long is NaN?
        bne     ret_long                ; yes, return @long
        bitb    #$02                    ; TOS is NaN?
        beq     _nextafterf_01          ; no, skip
ret_tos:
        ldx     4,x
        stx     @long+2
        tsx
        ldx     2,x
        stx     @long
ret_long:
        rts
_nextafterf_01:
        pshb                            ; save __zin
        andb    #$30
        cmpb    #$30
        bne     _nextafterf_02          ; a==±0.0 && b==±0.0 ?
        ins                             ; yes, return b
        bra     ret_tos
_nextafterf_02:
        jsr     __cmpf32x               ; a==b ?
        pula
        beq     ret_tos                 ; yes, return b;        (AccB=$00)
;                                       ; here, if a>b then B=$01 else if a<b B=$FF
;
        bita    #$10                    ; @long == 0 ?
;
        beq     _nextafterf_10
;                                       ; if @long==0
        tsx
        asl     2,x                     ;   @long=copysign(FLT_TRUE_MIN,@long)
        ldx     #1                      ; 
        stx     @long+2
        ldx     #0
        stx     @long
        ror     @long
;
        rts
;
_nextafterf_10:
;
        ldx     @long+2
        eorb    @long
        andb    #$80                    ; if (a>0.0 && a<b) B=$80 else B=$00
        beq     _nextafterf_20
;
        inx
        stx     @long+2
        bne     _nextafterf_19
        ldx     @long
        inx
        stx     @long
_nextafterf_19:
        rts
;
_nextafterf_20:
        dex
        stx     @long+2
        inx
        bne     _nextafterf_29
        ldx     @long
        dex
        stx     @long
_nextafterf_29:
        rts
