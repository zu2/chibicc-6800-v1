;	.file 1 "0803-itoa.c"
; 4. var->name=q, var->offset=0,var->ty->size=100, codegen.c 1582
; 4. var->name=p, var->offset=100,var->ty->size=2, codegen.c 1582
; 4. var->name=__alloca_size__, var->offset=102,var->ty->size=2, codegen.c 1582
; 4. var->name=argc, var->offset=106,var->ty->size=2, codegen.c 1582
; fn->stack_size = 108
; 9. var->name=argv, var->offset=110,var->ty->size=2, codegen.c 1636
; 4. var->name=, var->offset=0,var->ty->size=2, codegen.c 1582
; 4. var->name=, var->offset=2,var->ty->size=2, codegen.c 1582
; 4. var->name=, var->offset=4,var->ty->size=2, codegen.c 1582
; 4. var->name=p, var->offset=6,var->ty->size=2, codegen.c 1582
; 4. var->name=m, var->offset=8,var->ty->size=2, codegen.c 1582
; 4. var->name=sign, var->offset=10,var->ty->size=2, codegen.c 1582
; 4. var->name=__alloca_size__, var->offset=12,var->ty->size=2, codegen.c 1582
; 4. var->name=n, var->offset=16,var->ty->size=2, codegen.c 1582
; fn->stack_size = 18
; 9. var->name=buffer, var->offset=20,var->ty->size=2, codegen.c 1636
; 9. var->name=radix, var->offset=22,var->ty->size=2, codegen.c 1636
; 4. var->name=, var->offset=0,var->ty->size=2, codegen.c 1582
; 4. var->name=, var->offset=2,var->ty->size=2, codegen.c 1582
; 4. var->name=__alloca_size__, var->offset=4,var->ty->size=2, codegen.c 1582
; 4. var->name=p, var->offset=8,var->ty->size=2, codegen.c 1582
; fn->stack_size = 10
; 9. var->name=q, var->offset=12,var->ty->size=2, codegen.c 1636
; 4. var->name=, var->offset=0,var->ty->size=2, codegen.c 1582
; 4. var->name=, var->offset=2,var->ty->size=2, codegen.c 1582
; 4. var->name=__alloca_size__, var->offset=4,var->ty->size=2, codegen.c 1582
; 4. var->name=p, var->offset=8,var->ty->size=2, codegen.c 1582
; fn->stack_size = 10
; 9. var->name=q, var->offset=12,var->ty->size=2, codegen.c 1636
; 4. var->name=, var->offset=0,var->ty->size=2, codegen.c 1582
; 4. var->name=__alloca_size__, var->offset=2,var->ty->size=2, codegen.c 1582
; 4. var->name=s, var->offset=6,var->ty->size=2, codegen.c 1582
; fn->stack_size = 8
; fn->stack_size = 0
; fn->stack_size = 0
	.data
;  .type L__18, @object
;  .size L__18, 7
_L__18:
	.byte 45
	.byte 51
	.byte 50
	.byte 53
	.byte 48
	.byte 49
	.byte 0
	.data
;  .type L__17, @object
;  .size L__17, 5
_L__17:
	.byte 109
	.byte 97
	.byte 105
	.byte 110
	.byte 0
	.data
;  .type L__16, @object
;  .size L__16, 5
_L__16:
	.byte 109
	.byte 97
	.byte 105
	.byte 110
	.byte 0
	.data
;  .type L__13, @object
;  .size L__13, 5
_L__13:
	.byte 105
	.byte 116
	.byte 111
	.byte 97
	.byte 0
	.data
;  .type L__12, @object
;  .size L__12, 5
_L__12:
	.byte 105
	.byte 116
	.byte 111
	.byte 97
	.byte 0
	.data
;  .type L__9, @object
;  .size L__9, 7
_L__9:
	.byte 115
	.byte 116
	.byte 114
	.byte 99
	.byte 109
	.byte 112
	.byte 0
	.data
;  .type L__8, @object
;  .size L__8, 7
_L__8:
	.byte 115
	.byte 116
	.byte 114
	.byte 99
	.byte 109
	.byte 112
	.byte 0
	.data
;  .type L__5, @object
;  .size L__5, 7
_L__5:
	.byte 115
	.byte 116
	.byte 114
	.byte 99
	.byte 112
	.byte 121
	.byte 0
	.data
;  .type L__4, @object
;  .size L__4, 7
_L__4:
	.byte 115
	.byte 116
	.byte 114
	.byte 99
	.byte 112
	.byte 121
	.byte 0
	.data
;  .type L__1, @object
;  .size L__1, 7
_L__1:
	.byte 112
	.byte 117
	.byte 116
	.byte 115
	.byte 116
	.byte 114
	.byte 0
	.data
;  .type L__0, @object
;  .size L__0, 7
_L__0:
	.byte 112
	.byte 117
	.byte 116
	.byte 115
	.byte 116
	.byte 114
	.byte 0
	.export _main
	.code
_main:
; function prologue emit_text codegen.c 1790
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	pshb
	psha
	sts @bp
	ldab @bp+1
	ldaa @bp
	subb #<103
	sbca #>103
	stab @bp+1
	staa @bp
	ldx @bp
	txs
	ldab @bp+1
	ldaa @bp
	addb #<100
	adca #>100
	pshb
	psha
; push_args codegen.c 557
; push_args gp=0 codegen.c 563
; push_args gp=0, GP_MAX=1, codegen.c 610
; push_args gp=1, GP_MAX=1, codegen.c 610
; push_args gp=2, GP_MAX=1, codegen.c 610
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=1, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab #<10
	ldaa #>10
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=1, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=0, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=1, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=1, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=0, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab #<32501
	ldaa #>32501
	nega
	negb
	sbca #0
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
;↑stack_args=4  gen_expr codegen.c 984
;↑depth=8  gen_expr codegen.c 985
	ldab #<_itoa
	ldaa #>_itoa
;↑made by gen_expr codegen.c 987
;↑depth=8  gen_expr codegen.c 988
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
; pop(%rdi) codegen.c 39
	pula
	pulb
	jsr 0,x
;↑made by gen_expr codegen.c 1053
;↑depth=6  gen_expr codegen.c 1054
;↑stack_args=6  gen_expr codegen.c 1059
	ins
	ins
	ins
	ins
;↑depth=2  gen_expr codegen.c 1065
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; push_args codegen.c 557
; push_args gp=0 codegen.c 563
; push_args gp=0, GP_MAX=1, codegen.c 610
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=0, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=0, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab @bp+1
	ldaa @bp
	addb #<100
	adca #>100
	jsr __load16	; AccAB = (AccAB)
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
;↑stack_args=0  gen_expr codegen.c 984
;↑depth=2  gen_expr codegen.c 985
	ldab #<_putstr
	ldaa #>_putstr
;↑made by gen_expr codegen.c 987
;↑depth=2  gen_expr codegen.c 988
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
; pop(%rdi) codegen.c 39
	pula
	pulb
	jsr 0,x
;↑made by gen_expr codegen.c 1053
;↑depth=0  gen_expr codegen.c 1054
;↑stack_args=0  gen_expr codegen.c 1059
;↑depth=0  gen_expr codegen.c 1065
; push_args codegen.c 557
; push_args gp=0 codegen.c 563
; push_args gp=0, GP_MAX=1, codegen.c 610
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=0, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=0, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab #<10
	ldaa #>10
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
;↑stack_args=0  gen_expr codegen.c 984
;↑depth=2  gen_expr codegen.c 985
	ldab #<_putc
	ldaa #>_putc
;↑made by gen_expr codegen.c 987
;↑depth=2  gen_expr codegen.c 988
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
; pop(%rdi) codegen.c 39
	pula
	pulb
	jsr 0,x
;↑made by gen_expr codegen.c 1053
;↑depth=0  gen_expr codegen.c 1054
;↑stack_args=0  gen_expr codegen.c 1059
;↑depth=0  gen_expr codegen.c 1065
; push_args codegen.c 557
; push_args gp=0 codegen.c 563
; push_args gp=0, GP_MAX=1, codegen.c 610
; push_args gp=1, GP_MAX=1, codegen.c 610
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=1, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab #<_L__18
	ldaa #>_L__18
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=0, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=1, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=0, codegen.c 497
; push_args2 call gen_expr codegen.c 501
	ldab @bp+1
	ldaa @bp
	addb #<100
	adca #>100
	jsr __load16	; AccAB = (AccAB)
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
;↑stack_args=2  gen_expr codegen.c 984
;↑depth=4  gen_expr codegen.c 985
	ldab #<_strcmp
	ldaa #>_strcmp
;↑made by gen_expr codegen.c 987
;↑depth=4  gen_expr codegen.c 988
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
; pop(%rdi) codegen.c 39
	pula
	pulb
	jsr 0,x
;↑made by gen_expr codegen.c 1053
;↑depth=2  gen_expr codegen.c 1054
;↑stack_args=2  gen_expr codegen.c 1059
	ins
	ins
;↑depth=0  gen_expr codegen.c 1065
	jmp L_return_main
	ldab #<0
	ldaa #>0
L_return_main:
	stab @tmp1+1
	staa @tmp1
	ldab @bp+1
	ldaa @bp
	addb #<103
	adca #>103
	stab @bp+1
	staa @bp
	lds @bp
	pula
	pulb
	stab @bp+1
	staa @bp
	ins
	ins
	ldab @tmp1+1
	ldaa @tmp1
	rts
	.export _itoa
	.code
_itoa:
; function prologue emit_text codegen.c 1790
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	pshb
	psha
	sts @bp
	ldab @bp+1
	ldaa @bp
	subb #<13
	sbca #>13
	stab @bp+1
	staa @bp
	ldx @bp
	txs
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<10
	ldaa #>10
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<22
	adca #>22
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	jsr __ne16
	aba
	adca #0
	jeq L_else_1
	ldab #<0
	ldaa #>0
	; movzwl %ax, %eax XXX i32u16codegen.c
	jmp L_return_itoa
	jmp L_end_1
L_else_1:
L_end_1:
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<8
	ldaa #>8
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<20
	adca #>20
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<4
	adca #>4
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<4
	adca #>4
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<4
	adca #>4
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	pshb
	psha
	ldab #<0
	ldaa #>0
	; movzbl %al, %eax XXX i32u8 codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=1, codegen.c 282
	stab 0,x
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<0
	ldaa #>0
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<16
	adca #>16
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	jsr __ge16s
	aba
	adca #0
	jeq L_else_2
	ldab @bp+1
	ldaa @bp
	addb #<10
	adca #>10
	pshb
	psha
	ldab #<0
	ldaa #>0
	; movzwl %ax, %eax XXX i32u16codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<16
	adca #>16
	jsr __load16	; AccAB = (AccAB)
	; movzwl %ax, %eax XXX i32u16codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	jmp L_end_2
L_else_2:
	ldab @bp+1
	ldaa @bp
	addb #<10
	adca #>10
	pshb
	psha
	ldab #<1
	ldaa #>1
	; movzwl %ax, %eax XXX i32u16codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<16
	adca #>16
	jsr __load16	; AccAB = (AccAB)
	nega
	negb
	sbca #0
	; movzwl %ax, %eax XXX i32u16codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
L_end_2:
L_begin_3:
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<48
	ldaa #>48
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<10
	ldaa #>10
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
; end call
	jsr __rem16x16u
	ins
	ins
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	; movzbl %al, %eax XXX i32u8 codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=1, codegen.c 282
	stab 0,x
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<10
	ldaa #>10
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
; end call
	jsr __div16x16u
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
L__15:
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<0
	ldaa #>0
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	jsr __gt16u
	aba
	adca #0
	jne L_begin_3
L__14:
	ldab @bp+1
	ldaa @bp
	addb #<10
	adca #>10
	jsr __load16	; AccAB = (AccAB)
	aba
	adca #0
	jeq L_else_4
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	pshb
	psha
	ldab #<45
	ldaa #>45
	; movzbl %al, %eax XXX i32u8 codegen.c
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=1, codegen.c 282
	stab 0,x
	jmp L_end_4
L_else_4:
L_end_4:
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	jsr __load16	; AccAB = (AccAB)
	jmp L_return_itoa
L_return_itoa:
	stab @tmp1+1
	staa @tmp1
	ldab @bp+1
	ldaa @bp
	addb #<13
	adca #>13
	stab @bp+1
	staa @bp
	lds @bp
	pula
	pulb
	stab @bp+1
	staa @bp
	ins
	ins
	ldab @tmp1+1
	ldaa @tmp1
	rts
	.export _strcmp
	.code
_strcmp:
; function prologue emit_text codegen.c 1790
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	pshb
	psha
	sts @bp
	ldab @bp+1
	ldaa @bp
	subb #<5
	sbca #>5
	stab @bp+1
	staa @bp
	ldx @bp
	txs
L_begin_5:
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	aba
	adca #0
	jeq L_false_7
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	aba
	adca #0
	beq L_false_7
	ldab #1
	bra L_end_7
L_false_7:
	clrb
L_end_7:
	clra
	aba
	adca #0
	jeq L_false_6
; call gen_expr(node->rhs) codegen.c 1232
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	jsr __eq16
	aba
	adca #0
	beq L_false_6
	ldab #1
	bra L_end_6
L_false_6:
	clrb
L_end_6:
	clra
	aba
	adca #0
	jeq L__10
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<65535
	ldaa #>65535
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<65535
	ldaa #>65535
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
L__11:
	jmp L_begin_5
L__10:
; call gen_expr(node->rhs) codegen.c 1232
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
; end call
	tsx
	subb 1,x
	sbca 0,x
	ins
	ins
	jmp L_return_strcmp
L_return_strcmp:
	stab @tmp1+1
	staa @tmp1
	ldab @bp+1
	ldaa @bp
	addb #<5
	adca #>5
	stab @bp+1
	staa @bp
	lds @bp
	pula
	pulb
	stab @bp+1
	staa @bp
	ins
	ins
	ldab @tmp1+1
	ldaa @tmp1
	rts
	.export _strcpy
	.code
_strcpy:
; function prologue emit_text codegen.c 1790
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	pshb
	psha
	sts @bp
	ldab @bp+1
	ldaa @bp
	subb #<5
	sbca #>5
	stab @bp+1
	staa @bp
	ldx @bp
	txs
L_begin_8:
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	aba
	adca #0
	jeq L__6
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<65535
	ldaa #>65535
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<8
	adca #>8
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<2
	adca #>2
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<65535
	ldaa #>65535
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<12
	adca #>12
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	jsr __load8s	; AccB = (AccAB)
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=1, codegen.c 282
	stab 0,x
L__7:
	jmp L_begin_8
L__6:
L_return_strcpy:
	stab @tmp1+1
	staa @tmp1
	ldab @bp+1
	ldaa @bp
	addb #<5
	adca #>5
	stab @bp+1
	staa @bp
	lds @bp
	pula
	pulb
	stab @bp+1
	staa @bp
	ins
	ins
	ldab @tmp1+1
	ldaa @tmp1
	rts
	.export _putstr
	.code
_putstr:
; function prologue emit_text codegen.c 1790
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	pshb
	psha
	sts @bp
	ldab @bp+1
	ldaa @bp
	subb #<3
	sbca #>3
	stab @bp+1
	staa @bp
	ldx @bp
	txs
L_begin_9:
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	jsr __load16	; AccAB = (AccAB)
	jsr __load8s	; AccB = (AccAB)
	aba
	adca #0
	jeq L__2
; push_args codegen.c 557
; push_args gp=0 codegen.c 563
; push_args gp=0, GP_MAX=1, codegen.c 610
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=1, args->pass_by_stack=0, codegen.c 497
; push_args2 args=1, codegen.c 496
; push_args2 first_pass=0, args->pass_by_stack=0, codegen.c 497
; push_args2 call gen_expr codegen.c 501
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<65535
	ldaa #>65535
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	pshb
	psha
	ldab @bp+1
	ldaa @bp
	addb #<6
	adca #>6
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	pshb
	psha
; call gen_expr(node->rhs) codegen.c 1232
; call gen_expr(node->rhs) codegen.c 1232
	ldab #<1
	ldaa #>1
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab #<1
	ldaa #>1
; end call
	jsr __mul16x16
	ins
	ins
	; movzwl %ax, %eax XXX i32u16codegen.c
	pshb
	psha
; call gen_expr(node->lhs) codegen.c 1235
	ldab @bp+1
	ldaa @bp
	addb #<0
	adca #>0
	jsr __load16	; AccAB = (AccAB)
	jsr __load16	; AccAB = (AccAB)
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	tsx
	ldx 0,x
	ins
	ins
; store ty->size=2, codegen.c 282
	stab 1,x
	staa 0,x
; end call
	tsx
	addb 1,x
	adca 0,x
	ins
	ins
	jsr __load8s	; AccB = (AccAB)
; push_args2 end  gen_expr codegen.c 503
; push_args2 call push() by default codegen.c 530
	pshb
	psha
;↑stack_args=0  gen_expr codegen.c 984
;↑depth=10  gen_expr codegen.c 985
	ldab #<_putc
	ldaa #>_putc
;↑made by gen_expr codegen.c 987
;↑depth=10  gen_expr codegen.c 988
	pshb
	psha
	tsx
	ldx 0,x
	ins
	ins
; pop(%rdi) codegen.c 39
	pula
	pulb
	jsr 0,x
;↑made by gen_expr codegen.c 1053
;↑depth=8  gen_expr codegen.c 1054
;↑stack_args=8  gen_expr codegen.c 1059
;↑depth=8  gen_expr codegen.c 1065
L__3:
	jmp L_begin_9
L__2:
L_return_putstr:
	stab @tmp1+1
	staa @tmp1
	ldab @bp+1
	ldaa @bp
	addb #<3
	adca #>3
	stab @bp+1
	staa @bp
	lds @bp
	pula
	pulb
	stab @bp+1
	staa @bp
	ins
	ins
	ldab @tmp1+1
	ldaa @tmp1
	rts
