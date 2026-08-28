;
;	unsigned long __u32exp(unsigned long z)
;
;	entry:	@long = z,  0 <= z <= 0x2C5C85FD	; ln2 * 2^30
;	exit:	@long = exp(z)
;
;	Both are fixed point with 1.0 = 0x40000000	; 1UL << 30
;
;	// table[i-1] = ln(1 + 2^-i) * (1UL << 30), i = 1..14
;	static const unsigned long table[] = {
;	  0x19F323EDUL, 0x0E47FBE4UL, 0x0789C1DCUL, 0x03E14618UL,
;	  0x01F829B1UL, 0x00FE0546UL, 0x007F80AAUL, 0x003FE015UL,
;	  0x001FF803UL, 0x000FFE00UL, 0x0007FF80UL, 0x0003FFE0UL,
;	  0x0001FFF8UL, 0x0000FFFEUL
;	};
;
;	unsigned long u32exp_ref(unsigned long z)
;	{
;	  unsigned long y;
;	  unsigned int u;
;	  unsigned char i;
;
;	  y = 1UL << 30;
;	  for (i = 1; i <= sizeof table / sizeof table[0]; i++) {
;	    if (z >= table[i - 1]) {
;	      z -= table[i - 1];
;	      y += y >> i;
;	    }
;	  }
;
;	  u = (unsigned int)((y - (1UL << 30)) >> (30 - 16));
;
;	  return y + z + (((unsigned long)u * (unsigned int)z) >> 16);
;	}
;
;
	.export ___u32exp
;
	.data
__y:	.byte	0,0,0,0
__tmp:	.byte	0,0,0,0		; y >> i
;
	.code
___u32exp:
	ldx	#$4000		; y = 1 << 30
	stx	__y
	ldx	#0
	stx	__y+2
	ldaa	#1		; i = 1
	ldx	#__table
;
loop:
	ldab	@long		; z < table[i-1] ?
	cmpb	0,x
	bne	cmp
	ldab	@long+1
	cmpb	1,x
	bne	cmp
	ldab	@long+2
	cmpb	2,x
	bne	cmp
	ldab	@long+3
	cmpb	3,x
cmp:
	bcs	next		; if z < table[i-1], skip
;
	ldab	@long+3		; z -= table[i-i]
	subb	3,x
	stab	@long+3
	ldab	@long+2
	sbcb	2,x
	stab	@long+2
	ldab	@long+1
	sbcb	1,x
	stab	@long+1
	ldab	@long
	sbcb	0,x
	stab	@long
;
	bsr	shift	; __tmp = y >> i. A and X are left alone.
;
	ldab	__y+3		; y += __tmp
	addb	__tmp+3
	stab	__y+3
	ldab	__y+2
	adcb	__tmp+2
	stab	__y+2
	ldab	__y+1
	adcb	__tmp+1
	stab	__y+1
	ldab	__y
	adcb	__tmp
	stab	__y
;
next:
	inx
	inx
	inx
	inx
	inca
	cmpa	#15
	bne	loop
;
	ldab	@long+3		; push z, which is below 2^16 after 14 steps
	pshb
	ldab	@long+2
	pshb
;
;	AccAB = (y - (1UL<<30)) >> 14
;
;	y     = ..uuuuuu uuuuuuuu uu...... ........
;		  ^^^^^^^^^^^^^^^^^^
;
;	y - (1UL<<30) changes b31:b30 only, so u ignores it
;
	ldab	__y+2
	aslb
	stab	__tmp
;
	ldab	__y+1
	ldaa	__y
	rolb
	rola
;
	asl	__tmp
	rolb
	rola
	jsr	__mul16x16u_32	; @long = AccAB * TOS ; (u*z)
;
	pula
	pulb
	clr	__tmp		; __tmp:AccAB = z + (@long >> 16)
	addb	@long+1
	adca	@long
	rol	__tmp
;
	addb	__y+3		; @long = y + __tmp:AccAB
	stab	@long+3
	adca	__y+2
	staa	@long+2
	ldab	__y+1
	adcb	__tmp
	stab	@long+1
	ldab	__y
	adcb	#0
	stab	@long
	rts
;
;	__tmp = y >> i		; i:AccA
;
shift:
	psha
	bita	#8
	bne	shbig
	ldab	__y+1
	stab	__tmp+1
	ldab	__y+2
	stab	__tmp+2
	ldab	__y+3
	stab	__tmp+3
	ldab	__y
	bra	shloop
shbig:
	ldab	__y
	stab	__tmp+1
	ldab	__y+1
	stab	__tmp+2
	ldab	__y+2
	stab	__tmp+3
	clrb
	suba	#8
	beq	shdone
shloop:
	lsrb
	ror	__tmp+1
	ror	__tmp+2
	ror	__tmp+3
	deca
	bne	shloop
shdone:
	stab	__tmp
	pula
;
	rts

	.data
__table:
	.byte	$19,$f3,$23,$ed		; ln(1 + 2^-1)
	.byte	$0e,$47,$fb,$e4		; ln(1 + 2^-2)
	.byte	$07,$89,$c1,$dc		; ln(1 + 2^-3)
	.byte	$03,$e1,$46,$18		; ln(1 + 2^-4)
	.byte	$01,$f8,$29,$b1		; ln(1 + 2^-5)
	.byte	$00,$fe,$05,$46		; ln(1 + 2^-6)
	.byte	$00,$7f,$80,$aa		; ln(1 + 2^-7)
	.byte	$00,$3f,$e0,$15		; ln(1 + 2^-8)
	.byte	$00,$1f,$f8,$03		; ln(1 + 2^-9)
	.byte	$00,$0f,$fe,$00		; ln(1 + 2^-10)
	.byte	$00,$07,$ff,$80		; ln(1 + 2^-11)
	.byte	$00,$03,$ff,$e0		; ln(1 + 2^-12)
	.byte	$00,$01,$ff,$f8		; ln(1 + 2^-13)
	.byte	$00,$00,$ff,$fe		; ln(1 + 2^-14)
