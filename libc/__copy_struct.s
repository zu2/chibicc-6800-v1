;
;	void __copy_struct(char *dest,const char *src,size_t n)
;	void __copy_struct2(char *dest,const char *src,size_t n){
;	  while (n--)
;	    *dest++ = *src++;
;	}
;
;	copy_struct:
;	  AccAB:src
;	  IX:	count
;	  TOS:	dest
;	copy_struct2:
;	  AccAB:size
;	  IX:	dest
;	  TOS:  src
;

	.code
	.export	__copy_struct
	.export	__copy_struct2
;
__copy_struct2:			; AccAB: size
	stx	@tmp3		; dest
;
	tsx
	ldx	2,x
	stx	@tmp2		; src
	bra	__copy_common
;
__copy_struct:
	stx	@tmp1		; size
;
	tsx
	ldx	2,x		; dest
	stx	@tmp3
;
	stab	@tmp2+1		; src
	staa	@tmp2
;
	ldab	@tmp1+1		; size
	ldaa	@tmp1
;
__copy_common:
;
	tsta
	bne	__copy_2
	cmpb	#1
	beq	__copy_1byte
;
__copy_2:
	stab    @tmp4
;
        andb    #$FE            ; Make AccB even using AND operation
;
        addb    @tmp3+1
        adca    @tmp3
        stab    @tmp1+1         ; copy end address
        staa    @tmp1
;
__copy_loop:
	ldx	@tmp2
	ldaa	0,x
	ldab	1,x
	inx
	inx
	stx	@tmp2
	ldx	@tmp3
	staa	0,x
	stab	1,x
	inx
	inx
	stx	@tmp3
        cpx     @tmp1
	bne	__copy_loop
;
	ror	@tmp4           ; more 1-byte need copy?
	bcc	__copy_end
;
__copy_1byte:
	ldx	@tmp2
	ldaa	0,x
	ldx	@tmp3
	staa	0,x
;
__copy_end:
	tsx
	ldx	0,x
	ins
	ins
	ins
	ins
	jmp	0,x
