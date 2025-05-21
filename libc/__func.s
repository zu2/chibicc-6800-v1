;
;	function prologue/epilogue
;
	.export	__prologue_0
	.export	__prologue_1
	.export	__prologue_2
	.export	__prologue_4
        .export __sub_bp_b
        .export __sub_bp_d
        .export __add_bp_b

	.code
__prologue_0:
        tsx
        ldx     0,x
        stx     @tmp1
        tsx
        bra     __prologue_bp
__prologue_1:
        tsx
        ldx     0,x
        stx     @tmp1
        tsx
        pshb
        bra     __prologue_bp
__prologue_2:
        tsx
        ldx     0,x
        stx     @tmp1
        tsx
        pshb
        psha
__prologue_bp:
        ldab    @bp+1
        ldaa    @bp
        stab    1,x
        staa    0,x
        ldx     @tmp1
        jmp     0,x
__prologue_4:
        tsx
        ldx     0,x
        stx     @tmp1
        tsx
        ldab    @long+3
        pshb
        ldab    @long+2
        pshb
        ldab    @long+1
        pshb
        ldab    @long
        pshb
        bra     __prologue_bp
;
;
;
__add_bp_b:
        psha
        clra
        bsr     __add_bp_d
        pula
        rts
__sub_bp_b:
        clra
__sub_bp_d:
        nega
        negb
        sbca    #0
__add_bp_d:
        addb    @bp+1
        adca    @bp
        stab    @bp+1
        staa    @bp
        ldx     @bp
        rts

