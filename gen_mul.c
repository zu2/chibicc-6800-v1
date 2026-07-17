#include "chibicc.h"


bool is_byte_unum(Node *node)
{
  int64_t val;

  if (node->kind == ND_NUM && is_integer_constant(node, &val)) {
    return (val >= 0 && val <= 255);
  }

  return false;
}

bool is_uchar_or_unum(Node *node)
{
  if (node->ty->kind == TY_CHAR && node->ty->is_unsigned)
    return true;
  if (is_byte_unum(node))
    return true;

  return false;
}

//
// int = unsigned char * unsigned char
//
bool
gen_mul8u(Node *node)
{
  Node *lhs = skip_integral_promotion(node->lhs);
  Node *rhs = skip_integral_promotion(node->rhs);
  int64_t val;

  if (is_uchar_or_unum(lhs)
  &&  is_uchar_or_unum(rhs)) {
    if (is_integer_constant(rhs,&val)) {
      switch(val){
      case 0:
        gen_expr(lhs);  // side effect?
        println("\tclrb");
        println("\tclra");
        return true;
      case 1:
        gen_expr(lhs);
        println("\tclra");
        return true;
      case 2:
      case 4:
      case 8:
      case 16:
        gen_expr(lhs);
        println("\tclra");
        for(int i=2; i<=val; i*=2) {
          println("\taslb");
          println("\trola");
        }
        return true;
      }
    }
    if (can_direct_char(node->rhs)) {
      gen_expr(lhs);
      gen_direct_char(rhs,"ldaa",NULL);
    }else if (test_addr_x(rhs)) {
      gen_expr(lhs);
      int off = gen_addr_x(rhs,false);
      println("\tldaa %d,x",off);
    }else{
      gen_expr(lhs);
      push1();
      gen_expr(rhs);
      popa();
    }
    println("\tjsr __mul8x8u");
    IX_Dest = IX_None;
    return true;
  }
  return false;
}

bool is_byte_snum(Node *node)
{
  int64_t val;

  if (node->kind == ND_NUM
  &&  is_integer_constant(node, &val)
  &&  !node->ty->is_unsigned) {
    return (val >= -128 && val <= 127);
  }

  return false;
}

bool is_schar_or_snum(Node *node)
{
  if (node->ty->kind == TY_CHAR && !node->ty->is_unsigned)
    return true;
  if (is_byte_unum(node))
    return true;

  return false;
}

bool
gen_mul8s(Node *node)
{
  Node *lhs = skip_integral_promotion(node->lhs);
  Node *rhs = skip_integral_promotion(node->rhs);
  int64_t val;

  if (is_schar_or_snum(lhs)
  &&  is_schar_or_snum(rhs)) {
    if (is_integer_constant(rhs,&val)) {
      switch(val){
      case 0:
        gen_expr(lhs);  // side effect?
        println("\tclrb");
        println("\tclra");
        return true;
      case 1:
        gen_expr(lhs);
        println("\tclra");
        println("\tasrb");
        println("\trolb");
        println("\tsbca #0");
        return true;
      case 2:
      case 4:
      case 8:
      case 16:
        gen_expr(lhs);
        println("\tclra");
        println("\tasrb");
        println("\trolb");
        println("\tsbca #0");
        for(int i=2; i<=val; i*=2) {
          println("\taslb");
          println("\trola");
        }
        return true;
      }
    }
    if (can_direct_char(node->rhs)) {
      gen_expr(lhs);
      gen_direct_char(rhs,"ldaa",NULL);
    }else if (test_addr_x(rhs)) {
      gen_expr(lhs);
      int off = gen_addr_x(rhs,false);
      println("\tldaa %d,x",off);
    }else{
      gen_expr(lhs);
      push1();
      gen_expr(rhs);
      popa();
    }
    println("\tjsr __mul8x8s");
    IX_Dest = IX_None;
    return true;
  }
  return false;
}

//
// jsr mul16x16 uses 10bytes; unrolled if smaller, even for Os
//
//    pshb          // 1 4
//    psha          // 1 4
//    ldab #xx      // 2 2
//    clra          // 1 1
//    jsr mul16x16  // 3 9
//    ins           // 1 4
//    ins           // 1 4
//
bool
gen_mul16(Node *node)
{
  int off;
  bool addr_x = false; 
  bool global = false; 
  char ta[64], tb[64];
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (is_global_var(lhs)) {
    global = true;
    sprintf(tb,"_%s+1",lhs->var->name);
    sprintf(ta,"_%s",  lhs->var->name);
    println("\tldab %s",tb);
    println("\tldaa %s",ta);
  } else if ((addr_x = test_addr_x(lhs))) {
    off = gen_addr_x(lhs,false);
    sprintf(tb,"%d,x",off+1);
    sprintf(ta,"%d,x",off);
    println("\tldab %s",tb);
    println("\tldaa %s",ta);
  }else{
    gen_expr(lhs);
  }
  switch(rhs->kind){
  case ND_NUM:
    switch (rhs->ty->kind) {
    case TY_INT:
    case TY_SHORT:
    case TY_ENUM:
      switch(rhs->val){
      case -4:
        println("\taslb");
        println("\trola");
        // thru
      case -2:
        println("\taslb");
        println("\trola");
        // thru
      case -1:
        negd();
        return true;
      case 0:
        println("\tclrb");
        println("\tclra");
        return true;
      case 1:
        return true;
      case 2:
        println("\taslb");
        println("\trola");
        return true;
      case 3:
        if (global || addr_x) {
          println("\taslb");
          println("\trola");
          println("\taddb %s",tb);
          println("\tadca %s",ta);
          return true;
        }
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        return true;
      case 4:
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        return true;
      case 5:
        if (global || addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taddb %s",tb);
          println("\tadca %s",ta);
          return true;
        }
        if (opt('O','s'))
          break;
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        return true;
      case 6:
        if (global || addr_x) {
          println("\taslb");
          println("\trola");
          println("\taddb %s",tb);
          println("\tadca %s",ta);
          println("\taslb");
          println("\trola");
          return true;
        }
        if (opt('O','s'))
          break;
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        println("\taslb");
        println("\trola");
        return true;
      case 7:
        if (global || addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\tsubb %s",tb);
          println("\tsbca %s",ta);
          return true;
        }
        if (opt('O','s'))
          break;
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\tsubb @tmp1+1");
        println("\tsbca @tmp1");
        return true;
      case 8:
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        return true;
      case 10:
        if (global || addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taddb %s",tb);
          println("\tadca %s",ta);
          println("\taslb");
          println("\trola");
          return true;
        }
        if (opt('O','s'))
          break;
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        println("\taslb");
        println("\trola");
        return true;
      case 16:
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        return true;
      case 32:
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        return true;
      case 64:
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        return true;
      case 100:
        println("\tjsr __mul100");
        return true;
      }
    }
    if (rhs->val>0 && rhs->val<256) {
      println("\tldx #%ld",rhs->val);
      println("\tjsr __mul16x8x");
      IX_Dest = IX_None;
      return true;
    }
  }
  push();
  gen_expr(rhs);
  println("\tjsr __mul16x16");
  IX_Dest = IX_None;
  ins(2);
  return true;
}

