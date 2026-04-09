#include "chibicc.h"

bool
gen_mul8u(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int64_t val;

  if (node->lhs->kind == ND_CAST
  &&  node->lhs->ty->kind == TY_INT) {
    lhs = lhs->lhs;
  }
  if (node->rhs->kind == ND_CAST
  &&  node->rhs->ty->kind == TY_INT) {
    rhs = rhs->lhs;
  }
  if (lhs->ty->kind == TY_CHAR
  &&  rhs->ty->kind == TY_CHAR) {
    if (lhs->ty->is_unsigned
    &&  rhs->ty->is_unsigned) {
      if (is_integer_constant(rhs,&val)) {
        switch(val){
        case 0:
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
      if (can_direct(node->rhs)) {
        gen_expr(lhs);
        gen_direct(rhs,"ldaa",NULL);
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
  }
  return false;
}

bool
gen_mul8s(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (node->lhs->kind == ND_CAST
  &&  node->lhs->ty->kind == TY_INT
  && !node->lhs->ty->is_unsigned) {
    lhs = lhs->lhs;
  }
  if (node->rhs->kind == ND_CAST
  &&  node->rhs->ty->kind == TY_INT
  && !node->rhs->ty->is_unsigned) {
    rhs = rhs->lhs;
  }
  if (lhs->ty->kind == TY_CHAR
  &&  rhs->ty->kind == TY_CHAR) {
    if (!lhs->ty->is_unsigned
    &&  !rhs->ty->is_unsigned) {
      if (can_direct(node->rhs)) {
        gen_expr(lhs);
        gen_direct(rhs,"ldaa",NULL);
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
  bool addr_x;

  if ((addr_x = test_addr_x(node->lhs))) {
    off = gen_addr_x(node->lhs,false);
    println("\tldab %d,x",off+1);
    println("\tldaa %d,x",off);
  }else{
    gen_expr(node->lhs);
  }
  switch(node->rhs->kind){
  case ND_NUM:
    switch (node->rhs->ty->kind) {
    case TY_INT:
    case TY_SHORT:
    case TY_ENUM:
      switch(node->rhs->val){
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
        if (addr_x) {
          println("\taslb");
          println("\trola");
          println("\taddb %d,x",off+1);
          println("\tadca %d,x",off);
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
        if (addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taddb %d,x",off+1);
          println("\tadca %d,x",off);
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
        if (addr_x) {
          println("\taslb");
          println("\trola");
          println("\taddb %d,x",off+1);
          println("\tadca %d,x",off);
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
        if (addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\tsubb %d,x",off+1);
          println("\tsbca %d,x",off);
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
        if (addr_x) {
          println("\taslb");
          println("\trola");
          println("\taslb");
          println("\trola");
          println("\taddb %d,x",off+1);
          println("\tadca %d,x",off);
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
    if (node->rhs->val>0 && node->rhs->val<256) {
      println("\tldx #%ld",node->rhs->val);
      println("\tjsr __mul16x8x");
      IX_Dest = IX_None;
      return true;
    }
  }
  push();
  gen_expr(node->rhs);
  println("\tjsr __mul16x16");
  IX_Dest = IX_None;
  ins(2);
  return true;
}

