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
__copy_struct2:
	stab	@tmp1+1		; size
;
	stx	@tmp3		; dest
;
	tsx
	ldx	2,x
	stx	@tmp2		; src
	bra	__copy_struct_common
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
__copy_struct_comon:
	cmpb	#1		; if b!=0 then a++
	sbca	#0
	inca
	staa	@tmp1
;
__copy_loop:
	ldx	@tmp2
	ldaa	0,x
	inx
	stx	@tmp2
	ldx	@tmp3
	staa	0,x
	inx
	stx	@tmp3
	decb
	bne	__copy_loop
	dec	@tmp1
	bne	__copy_loop
;
__copy_end:
	tsx
	ldx	0,x
	ins
	ins
	ins
	ins
	jmp	0,x
