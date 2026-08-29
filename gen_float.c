#include "chibicc.h"

void
gen_direct_pushf(double fval)
{
  union { float f32; uint32_t u32; } u = { fval };

  println("; push float %e, %08x",u.f32,u.u32);
  gen_direct_pushl(u.u32);
}

typedef struct FloatLiteral FloatLiteral;
struct FloatLiteral {
  FloatLiteral *next;
  uint32_t      bits;
};

static FloatLiteral *float_literals;

char *float_literal_label(double fval)
{
  union { float f32; uint32_t u32; } u = { fval };
  FloatLiteral *fl;

  for (fl = float_literals; fl; fl = fl->next) {
    if (fl->bits == u.u32) {
      return format("CF_%08x",u.u32);
    }
  }
  fl = calloc(1,sizeof(FloatLiteral));
  fl->bits = u.u32;
  fl->next = float_literals;
  float_literals = fl;
  return format("CF_%08x",u.u32);
}

void emit_float_literals(void)
{
  if (!float_literals) {
    return;
  }
  println("\t.data");
  for (FloatLiteral *fl = float_literals; fl; fl = fl->next) {
    union { float f32; uint32_t u32; } u = { .u32 = fl->bits };
    println(";");
    println("; float %e",u.f32);
    println("CF_%08x:",fl->bits);
    println("\t.word %d",(fl->bits>>16) & 0xffff);
    println("\t.word %d",(fl->bits    ) & 0xffff);
  }
}

void cmpf32x(int off)
{
  if (off==0) {
    println("\tjsr __cmpf32x");
  }else if (1<=off && off<=255) {
    ldab_i(off);
    println("\tjsr __cmpf32bx");
    IX_invalidate();
  }else{
    ldd_i(off);
    println("\tjsr __cmpf32dx");
    IX_invalidate();
  }
}

void gen_expr_float(Node *node)
{
  double fval;
  char *addr;

  switch (node->kind) {
  case ND_NEG:
    gen_expr(node->lhs);
    println("\tldab @long	; negate float");
    println("\teorb #$80");
    println("\tstab @long");
    return;
  case ND_ADD:
    if (is_flonum_constant(node->rhs, &fval)) {
      gen_expr(node->lhs);
      ldx_IMM_STR(float_literal_label(fval));
      println("\tjsr __addf32x");
      IX_invalidate();
      return;
    }else if ((addr = is_var_addr_constant(node->rhs))) {
      gen_expr(node->lhs);
      ldx_IMM_STR(addr);
      println("\tjsr __addf32x");
      IX_invalidate();
      return;
    }else if (test_addr_x(node->rhs)) {
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs);
      if (off==0) {
        println("\tjsr __addf32x");
      }else if (1<=off && off<=255) {
        ldab_i(off);
        println("\tjsr __addf32bx");
      }else{
        ldd_i(off);
        println("\tjsr __addf32dx");
      }
      IX_invalidate();
      return;
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
    }
    gen_expr(node->lhs);	// xmm0
    println("\tjsr __addf32tos");
    IX_invalidate();
    depth -= 4;
    return;
  case ND_SUB:
    if (is_flonum_constant(node->rhs, &fval)) {
      gen_expr(node->lhs);
      ldx_IMM_STR(float_literal_label(fval));
      println("\tjsr __subf32x");
      IX_invalidate();
      return;
    }else if ((addr = is_var_addr_constant(node->rhs))) {
      gen_expr(node->lhs);
      ldx_IMM_STR(addr);
      println("\tjsr __subf32x");
      IX_invalidate();
      return;
    }else if (test_addr_x(node->rhs)) {
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs);
      if (off==0) {
        println("\tjsr __subf32x");
      }else if (1<=off && off<=255) {
        ldab_i(off);
        println("\tjsr __subf32bx");
      }else{
        ldd_i(off);
        println("\tjsr __subf32dx");
      }
      IX_invalidate();
      return;
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
    }
    gen_expr(node->lhs);	// xmm0
    println("\tjsr __subf32tos");
    IX_invalidate();
    depth -= 4;
    return;
  case ND_MUL:
    if (is_flonum_constant(node->rhs, &fval)) {
      int k;
      // exp * 2^k
      if (isfinite(fval) && fval != 0
      &&  fabsf(frexpf((float)fval, &k)) == 0.5f) { // rhs == 0.5 * 2^k ?
          gen_expr(node->lhs);
        if (fval < 0) {
          println("\tldab @long");
          println("\teorb #$80");
          println("\tstab @long");
        }
        if (k!=1) { // * 1.0f?
          ldd_i(k-1);
          push();
          println("\tjsr _ldexpf");
          IX_invalidate();
          remove_args(2);
        }
        return;
      }
      // exp * const
      gen_expr(node->lhs);
      ldx_IMM_STR(float_literal_label(fval));
      println("\tjsr __mulf32x");
      IX_invalidate();
      return;
    }
    if ((addr = is_var_addr_constant(node->rhs))) {
      gen_expr(node->lhs);
      ldx_IMM_STR(addr);
      println("\tjsr __mulf32x");
      IX_invalidate();
      return;
    }else if (test_addr_x(node->rhs)) {
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs);
      if (off==0) {
        println("\tjsr __mulf32x");
      }else if (1<=off && off<=255) {
        ldab_i(off);
        println("\tjsr __mulf32bx");
      }else{
        ldd_i(off);
        println("\tjsr __mulf32dx");
      }
      IX_invalidate();
      return;
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
    }
    gen_expr(node->lhs);	// xmm0
    println("\tjsr __mulf32tos");
    IX_invalidate();
    depth -= 4;
    return;
  case ND_DIV:
    if (is_flonum_constant(node->rhs, &fval)) {
      int k;
      // exp / 2^k
      if (isfinite(fval) && fval != 0
      &&  fabsf(frexpf((float)fval, &k)) == 0.5f) { // rhs == 0.5 * 2^k ?
          gen_expr(node->lhs);
        if (fval < 0) {
          println("\tldab @long");
          println("\teorb #$80");
          println("\tstab @long");
        }
        if (k!=1) { // / 1.0f?
          ldd_i(1-k);
          push();
          println("\tjsr _ldexpf");
          IX_invalidate();
          remove_args(2);
        }
        return;
      }
      // exp / const
      gen_expr(node->lhs);
      ldx_IMM_STR(float_literal_label(fval));
      println("\tjsr __divf32x");
      IX_invalidate();
      return;
    }
    if ((addr = is_var_addr_constant(node->rhs))) {
      gen_expr(node->lhs);
      ldx_IMM_STR(addr);
      println("\tjsr __divf32x");
      IX_invalidate();
      return;
    }else if (test_addr_x(node->rhs)) {
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs);
      if (off==0) {
        println("\tjsr __divf32x");
      }else if (1<=off && off<=255) {
        ldab_i(off);
        println("\tjsr __divf32bx");
      }else{
        ldd_i(off);
        println("\tjsr __divf32dx");
      }
      IX_invalidate();
      return;
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
    }
    gen_expr(node->lhs);	// xmm0
    println("\tjsr __divf32tos");
    IX_invalidate();
    depth -= 4;
    return;
  case ND_EQ:
  case ND_NE:
    if (is_flonum_constant(node->rhs, &fval)
    &&  fval == 0.0) {
      gen_expr(node->lhs);
      println("\tjsr __f32iszero");	// ±0.0: AccB=0, NaN: AccB!=0
      IX_invalidate();
      println("\tclra");
      println("\tnegb");		// C=1 if AccB!=0
      if (node->kind == ND_EQ) {
        println("\tldab #1");
        println("\tsbcb #0");
      } else {
        println("\trolb");
        println("\tandb #1");
      }
      return;
    }
    // fall through
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE: { // float relop float
    if (is_flonum_constant(node->rhs, &fval)) {
      gen_expr(node->lhs);
      ldx_IMM_STR(float_literal_label(fval));
      println("\tjsr __cmpf32x");
    }else if ((addr = is_var_addr_constant(node->rhs))) {
      gen_expr(node->lhs);
      ldx_IMM_STR(addr);
      println("\tjsr __cmpf32x");
    }else if (test_addr_x(node->rhs)) {
      gen_expr(node->lhs);
      cmpf32x(gen_addr_x(node->rhs));
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
      gen_expr(node->lhs);	// xmm0
      println("\tjsr __cmpf32tos");	// @long cmp  TOS");
      depth -= 4;
    }
    char *L_cmpf1 = new_jump_label();
    char *L_cmpf2 = new_jump_label();
    IX_invalidate();
    println("\tbcc %s",L_cmpf1);	// when carry=1, compare NaN
    println("\tclra");
    if (node->kind == ND_NE) {
      ldab_i(1);
    }else{
      println("\tclrb");
    }
    println("\tbra %s",L_cmpf2);
    println("%s:",L_cmpf1);
    println("\tclra");
    // Set bit 0 to compare result before andb #1.
    if (node->kind == ND_EQ) {          // AccB:00 true
      println("\teorb #1");	// FF,00,01 -> FE,01,00
      println("\tandb #1"); //          -> 00,01,00
    } else if (node->kind == ND_NE) {   // AccB:FF,01 true
      println("\tandb #1"); // FF,00,01 -> 01,00,01
    } else if (node->kind == ND_LT) {	  // AccB:FF true
      println("\tlsrb");		// FF,00,01 -> 7F,00,00
      println("\tandb #1"); //          -> 01,00,00
    } else if (node->kind == ND_GT) {	  // AccB:01 true
      println("\tincb");		// FF,00,01 -> 00,01,02
      println("\tlsrb");		//          -> 00,00,01
    } else if (node->kind == ND_LE) {   // AccB:FF,00 true
      println("\tdecb");		// FF,00,01 -> FE,FF,00
      println("\tlsrb");		//          -> 7F,7F,00
      println("\tandb #1"); //          -> 01,01,00
    } else if (node->kind == ND_GE) {   // AccB:00,01 true
      println("\tincb");	  // FF,00,01 -> 00,01,02
      println("\tlsrb");    //          -> 00,00,01 C=0,1,0
      println("\tadcb #0"); //          -> 00,01,01
    } else {
      error_tok(node->tok, "invalid expression");
    }
    println("%s:",L_cmpf2);
    IX_invalidate();
    return;
    }
  }

  error_tok(node->tok, "invalid expression");
}
