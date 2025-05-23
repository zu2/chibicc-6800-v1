;
;	int builtin_clz(unsigned long x)
;	int builtin_ctz(unsigned long x)
;           x=0 : undefined behavior
;

	.data
	.code
	.export	___builtin_clz
	.export	___builtin_ctz
;
___builtin_clz:
        clrb
        ldaa    @long
        bne     __clz_count
        addb    #8
        ldaa    @long+1
        bne     __clz_count
        addb    #8
        ldaa    @long+2
        bne     __clz_count
        addb    #8
        ldaa    @long+3
        ble     __clz_ret               ; see. *1
;                                       ; 0 input: undefined behavior
__clz_loop:                             ; count AccA's bit, increment AccB
        incb
        asla
__clz_count:
        bpl     __clz_loop
__clz_ret:
        clra
        rts
;
; *1
; BLE branches when Z or (N xor V) == 1.
; Immediately after LDAA, V = 0 and N is the most significant bit of AccA.
; Therefore, the condition simplifies to (Z or N) == 1,
; meaning the branch occurs if AccA is zero or the MSB of AccA is set.
;
;
___builtin_ctz:
        clrb
        ldaa    @long+3
        bne     __ctz_loop
        addb    #8
        ldaa    @long+2
        bne     __ctz_loop
        addb    #8
        ldaa    @long+1
        bne     __ctz_loop
        addb    #8
        ldaa    @long
        bne     __ctz_loop
        clra
        rts                             ; 0 input: undefined behavior
;
__ctz_loop:                             ; count AccA's bit, increment AccB
        incb
        lsra
        bcc     __ctz_loop
        decb                            ; Counts one extra due to carry.
__ctz_ret:
        clra
        rts
