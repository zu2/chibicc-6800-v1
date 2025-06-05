	.setcpu 6800
	.bss
end_p:
	.ds 2
	.bss
start_p:
	.ds 2
	.data
next_p:
	.byte 0
	.byte 0
;
	.export _strtok
	.code
;
;       strtok(char *str_p, const char *delim);
;
;       AccAB:  str_p
;       +0:     return address
;       +2:     delim
;
delim   .equ    2
;
_strtok:
        tstb                    ; if (str_p) next_p = str_p;
        bne L_0
        tsta
        beq L_1
;
L_0:
	stab next_p+1
	staa next_p
L_1:
	ldx next_p              ; if (next_p==NULL) return NULL;
	jne skip_loop_begin
;
	clrb
	clra
	bra L_return_1
;
token_start_loop:
        clra
        tsx
	pshb
	psha
	ldab delim+1,x
	ldaa delim,x
	jsr _strchr             ; strchr(delim, *start_p)
	ins
	ins
	aba
	adca #0
	beq skip_loop_end
        ldx start_p
        inx
skip_loop_begin:
        stx start_p
	ldab 0,x
	bne token_start_loop
;
skip_loop_end:
	ldx start_p             ; if (!*start_p) {
	ldab 0,x
	bne L_3
;
        ldx #0                  ;   next_p = NULL;
        stx  next_p
	clrb
	clra
	bra L_return_1          ;   return NULL;
L_3:
        ldx start_p
        bra end_loop_begin
token_end_loop:
        clra
        tsx
	pshb
	psha
	ldab delim+1,x
	ldaa delim,x
	jsr _strchr
	ins
	ins
	aba
	adca #0
	bne L_4
        ldx end_p
        inx
end_loop_begin:
        stx end_p
	ldab 0,x
	bne token_end_loop
L_4:
        ldx end_p               ; if (*end_p) {
	ldab 0,x
	jeq L_5
        clr 0,x                 ;   *end_p = 0;
        inx                     ;   next_p = end_p + 1;
	bra L_6
L_5:                            ; } else {
        ldx #0                  ;   next_p = NULL;
L_6:
	stx next_p              ; }
	ldab start_p+1          ; return start_p;
	ldaa start_p
L_return_1:
	rts
;
