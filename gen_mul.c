#include "chibicc.h"

//
// int = unsigned char * unsigned char
//
bool
gen_mul8u(Node *node)
{
  Node *lhs = skip_integral_promotion(node->lhs);
  Node *rhs = skip_integral_promotion(node->rhs);
  int64_t val;

  if ((is_uchar_or_u8num(lhs) && is_uchar_or_u8num(rhs))
  ||  (is_int8(node->ty) && is_char_or_8num(lhs) && is_char_or_8num(rhs))) {
    gen_expr(lhs);
    if (is_int8(node->ty)
    &&  is_integer_constant(rhs,&val)) {
      switch(val) {
      case 0:
        println("\tclrb");
        return true;
      case 1:
        return true;
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
        int n = exact_log2(val);
        for(int i=0; i<n; i++)
          println("\taslb");
        return true;
      case 3:
        println("\ttba");
        println("\taba");
        println("\taba");
        println("\ttab");
        return true;
      case 5:
        println("\ttba");
        println("\tasla");
        println("\tasla");
        println("\taba");
        println("\ttab");
        return true;
      case 6:
        println("\ttba");
        println("\tasla");
        println("\taba");
        println("\tasla");
        println("\ttab");
        return true;
      case 7:
        println("\ttba");
        println("\tasla");
        println("\taba");
        println("\tasla");
        println("\taba");
        println("\ttab");
        return true;
      case 9:
        println("\ttba");
        println("\tasla");
        println("\tasla");
        println("\tasla");
        println("\taba");
        println("\ttab");
        return true;
      case 10:
        println("\ttba");
        println("\tasla");
        println("\tasla");
        println("\taba");
        println("\tasla");
        println("\ttab");
        return true;
      case 12:
        println("\ttba");
        println("\tasla");
        println("\taba");
        println("\tasla");
        println("\tasla");
        println("\ttab");
        return true;
      case 64:
        println("\trorb");
        println("\trorb");
        println("\trorb");
        println("\tandb #<192");
        return true;
      case 128:
        println("\trorb");
        println("\trorb");
        println("\tandb #<128");
        return true;
      }
    }
    if (is_integer_constant(rhs,&val)) {
      switch(val){
      case 0:
        println("\tclrb");
        println("\tclra");
        return true;
      case 1:
        println("\tclra");
        return true;
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
        println("\tclra");
        int n = exact_log2(val);
        for(int i=0; i<n; i++) {
          println("\taslb");
          println("\trola");
        }
        return true;
      case 3:
        println("\tclra");
        println("\tstab @tmp1+1");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca #0");
        return true;
      case 5:
        if (opt('O','s')) break;
        println("\tclra");
        println("\tstab @tmp1+1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca #0");
        return true;
      case 7:
        if (opt('O','s')) break;
        println("\tclra");
        println("\tstab @tmp1+1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\tsubb @tmp1+1");
        println("\tsbca #0");
        return true;
      case 10:
        if (opt('O','s')) break;
        println("\tclra");
        println("\tstab @tmp1+1");
        println("\taslb");
        println("\trola");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca #0");
        println("\taslb");
        println("\trola");
        return true;
      case 100:
        println("\tclra");
        println("\tjsr __mul100");
        return true;
      case 128:
        println("\ttba");
        println("\tclrb");
        println("\tlsra");
        println("\trorb");
        return true;
      }
    }
    if (can_direct_8bit(rhs)) {
      gen_direct_8bit(rhs,"ldaa");
    }else if (test_addr_x(rhs)) {
      int off = gen_addr_x(rhs);
      println("\tldaa %d,x",off);
    }else{
      push1();
      gen_expr(rhs);
      popa();
    }
    println("\tjsr __mul8x8u");
    IX_invalidate();
    return true;
  }
  return false;
}

bool
gen_mul8s(Node *node)
{
  Node *lhs = skip_integral_promotion(node->lhs);
  Node *rhs = skip_integral_promotion(node->rhs);
  int64_t val;

  if (is_schar_or_s8num(lhs)
  &&  is_schar_or_s8num(rhs)) {
    gen_expr(lhs);
    if (is_integer_constant(rhs,&val)) {
      switch(val){
      case 0:
        println("\tclrb");
        println("\tclra");
        return true;
      case 1:
        sign_extend();
        return true;
      case 2:
      case 4:
      case 8:
      case 16:
      case 32:
      case 64:
        sign_extend();
        int n = exact_log2(val);
        for(int i=0; i<n; i++) {
          println("\taslb");
          println("\trola");
        }
        return true;
      case 3:
        if (opt('O','s')) break;
        sign_extend();
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        return true;
      case 5:
        if (opt('O','s')) break;
        sign_extend();
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
        if (opt('O','s')) break;
        sign_extend();
        println("\tstab @tmp1+1");
        println("\tstaa @tmp1");
        println("\taslb");
        println("\trola");
        println("\taddb @tmp1+1");
        println("\tadca @tmp1");
        println("\taslb");
        println("\trola");
        return true;
      }
    }
    if (can_direct_8bit(rhs)) {
      gen_direct_8bit(rhs,"ldaa");
    }else if (test_addr_x(rhs)) {
      int off = gen_addr_x(rhs);
      println("\tldaa %d,x",off);
    }else{
      push1();
      gen_expr(rhs);
      popa();
    }
    println("\tjsr __mul8x8s");
    IX_invalidate();
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
    off = gen_addr_x(lhs);
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
      IX_invalidate();
      return true;
    }
  }
  push();
  gen_expr(rhs);
  println("\tjsr __mul16x16");
  IX_invalidate();
  ins(2);
  return true;
}

