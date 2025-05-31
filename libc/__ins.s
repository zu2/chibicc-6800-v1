;
;   __ins_i
;     This subroutine, called right after a function call
;     for stack manipulation, should keep AccAB intact.
;     This subroutine break IX.
;
;       jsr __ins_i
;       .byte   n
;
	.export __ins_i
	.code
__ins_i:
	stab	@tmp1+1
        tsx
        ldx     0,x
        ins
        ins
        ldab    0,x
        inx
__ins_loop:
        ins
        decb
        bne     __ins_loop
        ldab    @tmp1+1
        jmp     0,x
