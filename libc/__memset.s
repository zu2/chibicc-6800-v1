;
;	void *memset(void *s,int c,size_t n)
;	{
;	  char *p = s;
;	  while (n--){
;	    *p++ = c;
;	  }
;	  return s;
;	}
;
;	stack:
;	+0 ret addr
;	+2 c
;	+4 n
;
	.code
	.export	_memset
_memset:
	tsx
	ldx	4,x		; n==0 ?
	beq	_memset_ret
	pshb			; save s, access to the stack will add two.
	psha
;
	tsx
	ldab	7,x		; get n
	ldaa	6,x
;
        stab    @tmp1           ; save n low
;
        andb    #$FC            ; Make Acc a multiple of 4 using AND.
        bne     _memset_1
        tsta
        bne     _memset_1
        ldab    5,x             ; get c
        ldx     0,x             ; get s
        bra     _memset_final
_memset_1:
        addb    1,x             ; tmp4 = s + (n & ~3)
        adca    0,x
        stab    @tmp4+1         ; save end address
        staa    @tmp4
;        
	ldab	5,x		; get c
;
	ldx	0,x		; get s
;
_memset_loop:
	stab	0,x
	inx
	stab	0,x
        inx
	stab	0,x
	inx
	stab	0,x
        inx
        cpx     @tmp4
	bne	_memset_loop
;
_memset_final:
        ldaa    @tmp1           ; get (n & 3)
        anda    #$03
        beq     _memset_end
        stab    0,x             ; set one more byte
        deca
        beq     _memset_end
        stab    1,x
        deca
        beq     _memset_end
        stab    2,x
;
_memset_end:
	pula
	pulb
;
_memset_ret:
	rts
;
