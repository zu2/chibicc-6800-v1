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
    return 1;
  }
  return 0;
}

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
    return 1;
  }
  return 0;
}

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
    return 1;
  }
  return 0;
}

Type *is_byte(Node *node)
{
  int64_t val;

  if (node->kind == ND_CAST && node->ty->kind == TY_INT &&
      !node->ty->is_unsigned) { // integral promotion ?
    node = node->lhs;
  }
  if (node->ty->kind == TY_CHAR || node->ty->kind == TY_BOOL) {
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

static int isNUM(Node *node) { return node->kind == ND_NUM; }
static int isVAR(Node *node) { return node->kind == ND_VAR; }
static int isVARorNUM(Node *node) { return isVAR(node) || isNUM(node); }

static bool check_in_char(Node *node)
{
  int64_t val;

  if (is_integer_constant(node, &val)) {
    if ((val & ~0xff) == 0) {
      return true;
    }
  }
  return false;
}

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

  if (isVAR(node)) {
    if (!is_byte(node)) {
      return false;
    }
    //  This tstb is unnecessary in the current codegen.c:load_var(),
    //  but not using it may cause risky behavior in the future.
    load_var(node);
    //  println("\tstb");
    println("\tjeq %s", if_false);
    return true;
  }

  if (node->kind == ND_LOGOR) {
    char if_thru[32];
    sprintf(if_thru, "L_thru_%d", count());
    gen_jump_if_true(lhs, if_thru);
    gen_jump_if_false(rhs, if_false);
    println("%s:", if_thru);
    return true;
  }
  if (node->kind == ND_LOGAND) {
    gen_jump_if_false(lhs, if_false);
    gen_jump_if_false(rhs, if_false);
    return true;
  }

  if (node->kind == ND_BITAND && check_in_char(rhs)) {
    println("\tandb #$%02lx", rhs->val);
    println("\tjeq %s", if_false);
    return true;
  }

  if (!is_compare(node)) {
    return false;
  }

  Type *lty;
  Type *rty;
  if (!(lty = is_byte(lhs))) {
    return false;
  }
  if (!(rty = is_byte(rhs))) {
    return false;
  }
  if (lty->is_unsigned != rty->is_unsigned) {
    return false;
  }

  if (lhs->kind == ND_CAST && lhs->ty->kind == TY_INT) {
    lhs = lhs->lhs;
  }
  if (rhs->kind == ND_CAST && rhs->ty->kind == TY_INT) {
    rhs = rhs->lhs;
  }
  if (can_direct(rhs)) {
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
      gen_direct(rhs, "cmpb", NULL);
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
  return 1;
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
  char if_thru[32];
  char if_cond[32];
  sprintf(if_thru, "L_thru_%d", count());

  if (node->kind == ND_NOT) {
    return gen_jump_if_true(node->lhs, if_false);
  }

  if (isVAR(node) && is_integer_or_ptr(node->ty) && node->ty->size == 2) {
    if (is_global_var(node)) {
      println("\tldx _%s", node->var->name);
      println("\tjeq %s", if_false);
      IX_Dest = IX_None;
      return 1;
    }
    if (is_local_var(node) && node->var->offset <= 255 && test_addr_x(node)) {
      int off = gen_addr_x(node, false);
      println("\tldx %d,x", off);
      println("\tjeq %s", if_false);
      IX_Dest = IX_None;
      return 1;
    }
  }

  if (node->kind == ND_LOGOR) {
    gen_jump_if_true(node->lhs, if_thru);
    gen_jump_if_false(node->rhs, if_false);
    println("%s:", if_thru);
    return true;
  }
  if (node->kind == ND_LOGAND) {
    gen_jump_if_false(node->lhs, if_false);
    gen_jump_if_false(node->rhs, if_false);
    return true;
  }

  if (!is_compare(node) || (node->ty->kind != TY_INT)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjeq %s", if_false);
    return 1;
  }

  // If one side is ND_NUM, both sides are promoted to int,
  // so this can't be optimized. TODO: Fix optimize.c
  if (is_byte(node->lhs) && is_byte(node->rhs)) {
    if (gen_jump_if_false_8bit(node, if_false)) {
      return 1;
    }
  }

  // special long case
  if (lhs->ty->kind == TY_LONG
  && is_integer_constant(rhs,&val)
  && val == 0) {
    if (!test_addr_x(lhs)) {
      goto fallback;
    }
    switch(node->kind) {
    case ND_LT:
      if (lhs->ty->is_unsigned) {
        println("; ulong < 0 is always false");
        println("\tjmp %s",if_false);
      }else{
        int off = gen_addr_x(lhs,false);
        println("\ttst %d,x",off);
        println("\tjpl %s", if_false);
      }
      return 1;
    }
    goto fallback;
  }

  if (lhs->ty->kind != TY_CHAR && lhs->ty->kind != TY_INT &&
      lhs->ty->kind != TY_SHORT) {
    goto fallback;
  }
  if (rhs->ty->kind != TY_CHAR && rhs->ty->kind != TY_INT &&
      rhs->ty->kind != TY_SHORT) {
    goto fallback;
  }

  if (rhs->kind == ND_NUM && rhs->val == 0) {
    if ((lhs->kind == ND_VAR && lhs->var->ty->kind != TY_VLA) &&
        ((lhs->var->is_local && lhs->ty->kind != TY_ARRAY) ||
         (!lhs->var->is_local && lhs->ty->kind != TY_FUNC))) {
      if (lhs->var->is_local && lhs->ty->kind != TY_ARRAY) {
        if (test_addr_x(lhs)) {
          int off = gen_addr_x(lhs, false);
          sprintf(if_cond, "%d,x", off);
        } else {
          // It's strange to fail test_addr_x, but it may pass in debugging.
          goto fallback;
        }
      } else if (!lhs->var->is_local && lhs->ty->kind != TY_FUNC) {
        sprintf(if_cond, "_%s", lhs->var->name);
      }
      switch (node->kind) {
      case ND_EQ:
        println("\tldx %s", if_cond);
        println("\tjne %s", if_false);
        IX_Dest = IX_None;
        return 1;
      case ND_NE:
        println("\tldx %s", if_cond);
        println("\tjeq %s", if_false);
        IX_Dest = IX_None;
        return 1;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          println("\tjmp %s", if_false);
          return 1;
        }
        println("\tldaa %s", if_cond);
        println("\tjpl %s", if_false);
        return 1;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          println("\tldx %s", if_cond);
          println("\tjeq %s", if_false);
          IX_Dest = IX_None;
          return 1;
        } else {
          println("; int>0 is !(int<0) && !(int==0)");
          println("\tldx %s", if_cond);
          println("\tjmi %s", if_false);
          println("\tjeq %s", if_false);
          IX_Dest = IX_None;
          return 1;
        }
        break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          return 1;
        } else {
          println("\tldaa %s", if_cond);
          println("\tjmi %s", if_false);
          return 1;
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          println("\tldx %s", if_cond);
          println("\tjne %s", if_false);
          IX_Dest = IX_None;
          return 1;
        } else {
          println("\tldx %s", if_cond);
          println("\tjmi %s", if_false);
          println("\tjeq %s", if_false);
          IX_Dest = IX_None;
          return 1;
        }
        break;
      }
    } else {
      gen_expr(lhs);
      switch (node->kind) {
      case ND_EQ:
      case ND_NE:
        break;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          println("\tjmp %s", if_false);
          return 1;
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
          return 1;
        } else {
          println("\ttsta");
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          node->kind = ND_EQ;
        } else {
          println("\ttsta");
        }
        break;
      }
    }
    // ↑ rhs==0
  } else if (rhs->kind == ND_NUM
         &&  (node->kind==ND_EQ || node->kind==ND_NE)
         &&  lhs->kind == ND_VAR && test_addr_x(lhs)) {
      int off = gen_addr_x(lhs, false);
      sprintf(if_cond, "%d,x", off);
      switch (node->kind) {
      case ND_EQ:
        println("\tldx %s", if_cond);
        switch(rhs->val){
        case 0:
          break;
        case 1:
          println("\tdex");
          break;
        case 0xffff:
          println("\tinx");
          break;
        default:
          println("\tcpx #%ld",rhs->val);
          break;
        }
        println("\tjne %s", if_false);
        IX_Dest = IX_None;
        return 1;
      case ND_NE:
        println("\tldx %s", if_cond);
        switch(rhs->val){
        case 0:
          break;
        case 1:
          println("\tdex");
          break;
        case 0xffff:
          println("\tinx");
          break;
        default:
          println("\tcpx #%ld",rhs->val);
          break;
        }
        println("\tjeq %s", if_false);
        IX_Dest = IX_None;
        return 1;
      default: ;
        goto fallback;  // It's strange to fail
      }
    // ↑ rhs==const && EQ or NE
  } else if (can_direct(rhs)) {
    gen_expr(lhs);
    if (!gen_direct(rhs, "subb", "sbca")) {
      assert(0);
    }
  } else if (0 && opt_O == 's' && is_compare(node)) {
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
  // aba/adca #0/jne	5bytes, 10cycle
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

  if (isVAR(node)) {
    if (!is_byte(node)) {
      return false;
    }
    load_var(node);
    //  This tstb is unnecessary in the current codegen.c:load_var(),
    //  but not using it may cause risky behavior in the future.
    //  println("\tstb");
    println("\tjne %s", if_true);
    return 1;
  }

  if (node->kind == ND_LOGOR) {
    gen_jump_if_true(node->lhs, if_true);
    gen_jump_if_true(node->rhs, if_true);
    return true;
  }
  if (node->kind == ND_LOGAND) {
    char if_thru[32];
    sprintf(if_thru, "L_thru_%d", count());
    gen_jump_if_false(node->lhs, if_thru);
    gen_jump_if_true(node->rhs, if_true);
    println("%s:", if_thru);
    return true;
  }

  if (isVAR(node) && is_integer_or_ptr(node->ty) && node->ty->size == 2) {
    if (is_global_var(node)) {
      println("\tldx _%s", node->var->name);
      println("\tjne %s", if_true);
      IX_Dest = IX_None;
      return 1;
    }
    if (is_local_var(node) && node->var->offset <= 255 && test_addr_x(node)) {
      int off = gen_addr_x(node, false);
      println("\tldx %d,x", off);
      println("\tjne %s", if_true);
      IX_Dest = IX_None;
      return 1;
    }
  }

  if (!is_compare(node)) {
    return false;
  }

  Type *lty;
  Type *rty;
  if (!(lty = is_byte(lhs))) {
    return false;
  }
  if (!(rty = is_byte(rhs))) {
    return false;
  }
  if (lty->is_unsigned != rty->is_unsigned) {
    return false;
  }
  if (lhs->kind == ND_CAST && lhs->ty->kind == TY_INT) {
    lhs = lhs->lhs;
  }
  if (rhs->kind == ND_CAST && rhs->ty->kind == TY_INT) {
    rhs = rhs->lhs;
  } 
  if (can_direct(rhs)) {
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
      gen_direct(rhs, "cmpb", NULL);
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
    if (node->lhs->ty->is_unsigned) {
      println("\tjcs %s", if_true);
    } else {
      println("\tjlt %s", if_true);
    }
    break;
  case ND_GE:
    if (node->lhs->ty->is_unsigned) {
      println("\tjcc %s", if_true);
    } else {
      println("\tjge %s", if_true);
    }
    break;
  case ND_LE:
    if (node->lhs->ty->is_unsigned) {
      println("\tjls %s", if_true);
    } else {
      println("\tjle %s", if_true);
    }
    break;
  case ND_GT:
    if (node->lhs->ty->is_unsigned) {
      println("\tjhi %s", if_true);
    } else {
      println("\tjgt %s", if_true);
    }
    break;
  }
  return 1;
}

//
// Compare two 8- or 16-bit integers.
//   Generate code that branches to if_true if the comparison result is true.
//   Return 1 if code was generated.
// For other types, generate no code and return false.
//
bool gen_jump_if_true(Node *node, char *if_true)
{
  node = optimize_expr(node);
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  char if_false[32];
  char if_thru[32];
  int c = count();
  sprintf(if_false, "L_false_%d", c);
  sprintf(if_thru, "L_thru_%d", c);

  if (node->kind == ND_NOT) {
    return (gen_jump_if_false(node->lhs, if_true));
  }

  if (isVAR(node) && is_byte(node)) {
    if (gen_jump_if_true_8bit(node, if_true)) {
      return true;
    }
    goto fallback;
  }

  if (node->kind == ND_LOGOR) {
    gen_jump_if_true(node->lhs, if_true);
    gen_jump_if_true(node->rhs, if_true);
    return true;
  }
  if (node->kind == ND_LOGAND) {
    gen_jump_if_false(node->lhs, if_thru);
    gen_jump_if_true(node->rhs, if_true);
    println("%s:", if_thru);
    return true;
  }

  if (isVAR(node) && is_integer_or_ptr(node->ty) && node->ty->size == 2) {
    if (is_global_var(node)) {
      println("\tldx _%s", node->var->name);
      println("\tjne %s", if_true);
      IX_Dest = IX_None;
      return 1;
    }
    if (is_local_var(node) && node->var->offset <= 255 && test_addr_x(node)) {
      int off = gen_addr_x(node, false);
      println("\tldx %d,x", off);
      println("\tjne %s", if_true);
      IX_Dest = IX_None;
      return 1;
    }
  }

  if (!is_compare(node)) {
    goto fallback;
  }

  if (lhs->ty->kind == TY_CHAR && rhs->ty->kind == TY_CHAR) {
    if (gen_jump_if_true_8bit(node, if_true)) {
      return true;
    }
    goto fallback;
  }

  if (lhs->ty->kind != TY_CHAR && lhs->ty->kind != TY_INT &&
      lhs->ty->kind != TY_SHORT) {
    goto fallback;
  }

  if (can_direct(rhs)) {
    gen_expr(lhs);
    if (rhs->kind == ND_NUM && rhs->val == 0) {
      if (node->kind != ND_EQ && node->kind != ND_NE) {
        println("\ttsta");
      }
    } else if (!gen_direct(rhs, "subb", "sbca")) {
      assert(0);
    }
  } else if (0 && opt_O == 's' && is_compare(node)) {
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
  return 1;
fallback:
  gen_expr(node);
  cmp_zero(node->ty);
  println("\tjne %s", if_true);
  return 1;
}
