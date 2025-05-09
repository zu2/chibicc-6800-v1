#include "chibicc.h"


bool is_compare(Node *node)
{
    switch(node->kind){
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
    switch(node->kind){
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
    switch(node->kind){
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

static int isNUM(Node *node)
{
  return node->kind==ND_NUM;
}
static int isVAR(Node *node)
{
  return node->kind==ND_VAR;
}
static int isVARorNUM(Node *node)
{
  return isVAR(node) || isNUM(node);
}

static int check_in_char(Node *node)
{

  if (node->kind==ND_NUM) {
    if (node->ty->is_unsigned) {
      if (node->val > 255) {
	return 0;
      }
    }else{ // signed
      if (node->val<128 || node->val > 127) {
	return 0;
      }
    }
  }
  return 1;
}


//
// Compare two 8 signed/unsigned integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return 0.
//
static int gen_jump_if_false_8bit(Node *node,char *if_false)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (isVAR(node)) {
    if (node->ty->kind != TY_CHAR)
      return 0;				// what's happen?
    load_var(node);
//  This tstb is unnecessary in the current codegen.c:load_var(),
//  but not using it may cause risky behavior in the future.
//  println("\tstb");
    println("\tjeq %s",if_false);
    return 1;
  }

  if (!is_compare(node))
    return 0;

  fprintf(stderr,"; gen_jump_if_false_8bit check TY_CHAR\n");
  if (lhs->ty->kind!=TY_CHAR || rhs->ty->kind!=TY_CHAR)
    return 0;
  fprintf(stderr,"; gen_jump_if_false_8bit check unsigned\n");
  if (lhs->ty->is_unsigned != rhs->ty->is_unsigned)
    return 0;
  fprintf(stderr,"; gen_jump_if_false_8bit check VAR or NUM\n");
  if (!isVARorNUM(lhs) || !isVARorNUM(rhs))
    return 0;
  if (rhs->kind==ND_NUM && !check_in_char(rhs))
    return 0;
  if (lhs->kind==ND_NUM && !check_in_char(lhs))
    return 0;

  if (!can_direct(node->rhs))
    return 0;


  if(node->rhs->kind == ND_NUM && node->rhs->val == 0){
    gen_expr(node->lhs);
    if(node->kind != ND_EQ && node->kind != ND_NE)
      println("\ttstb");
  }else if(rhs->kind==ND_NUM) {
    gen_expr(node->lhs);
    println("\tcmpb #%ld",rhs->val);
  }else if(rhs->kind==ND_VAR) {
    if (rhs->var->ty->kind == TY_VLA) return 0;
    if (rhs->ty->kind==TY_ARRAY)      return 0;
    if (rhs->var->is_local) {
      if (!test_addr_x(rhs)) return 0;
      int off = gen_addr_x(rhs,true);
      println("\tcmpb %d,x",off);
    }else{ // global
      println("\tcmpb _%s",rhs->var->name);
    }
  }else{
    assert(0);
  }

  switch(node->kind){
  case ND_EQ:
    println("\tjne %s",if_false);
    break;
  case ND_NE:
    println("\tjeq %s",if_false);
    break;
  case ND_LT:
    if (node->lhs->ty->is_unsigned){
      println("\tjcc %s",if_false);
    }else{
      println("\tjge %s",if_false);
    }
    break;
  case ND_GE:
    if (node->lhs->ty->is_unsigned){
      println("\tjcs %s",if_false);
    }else{
      println("\tjlt %s",if_false);
    }
    break;
  case ND_LE:
    if (node->lhs->ty->is_unsigned){
      println("\tjhi %s",if_false);
    }else{
      println("\tjgt %s",if_false);
    }
    break;
  case ND_GT:
    if (node->lhs->ty->is_unsigned){
      println("\tjls %s",if_false);
    }else{
      println("\tjle %s",if_false);
    }
    break;
  }
  return 1;
}

//
// Compare two 8- or 16-bit integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return 0.
//
int gen_jump_if_false(Node *node,char *if_false)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (node->kind==ND_NOT) {
    node->lhs->bool_result_unused = true;
    return gen_jump_if_true(node->lhs,if_false);
  }

  if (isVAR(node) && node->ty->kind==TY_CHAR)
    return gen_jump_if_false_8bit(node,if_false);

  if (isVAR(node) && node->ty->kind==TY_INT) {
    if (is_global_var(node)) {
      println("\tldx _%s",node->var->name);
      println("\tjeq %s",if_false);
      IX_Dest = IX_None;
      return 1;
    }
    if (is_local_var(node)
    && node->var->offset<=255
    && test_addr_x(node)) {
      int off = gen_addr_x(node,false);
      println("\tldx %d,x",off);
      println("\tjeq %s",if_false);
      IX_Dest = IX_None;
      return 1;
    }
  }

  if(!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjeq %s", if_false);
    return 1;
  }

// If one side is ND_NUM, both sides are promoted to int,
// so this can't be optimized. TODO: Fix optimize.c
  if(lhs->ty->kind==TY_CHAR
  && rhs->ty->kind==TY_CHAR
  && gen_jump_if_false_8bit(node,if_false)){
    return 1;
  }

  if(lhs->ty->kind!=TY_CHAR && lhs->ty->kind!=TY_INT && lhs->ty->kind!=TY_SHORT)
    return 0;
  if(rhs->ty->kind!=TY_CHAR && rhs->ty->kind!=TY_INT && rhs->ty->kind!=TY_SHORT)
    return 0;


  char if_thru[32];
  char if_label[32];
  char if_cond[32];
  int c = count();
  sprintf(if_thru,"L_thru_%d",c);
  if(rhs->kind == ND_NUM && rhs->val == 0){
    if ((lhs->kind==ND_VAR && lhs->var->ty->kind != TY_VLA)
    && ( (lhs->var->is_local && lhs->ty->kind!=TY_ARRAY)
       ||(!lhs->var->is_local && lhs->ty->kind!=TY_FUNC))) {
      if( lhs->var->is_local && lhs->ty->kind!=TY_ARRAY) {
        int off = gen_addr_x(lhs,false);
        sprintf(if_cond,"%d,x",off);
      }else if (!lhs->var->is_local && lhs->ty->kind!=TY_FUNC) {
        sprintf(if_cond,"_%s",lhs->var->name);
      }
      switch(node->kind) {
      case ND_EQ:
        println("\tldx %s",if_cond);
        println("\tjne %s",if_false);
        IX_Dest = IX_None;
        return 1;
      case ND_NE:
        println("\tldx %s",if_cond);
        println("\tjeq %s",if_false);
        IX_Dest = IX_None;
        return 1;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
          println("\tjmp %s",if_false);
          return 1;
        }
        println("\tldaa %s",if_cond);
        println("\tjpl %s",if_false);
        return 1;
      case ND_GT:
        if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
          println("\tldx %s",if_cond);
          println("\tjeq %s",if_false);
          IX_Dest = IX_None;
          return 1;
        }else{
          println("; int>0 is !(int<0) && !(int==0)");
          println("\tldx %s",if_cond);
	  println("\tjmi %s",if_false);
	  println("\tjeq %s",if_false);
          IX_Dest = IX_None;
	  return 1;
        }
	break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
          return 1;
        }else{
          println("\tldaa %s",if_cond);
          println("\tjmi %s",if_false);
	  return 1;
        }
        break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
          println("\tldx %s",if_cond);
          println("\tjne %s",if_false);
          IX_Dest = IX_None;
          return 1;
        }else{
          println("\tldx %s",if_cond);
          println("\tjmi %s",if_false);
          println("\tjeq %s",if_false);
          IX_Dest = IX_None;
          return 1;
        } 
        break;
      }
    }else{
      gen_expr(lhs);
      switch(node->kind) {
      case ND_EQ:
      case ND_NE:
	break;
      case ND_LT:
        if (lhs->ty->is_unsigned) {
          println("; uint < 0 is always false");
	  println("\tjmp %s",if_false);
	  return 1;
	}
        // 'subb #0 / sbca #0' can be substituted with 'tsta'.
        println("\ttsta");
	break;
      case ND_GT:
	if (lhs->ty->is_unsigned) {
          println("; uint > 0 is true for any value other than zero.");
	  node->kind = ND_NE;
	}else{
          println("\ttsta");
	}
	break;
      case ND_GE:
        if (lhs->ty->is_unsigned) {
          println("; uint >= 0 is always true");
	  return 1;
	}else{
          println("\ttsta");
	}
	break;
      case ND_LE:
        if (lhs->ty->is_unsigned) {
          println("; uint<=0 only when it is exactly 0");
	  node->kind = ND_EQ;
	}else{
          println("\ttsta");
	}
	break;
      }
    }
  // ↑ rhs==0
  }else if (can_direct(rhs)) {
    gen_expr(lhs);
    if(!gen_direct(rhs,"subb","sbca")){
      assert(0);
    }
  }else{
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
  switch(node->kind){
  // aba/adca #0/jne	5bytes, 10cycle
  // tstb/jne/tsta/jne	6bytes, 6 or 12cycle
  // aba/jne/jcs	5bytes, 6 or 10cycle
  // jxx may be converted to bxx/jmp, fewer jxx instructions are preferable.
  case ND_EQ:
    sprintf(if_label,"L_thru_%d",c);
    println("\taba");
    println("\tadca #0");
    println("\tjne %s",if_false);
    break;
  case ND_NE:
    println("\taba");
    println("\tadca #0");
    println("\tjeq %s",if_false);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned){
      println("\tjcc %s",if_false);
    }else{
      println("\tjge %s",if_false);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned){
      println("\tjcs %s",if_false);
    }else{
      println("\tjlt %s",if_false);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned){
      println("\tjhi %s",if_false);
      println("\tbcs %s",if_thru);
      println("\ttstb");
      println("\tjne %s",if_false);
      println("%s:",if_thru);
    }else{
      println("\tjgt %s",if_false);
      println("\tblt %s",if_thru);
      println("\ttstb");
      println("\tjne %s",if_false);
      println("%s:",if_thru);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned){
      println("\tjcs %s",if_false);
      println("\tbhi %s",if_thru);
      println("\ttstb");
      println("\tjeq %s",if_false);
      println("%s:",if_thru);
    }else{
      println("\tjlt %s",if_false);
      println("\tbgt %s",if_thru);
      println("\ttstb");
      println("\tjeq %s",if_false);
      println("%s:",if_thru);
    }
    break;
  }
  return 1;
}

//
// Compare two 8 signed/unsigned integers.
//   Generate code that branches to if_false if the comparison result is false.
//   Return 1 if code was generated.
// For other types, generate no code and return 0.
//
static int gen_jump_if_true_8bit(Node *node,char *if_true)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (isVAR(node)) {
    if (node->ty->kind != TY_CHAR)
      return 0;				// what's happen?
    load_var(node);
//  This tstb is unnecessary in the current codegen.c:load_var(),
//  but not using it may cause risky behavior in the future.
//  println("\tstb");
    println("\tjne %s",if_true);
    return 1;
  }

  if (!is_compare(node))
    return 0;

  if (lhs->ty->kind!=TY_CHAR || rhs->ty->kind!=TY_CHAR)
    return 0;
  if (lhs->ty->is_unsigned != rhs->ty->is_unsigned)
    return 0;
  if (!isVARorNUM(lhs) || !isVARorNUM(rhs))
    return 0;
  if (rhs->kind==ND_NUM && !check_in_char(rhs))
    return 0;
  if (lhs->kind==ND_NUM && !check_in_char(lhs))
    return 0;

  if (!can_direct(node->rhs))
    return 0;


  if(node->rhs->kind == ND_NUM && node->rhs->val == 0){
    gen_expr(node->lhs);
    if(node->kind != ND_EQ && node->kind != ND_NE)
      println("\ttstb");
  }else if(rhs->kind==ND_NUM) {
    gen_expr(node->lhs);
    println("\tcmpb #%ld",rhs->val);
  }else if(rhs->kind==ND_VAR) {
    if (rhs->var->ty->kind == TY_VLA) return 0;
    if (rhs->ty->kind==TY_ARRAY)      return 0;
    if (rhs->var->is_local) {
      if (!test_addr_x(rhs)) return 0;
      int off = gen_addr_x(rhs,true);
      println("\tcmpb %d,x",off);
    }else{ // global
      println("\tcmpb _%s",rhs->var->name);
    }
  }else{
    assert(0);
  }

  switch(node->kind){
  case ND_EQ:
    println("\tjeq %s",if_true);
    break;
  case ND_NE:
    println("\tjne %s",if_true);
    break;
  case ND_LT:
    if (node->lhs->ty->is_unsigned){
      println("\tjcs %s",if_true);
    }else{
      println("\tjlt %s",if_true);
    }
    break;
  case ND_GE:
    if (node->lhs->ty->is_unsigned){
      println("\tjcc %s",if_true);
    }else{
      println("\tjge %s",if_true);
    }
    break;
  case ND_LE:
    if (node->lhs->ty->is_unsigned){
      println("\tjls %s",if_true);
    }else{
      println("\tjle %s",if_true);
    }
    break;
  case ND_GT:
    if (node->lhs->ty->is_unsigned){
      println("\tjhi %s",if_true);
    }else{
      println("\tjgt %s",if_true);
    }
    break;
  }
  return 1;
}

//
// Compare two 8- or 16-bit integers.
//   Generate code that branches to if_true if the comparison result is true.
//   Return 1 if code was generated.
// For other types, generate no code and return 0.
//
int gen_jump_if_true(Node *node,char *if_true)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (node->kind==ND_NOT) {
    node->lhs->bool_result_unused = true;
    return gen_jump_if_false(node->lhs,if_true);
  }

  if (isVAR(node) && node->ty->kind==TY_CHAR)
    return gen_jump_if_true_8bit(node,if_true);

  if(!is_compare(node)) {
    gen_expr(node);
    cmp_zero(node->ty);
    println("\tjne %s", if_true);
    return 1;
  }

  if(lhs->ty->kind==TY_CHAR
  && rhs->ty->kind==TY_CHAR
  && gen_jump_if_true_8bit(node,if_true)){
    return 1;
  }

  if(lhs->ty->kind!=TY_CHAR && lhs->ty->kind!=TY_INT && lhs->ty->kind!=TY_SHORT)
    return 0;

  char if_thru[32];
  int c = count();
  sprintf(if_thru,"L_thru_%d",c);
  if (can_direct(rhs)){
    gen_expr(lhs);
    if(rhs->kind == ND_NUM && rhs->val == 0){
      if(node->kind != ND_EQ && node->kind != ND_NE)
        println("\ttsta");
    }else if(!gen_direct(rhs,"subb","sbca")){
      assert(0);
    }
  }else{
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
  switch(node->kind){
  case ND_EQ:
    println("\taba");
    println("\tadca #0");
    println("\tjeq %s",if_true);
    break;
  case ND_NE:
    println("\taba");
    println("\tadca #0");
    println("\tjne %s",if_true);
    break;
  case ND_LT:
    if (lhs->ty->is_unsigned){
      println("\tjcs %s",if_true);
    }else{
      println("\tjlt %s",if_true);
    }
    break;
  case ND_GE:
    if (lhs->ty->is_unsigned){
      println("\tjcc %s",if_true);
    }else{
      println("\tjge %s",if_true);
    }
    break;
  case ND_LE:
    if (lhs->ty->is_unsigned){
      println("\tjcs %s",if_true);
      println("\tjhi %s",if_thru);
      println("\ttstb");
      println("\tjeq %s",if_true);
      println("%s:",if_thru);
    }else{
      println("\tjlt %s",if_true);
      println("\tbgt %s",if_thru);
      println("\ttstb");
      println("\tjeq %s",if_true);
      println("%s:",if_thru);
    }
    break;
  case ND_GT:
    if (lhs->ty->is_unsigned){
      println("\tjhi %s",if_true);
      println("\tbcs %s",if_thru);
      println("\ttstb");
      println("\tjne %s",if_true);
      println("%s:",if_thru);
    }else{
      println("\tjgt %s",if_true);
      println("\tblt %s",if_thru);
      println("\ttstb");
      println("\tjne %s",if_true);
      println("%s:",if_thru);
    }
    break;
  }
  return 1;
}
