#include "chibicc.h"

bool is_compare(Node *node)
{
  switch (node->kind) {
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    return true;
  }
  return false;
}

#if 0
bool is_compare_or_not(Node *node)
{
  switch (node->kind) {
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
  case ND_NOT:
    return true;
  }
  return false;
}
#endif

bool is_boolean_result(Node *node)
{
  switch (node->kind) {
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
  case ND_NOT:
  case ND_LOGAND:
  case ND_LOGOR:
    return true;
  }
  return false;
}

Type *is_byte(Node *node)
{
  int64_t val;

  if (is_integral_promotion(node)) {
    node = skip_integral_promotion(node);
  }
  if (is_int8(node->ty)) {
    return node->ty;
  }

  if (node->kind == ND_NUM && is_integer_constant(node, &val)) {
    if (node->ty->is_unsigned && val >= 0 && val <= 255) {
      return node->ty;
    }
    if (!node->ty->is_unsigned && val >= -128 && val <= 127) {
      return node->ty;
    }
  }
  return NULL;
}

// (!= ty_int (ND_CAST TY_PTR(10):u (ND_VAR TY_ARRAY(12) arr global)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
// (!= ty_int (ND_CAST TY_PTR(10):u (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) arr global) 0)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
// (!= ty_int (ND_CAST TY_PTR(10):u (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) arr global) 100)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
char *is_addr_constant(Node *node)
{
  int64_t val;

  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_PTR
  &&  node->lhs->kind == ND_VAR
  &&  node->lhs->ty->kind == TY_ARRAY
  &&  !node->lhs->var->is_local) {
    return node->lhs->var->name;
  }
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_PTR
  &&  node->lhs->kind == ND_ADD
  &&  node->lhs->ty->kind == TY_ARRAY
  &&  node->lhs->lhs->kind == ND_VAR
  &&  node->lhs->lhs->ty->kind == TY_ARRAY
  &&  !node->lhs->lhs->var->is_local
  &&  is_integer_constant(node->lhs->rhs,&val)) {
    char *p = calloc(1,strlen(node->lhs->lhs->var->name)+32);
    sprintf(p,"%s+%ld",node->lhs->lhs->var->name,val);
    return p;
  }
  return NULL;
}

bool is_global_var_with_cast(Node *node)
{
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_PTR
  &&  node->lhs->ty->kind == TY_PTR) {
    return is_global_var(node->lhs);
  }
  return is_global_var(node);
}

Node *skip_redundant_ptr_cast(Node *node)
{
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_PTR
  &&  node->lhs->ty->kind == TY_PTR) {
    return node->lhs;
  }
  return node;
}

static int isNUM(Node *node) { return node->kind == ND_NUM; }
static int isVAR(Node *node) { return node->kind == ND_VAR; }

//
// Compare two 8 signed/unsigned integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return false.
//
static bool gen_jump_if_false_8bit(Node *node, char *if_false)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int64_t val;

  if (node->kind == ND_BITAND
  &&  is_integer_constant(node->rhs, &val)) {
    if ((val & ~0xFF)==0
    ||  (is_int8(node->lhs->ty) && node->lhs->ty->is_unsigned)) {
      gen_expr(lhs);
      println("\tandb #<%ld", val);
      println("\tjeq %s", if_false);
      return true;
    }
  }

  if (!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjeq %s", if_false);
    return true;
  }

  if ( !is_int8(lhs->ty) || !is_int8(rhs->ty) ){
    return false;
  }

  if (can_direct_char(rhs)) {
    if (is_integer_constant(rhs, &val)) {
      gen_expr(lhs);
      if (val == 0) {
        println("\ttstb");
      }else{
        println("\tcmpb #%ld", rhs->val);
      }
    } else {
      gen_expr(lhs);
      gen_direct_char(rhs, "cmpb", NULL);
    }
  }else if (test_addr_x(rhs)) {
    gen_expr(lhs);
    int off = gen_addr_x(rhs,true);
    println("\tcmpb %d,x",off);
  }else{
    gen_expr(lhs);
    push1();
    gen_expr(rhs);
    popa();
    println("\tcba");
  }

  switch (node->kind) {
  case ND_EQ:
    println("\tjne %s", if_false);
    break;
  case ND_NE:
    println("\tjeq %s", if_false);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned) {
      println("\tjcc %s", if_false);
    } else {
      println("\tjge %s", if_false);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_false);
    } else {
      println("\tjlt %s", if_false);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned) {
      println("\tjhi %s", if_false);
    } else {
      println("\tjgt %s", if_false);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned) {
      println("\tjls %s", if_false);
    } else {
      println("\tjle %s", if_false);
    }
    break;
  }
  return true;
}

//
// Compare two 8- or 16-bit integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return false.
//
bool gen_jump_if_false(Node *node, char *if_false)
{
  int64_t val;

  node = optimize_expr(node);
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  char *addr;
  char *if_thru = new_label("L_thru_%d");

  if (is_integer_constant(node,&val)) {
    if (val==0) {
      println("\tjmp %s",if_false);
    }
    return true;
  }
      
  if (node->kind == ND_NOT) {
    return gen_jump_if_true(lhs, if_false);
  }

  if (is_int16_or_ptr(node->ty)) {
    if (test_expr_x(node)) {
      gen_expr_x(node,false);
      println("\tjeq %s", if_false);
      return true;
    }
    if (test_addr_x(node)) {
      int off = gen_addr_x(node,false);
      ldx_nX(off);
      println("\tjeq %s", if_false);
      return true;
    }
  }

  if (node->kind == ND_LOGOR) {
    gen_jump_if_true(lhs, if_thru);
    gen_jump_if_false(rhs, if_false);
    println("%s:", if_thru);
    IX_Dest = IX_None;
    return true;
  }
  if (node->kind == ND_LOGAND) {
    gen_jump_if_false(lhs, if_false);
    gen_jump_if_false(rhs, if_false);
    return true;
  }

  if (node->kind == ND_BITAND && is_int8(node->ty)) {
    if (gen_jump_if_false_8bit(node,if_false)) {
      return true;
    }
  }

  if (!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjeq %s", if_false);
    return true;
  }

  // If one side is ND_NUM, both sides are promoted to int,
  // so this can't be optimized. TODO: Fix optimize.c
  if (is_byte(lhs) && is_byte(rhs)) {
    if (gen_jump_if_false_8bit(node, if_false)) {
      return true;
    }
  }

  // special long case
  if (lhs->ty->kind == TY_LONG
  && is_integer_constant(rhs,&val)
  && val == 0
  && test_addr_x(lhs)) {
    switch(node->kind) {
    case ND_LT:
      if (lhs->ty->is_unsigned) {
        println("; ulong < 0 is always false");
        println("\tjmp %s",if_false);
      }else{
        int off = gen_addr_x(lhs,false);
        println("\tldab %d,x",off);
        println("\tjpl %s", if_false);
      }
      return true;
    case ND_GE:
      if (lhs->ty->is_unsigned) {
        println("; ulong >= 0 is always true");
      }else{
        int off = gen_addr_x(lhs,false);
        println("\ttst %d,x",off);
        println("\tjmi %s", if_false);
      }
      return true;
    }
    goto fallback;
  }
  if (!is_int16_or_ptr(lhs->ty)) {
    goto fallback;
  }
  if (!is_int16_or_ptr(rhs->ty)) {
    goto fallback;
  }

  // if (expr op 0)
  if (is_integer_constant(rhs,&val) && val==0) {
    if (test_expr_x(lhs)) {
      gen_expr_x(lhs,false); // Evaluate LHS anyway; no side-effect check yet.
      println("\tcpx #0");
      switch (node->kind) {
      case ND_EQ:
        println("\tjne %s", if_false);
        return true;
      case ND_NE:
        println("\tjeq %s", if_false);
        return true;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          println("\tjmp %s", if_false);
          return true;
        }
        println("\tjpl %s", if_false);
        return true;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          println("\tjeq %s", if_false);
          return true;
        } else {
          println("; int>0 is !(int<0) && !(int==0)");
          println("\tjmi %s", if_false);
          println("\tjeq %s", if_false);
          return true;
        }
        break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          return true;
        } else {
          println("\tjmi %s", if_false);
          return true;
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          println("\tjne %s", if_false);
          return true;
        } else {
          println("; int<=0 is !(int==0 || int<0)");
          println("\tbeq %s", if_thru);
          println("\tjpl %s", if_false);
          println("%s:",if_thru);
          return true;
        }
        break;
      }
    } else {
      gen_expr(lhs);  // Evaluate LHS anyway; no side-effect check yet.
      switch (node->kind) {
      case ND_EQ:
      case ND_NE:
        break;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          println("\tjmp %s", if_false);
          return true;
        }
        // 'subb #0 / sbca #0' can be substituted with 'tsta'.
        println("\ttsta");
        break;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          node->kind = ND_NE;
        } else {
          println("\ttsta");
        }
        break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          return true;
        } else {
          println("\ttsta");
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          node->kind = ND_EQ;
        } else {  // int<=0  is !(int>=1)
          println("\tsubb #<1");
          println("\tsbca #>1");
          println("\tjge %s",if_false);
          return true;
        }
        break;
      }
    }
    // ↑ if (expr op 0) 
    // ↓ if (expr op const/var)
  } else if (is_integer_constant(rhs,&val)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      println("\tcpx #%ld",val);
      switch(node->kind) {
      case ND_EQ:
        println("\tjne %s", if_false);
        return true;
      case ND_NE:
        println("\tjeq %s", if_false);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (is_integer_constant(rhs,&val)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_addr_x(lhs))) {
      int off = gen_addr_x(lhs,false);
      ldx_nX(off);
      println("\tcpx #%ld",val);
      switch(node->kind) {
      case ND_EQ:
        println("\tjne %s", if_false);
        return true;
      case ND_NE:
        println("\tjeq %s", if_false);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (is_global_var_with_cast(rhs)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      cpx_EXT(skip_redundant_ptr_cast(rhs));
      switch(node->kind) {
      case ND_EQ:
        println("\tjne %s", if_false);
        return true;
      case ND_NE:
        println("\tjeq %s", if_false);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
    // ↑ rhs==const/var && EQ or NE
  } else if ((addr=is_addr_constant(rhs))    // rhs == addr const
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      println("\tcpx #_%s",addr);
      switch(node->kind) {
      case ND_EQ:
        println("\tjne %s", if_false);
        return true;
      case ND_NE:
        println("\tjeq %s", if_false);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (can_direct(rhs)) {
    gen_expr(lhs);
    if (!gen_direct(rhs, "subb", "sbca")) {
      assert(0);
    }
  } else if (0 && opt('O','s') && is_compare(node)) {
    gen_expr(node);
    println("\tjeq  %s", if_false);
    return true;
  } else {
    gen_expr(rhs);
    push();
    gen_expr(lhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\tsubb 1,x");
    println("\tsbca 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
  }
  switch (node->kind) {
  // aba/adca #0/jne	5bytes, 8cycle
  // tstb/jne/tsta/jne	6bytes, 6 or 12cycle
  // aba/jne/jcs	5bytes, 6 or 10cycle
  // jxx may be converted to bxx/jmp, fewer jxx instructions are preferable.
  case ND_EQ:
    println("\taba");
    println("\tadca #0");
    println("\tjne %s", if_false);
    break;
  case ND_NE:
    println("\taba");
    println("\tadca #0");
    println("\tjeq %s", if_false);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned) {
      println("\tjcc %s", if_false);
    } else {
      println("\tjge %s", if_false);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_false);
    } else {
      println("\tjlt %s", if_false);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned) {
      println("\tjhi %s", if_false);
      println("\tbcs %s", if_thru);
      println("\ttstb");
      println("\tjne %s", if_false);
      println("%s:", if_thru);
    } else {
      println("\tjgt %s", if_false);
      println("\tblt %s", if_thru);
      println("\ttstb");
      println("\tjne %s", if_false);
      println("%s:", if_thru);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_false);
      println("\tbhi %s", if_thru);
      println("\ttstb");
      println("\tjeq %s", if_false);
      println("%s:", if_thru);
    } else {
      println("\tjlt %s", if_false);
      println("\tbgt %s", if_thru);
      println("\ttstb");
      println("\tjeq %s", if_false);
      println("%s:", if_thru);
    }
    break;
  }
  return true;
fallback:
  gen_expr(node);
  cmp_zero(node->ty);
  println("\tjeq %s", if_false);
  return true;
}

//
// Compare two 8 signed/unsigned integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return false.
//
static bool gen_jump_if_true_8bit(Node *node, char *if_true)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int64_t val;

  if (node->kind == ND_BITAND
  &&  is_integer_constant(node->rhs, &val)) {
    if ((val & ~0xFF)==0
    ||  (is_int8(node->lhs->ty) && node->lhs->ty->is_unsigned)) {
      gen_expr(lhs);
      println("\tandb #<%ld", val);
      println("\tjne %s", if_true);
      return true;
    }
  }

  if (!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjne %s", if_true);
    return true;
  }

  if ( !is_int8(lhs->ty) || !is_int8(rhs->ty) ){
    return false;
  }

  if (can_direct_char(rhs)) {
    if (is_integer_constant(rhs, &val)) {
      gen_expr(lhs);
      if (val == 0) {
        if (node->kind != ND_EQ && node->kind != ND_NE) {
          println("\ttstb");
        }
      }else{
        println("\tcmpb #%ld", rhs->val);
      }
    } else {
      gen_expr(lhs);
      gen_direct_char(rhs, "cmpb", NULL);
    }
  }else{
    gen_expr(lhs);
    push1();
    gen_expr(rhs);
    popa();
    println("\tcba");
  }

  switch (node->kind) {
  case ND_EQ:
    println("\tjeq %s", if_true);
    break;
  case ND_NE:
    println("\tjne %s", if_true);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_true);
    } else {
      println("\tjlt %s", if_true);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned) {
      println("\tjcc %s", if_true);
    } else {
      println("\tjge %s", if_true);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned) {
      println("\tjls %s", if_true);
    } else {
      println("\tjle %s", if_true);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned) {
      println("\tjhi %s", if_true);
    } else {
      println("\tjgt %s", if_true);
    }
    break;
  }
  return true;
}

//
// Compare two 8- or 16-bit integers.
//   Generate code that branches to if_true if the comparison result is true.
//   Return 1 if code was generated.
// For other types, generate no code and return false.
//
bool gen_jump_if_true(Node *node, char *if_true)
{
  int64_t val;

  node = optimize_expr(node);
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  char *addr;
  char *if_thru = new_label("L_thru_%d");

  if (is_integer_constant(node,&val)) {
    if (val) {
      println("\tjmp %s",if_true);
    }
    return true;
  }
  if (node->kind == ND_NOT) {
    return (gen_jump_if_false(lhs, if_true));
  }

  if (isVAR(node) && is_byte(node)) {
    if (gen_jump_if_true_8bit(node, if_true)) {
      return true;
    }
    goto fallback;
  }

  if (is_int16_or_ptr(node->ty)) {
    if (test_expr_x(node)) {
      gen_expr_x(node,false);
      println("\tjne %s", if_true);
      return true;
    }
    if (test_addr_x(node)) {
      int off = gen_addr_x(node,false);
      ldx_nX(off);
      println("\tjne %s", if_true);
      return true;
    }
  }

  if (node->kind == ND_LOGOR) {
    gen_jump_if_true(lhs, if_true);
    gen_jump_if_true(rhs, if_true);
    return true;
  }
  if (node->kind == ND_LOGAND) {
    gen_jump_if_false(lhs, if_thru);
    gen_jump_if_true(rhs, if_true);
    println("%s:", if_thru);
    IX_Dest = IX_None;
    return true;
  }

  if (node->kind == ND_BITAND && is_int8(node->ty)) {
    gen_jump_if_true_8bit(node,if_true);
    return true;
  }

  if (!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjne %s",if_true);
    return true;
  }

  // If one side is ND_NUM, both sides are promoted to int,
  // so this can't be optimized. TODO: Fix optimize.c
  if (is_byte(lhs) && is_byte(rhs)) {
    if (gen_jump_if_true_8bit(node, if_true)) {
      return true;
    }
  }

  // special long case
  if (lhs->ty->kind == TY_LONG
  && is_integer_constant(rhs,&val)
  && val == 0
  && test_addr_x(lhs)) {
    switch(node->kind) {
    case ND_GE:
      if (lhs->ty->is_unsigned) {
        println("; ulong >= 0 is always false");
        println("\tjmp %s",if_true);
      }else{
        int off = gen_addr_x(lhs,false);
        println("\tldab %d,x",off);
        println("\tjpl %s", if_true);
      }
      return true;
    case ND_LT:
      if (lhs->ty->is_unsigned) {
        println("; ulong < 0 is always false");
      }else{
        int off = gen_addr_x(lhs,false);
        println("\ttst %d,x",off);
        println("\tjmi %s", if_true);
      }
      return true;
    }
    goto fallback;
  }

  if (!is_int16_or_ptr(lhs->ty)) {
    goto fallback;
  }
  if (!is_int16_or_ptr(rhs->ty)) {
    goto fallback;
  }

 // if (expr op 0)
  if (is_integer_constant(rhs,&val) && val==0) {
    if (test_expr_x(lhs)) {
      gen_expr_x(lhs,false); // Evaluate LHS anyway; no side-effect check yet.
      println("\tcpx #0");
      switch (node->kind) {
      case ND_EQ:
        println("\tjeq %s", if_true);
        return true;
      case ND_NE:
        println("\tjne %s", if_true);
        return true;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          return true;
        }
        println("\tjmi %s", if_true);
        return true;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          println("\tjne %s", if_true);
          return true;
        } else {
          println("; int>0 is !(int<0) && !(int==0)");
          println("\tjeq %s", if_thru);
          println("\tjpl %s", if_true);
          println("%s:",if_thru);
          return true;
        }
        break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          println("\tjmp %s",if_true);
          return true;
        } else {
          println("\tjpl %s", if_true);
          return true;
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          println("\tjeq %s", if_true);
          return true;
        } else {
          println("; int<=0 is !(int==0 || int<0)");
          println("\tjmi %s", if_true);
          println("\tjeq %s", if_true);
          return true;
        }
        break;
      }
    } else {
      gen_expr(lhs);  // Evaluate LHS anyway; no side-effect check yet.
      switch (node->kind) {
      case ND_EQ:
      case ND_NE:
        break;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          return true;
        }
        // 'subb #0 / sbca #0' can be substituted with 'tsta'.
        println("\ttsta");
        break;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          node->kind = ND_NE;
        } else {
          println("\ttsta");
        }
        break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          println("\tjmp %s",if_true);
          return true;
        } else {
          println("\ttsta");
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          node->kind = ND_EQ;
        } else {  // int<=0  is !(int>=1)
          println("\tsubb #<1");
          println("\tsbca #>1");
          println("\tjlt %s",if_true);
          return true;
        }
        break;
      }
    }
    // ↑ if (expr op 0) 
    // ↓ if (expr op const)
  } else if (is_integer_constant(rhs,&val)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      println("\tcpx #%ld",val);
      switch(node->kind) {
      case ND_EQ:
        println("\tjeq %s", if_true);
        return true;
      case ND_NE:
        println("\tjne %s", if_true);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (is_integer_constant(rhs,&val)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_addr_x(lhs))) {
      int off = gen_addr_x(lhs,false);
      ldx_nX(off);
      println("\tcpx #%ld",val);
      switch(node->kind) {
      case ND_EQ:
        println("\tjeq %s", if_true);
        return true;
      case ND_NE:
        println("\tjne %s", if_true);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (is_global_var_with_cast(rhs)
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      cpx_EXT(skip_redundant_ptr_cast(rhs));
      switch(node->kind) {
      case ND_EQ:
        println("\tjeq %s", if_true);
        return true;
      case ND_NE:
        println("\tjne %s", if_true);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
    // ↑ rhs==const && EQ or NE
  } else if ((addr=is_addr_constant(rhs))    // rhs == addr const
         && (node->kind==ND_EQ || node->kind==ND_NE)
         && (test_expr_x(lhs))) {
      int off = gen_expr_x(lhs,false);
      println("\tcpx #_%s",addr);
      switch(node->kind) {
      case ND_EQ:
        println("\tjeq %s", if_true);
        return true;
      case ND_NE:
        println("\tjne %s", if_true);
        return true;
      default: ;
        assert(0);    // It's strange to fail
      }
  } else if (can_direct(rhs)) {
    gen_expr(lhs);  // Evaluate LHS anyway; no side-effect check yet.
    if (rhs->kind == ND_NUM && rhs->val == 0) {
      if (node->kind != ND_EQ && node->kind != ND_NE) {
        println("\ttsta");
      }
    } else if (!gen_direct(rhs, "subb", "sbca")) {
      assert(0);
    }
  } else if (0 && opt('O','s') && is_compare(node)) {
    gen_expr(node);
    println("\tjne  %s", if_true);
    return true;
  } else {
    gen_expr(rhs);
    push();
    gen_expr(lhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\tsubb 1,x");
    println("\tsbca 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
  }
  switch (node->kind) {
  case ND_EQ:
    println("\taba");
    println("\tadca #0");
    println("\tjeq %s", if_true);
    break;
  case ND_NE:
    println("\taba");
    println("\tadca #0");
    println("\tjne %s", if_true);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_true);
    } else {
      println("\tjlt %s", if_true);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned) {
      println("\tjcc %s", if_true);
    } else {
      println("\tjge %s", if_true);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned) {
      println("\tjcs %s", if_true);
      println("\tjhi %s", if_thru);
      println("\ttstb");
      println("\tjeq %s", if_true);
      println("%s:", if_thru);
    } else {
      println("\tjlt %s", if_true);
      println("\tbgt %s", if_thru);
      println("\ttstb");
      println("\tjeq %s", if_true);
      println("%s:", if_thru);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned) {
      println("\tjhi %s", if_true);
      println("\tbcs %s", if_thru);
      println("\ttstb");
      println("\tjne %s", if_true);
      println("%s:", if_thru);
    } else {
      println("\tjgt %s", if_true);
      println("\tblt %s", if_thru);
      println("\ttstb");
      println("\tjne %s", if_true);
      println("%s:", if_thru);
    }
    break;
  }
  return true;
fallback:
  gen_expr(node);
  cmp_zero(node->ty);
  println("\tjne %s", if_true);
  return true;
}
