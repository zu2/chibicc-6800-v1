#include "chibicc.h"

//
// @long = lhs op rhs, where is the node (lhs,rhs) ?
//
// 0: nowhere
// 1: integer constant (#imm)
// 2: local frame (off,x)
// 3: global label (_name)
// 4: other, test_addr_x() holds (takes IX)
//
static int long_location_type(Node *node)
{
  if (is_long_constant(node,NULL)) {
    return 1;
  }

  if (node->kind == ND_VAR) {
    if (node->var->ty->kind == TY_VLA)
      return 0;
    if (node->var->is_local && test_addr_x(node))
      return 2;
    if (is_global_var(node))
      return 3;
    return 0;
  }
  if (is_var_addr_constant(node)) {
    return 3;
  }
  if (test_addr_x(node)) {
    return 4;
  }
  return 0;
}

int gen_direct_shl_long(Node *node,int64_t val)
{
  if (node->kind != ND_SHL) {
    return 0;
  }

  if ( val == 0) {
    return 1;
  }
  if ( val >= 32 ) {
    println("\tldx #0");
    println("\tstx @long+2");
    println("\tstx @long");
    IX_invalidate();
    return 1;
  }
  switch (val) {
  case 24:
    println("\tclra");
    println("\tldab @long+3");
    println("\tstab @long");
    println("\tstaa @long+3");
    println("\tstaa @long+2");
    println("\tstaa @long+1");
    return 1;
  case 16:
    println("\tldx @long+2");
    println("\tstx @long");
    println("\tldx #0");
    println("\tstx @long+2");
    IX_invalidate();
    return 1;
  case 8:
    println("\tldx @long+1");
    println("\tstx @long");
    println("\tldab @long+3");
    println("\tstab @long+2");
    println("\tclr @long+3");
    IX_invalidate();
    return 1;
  }
  return 0;
}

int gen_direct_shr_long(Node *node,int64_t val)
{
  Node *lhs = node->lhs;

  if (node->kind != ND_SHR)
    return 0;

  if ( val >= 32 ) {
    println("\tclra");
    if (!lhs->ty->is_unsigned) {
      println("\tasl @long");
      println("\tsbca #0");
    }
    println("\tstaa @long+3");
    println("\tstaa @long+2");
    println("\tstaa @long+1");
    println("\tstaa @long");
    return 1;
  }
  switch (val) {
  case 24:
  case 16:
  case 8:
    println("\tclra");
    if (!lhs->ty->is_unsigned) {
      char *skip = new_jump_label();
      println("\ttst @long");
      println("\tbpl %s",skip);
      println("\tdeca");
      println("%s:",skip);
    }
  }
  switch (val) {
  case 24:
    println("\tldab @long");
    println("\tstab @long+3");
    println("\tstaa @long+2");
    println("\tstaa @long+1");
    println("\tstaa @long");
    return 1;
  case 16:
    println("\tldx @long");
    println("\tstx @long+2");
    println("\tstaa @long+1");
    println("\tstaa @long");
    IX_invalidate();
    return 1;
  case 8:
    println("\tldx @long+1");
    println("\tstx @long+2");
    println("\tldab @long");
    println("\tstab @long+1");
    println("\tstaa @long");
    IX_invalidate();
    return 1;
  }
  return 0;
}

void gen_direct_long_addsub_imm(Node *node, int64_t val)
{
  char *opb, *opa;
  bool emitted = false;

  if (val==1 || val==-1) {
    switch (node->kind) {
    case ND_ADD: println(val==1 ? "\tjsr __inc32" : "\tjsr __dec32"); break;
    case ND_SUB: println(val==1 ? "\tjsr __dec32" : "\tjsr __inc32"); break;
    default: assert(0);
    }
    return;
  }

  if (!opt('O','2')) {
    switch (node->kind) {
    case ND_ADD: println("\tjsr __add32i"); break;
    case ND_SUB: println("\tjsr __sub32i"); break;
    default: assert(0);
    }
    word32i(val);
    IX_invalidate();
    return;
  }

  switch (node->kind) {
  case ND_ADD: opb="addb"; opa="adca"; break;
  case ND_SUB: opb="subb"; opa="sbca"; break;
  default: assert(0);
  }

  for (int nth = 3; nth >= 0; nth--) {
    uint8_t imm = (val >> ((3-nth)*8)) & 0xFF;

    if (!emitted && imm == 0) {
      continue;
    }
    if (!emitted) {
      println("\tldab @long+%d", nth);
      println("\t%s #%u", opb, imm);
      println("\tstab @long+%d", nth);
      emitted = true;
    } else {
      println("\tldaa @long+%d", nth);
      println("\t%s #%u", opa, imm);
      println("\tstaa @long+%d", nth);
    }
  }
}

void gen_direct_long_bitop_imm(Node *node, int64_t val)
{
  char *opb;
  char *fmt;
  uint8_t keep;
  uint8_t whole;

  if (!opt('O','2')) {
    switch (node->kind) {
    case ND_BITAND: println("\tjsr __and32i"); break;
    case ND_BITOR:  println("\tjsr __or32i");  break;
    case ND_BITXOR: println("\tjsr __xor32i"); break;
    default: assert(0);
    }
    word32i(val);
    IX_invalidate();
    return;
  }

  switch (node->kind) {
  case ND_BITAND: opb="andb"; keep=0xFF; whole=0x00; fmt="\tclr @long+%d"; break;
  case ND_BITOR:  opb="orab"; keep=0x00; whole=0xFF; fmt="\tldab #$FF\n\tstab @long+%d"; break;
  case ND_BITXOR: opb="eorb"; keep=0x00; whole=0xFF; fmt="\tcom @long+%d"; break;
  default: assert(0);
  }

  for (int nth = 3; nth >= 0; nth--) {
    uint8_t imm = (val >> ((3-nth)*8)) & 0xFF;

    if (imm == keep) {
      continue;
    }
    if (imm == whole) {
      println(fmt, nth);
      continue;
    }
    println("\tldab @long+%d", nth);
    println("\t%s #%u", opb, imm);
    println("\tstab @long+%d", nth);
  }
}

void gen_direct_long(Node *node)
{
  Node *rhs = skip_empty_cast(node->rhs);
  char *opb, *opa;
  int R = long_location_type(rhs);
  int roff = 0;
  char *raddr = NULL;

  assert(R == 2 || R == 3 || R == 4);

  switch (node->kind) {
  case ND_ADD:    opb="addb"; opa="adca"; break;
  case ND_SUB:    opb="subb"; opa="sbca"; break;
  case ND_BITAND: opb="andb"; opa="anda"; break;
  case ND_BITOR:  opb="orab"; opa="oraa"; break;
  case ND_BITXOR: opb="eorb"; opa="eora"; break;
  default: assert(0);
  }

  if (R==2 || R==4) {
    roff = gen_addr_x(rhs);
  }
  if (R==3) {
    raddr = is_var_addr_constant(rhs);
  }

  println("\tldab @long+3");
  if (R == 3) {
    println("\t%s %s+3", opb, raddr);
  } else {
    println("\t%s %d,x",  opb, roff+3);
  }
  println("\tstab @long+3");

  for (int nth = 2; nth >= 0; nth--) {
    println("\tldaa @long+%d", nth);
    if (R == 3) {
      println("\t%s %s+%d", opa, raddr, nth);
    } else {
      println("\t%s %d,x",   opa, roff+nth);
    }
    println("\tstaa @long+%d", nth);
  }
}

bool can_direct_long(Node *node)
{
  return long_location_type(skip_empty_cast(node->rhs)) != 0;
}

bool gen_direct_long2(Node *node)
{
  Node *lhs = skip_empty_cast(node->lhs);
  Node *rhs = skip_empty_cast(node->rhs);
  char *opb, *opa;
  int L = long_location_type(lhs);
  int R = long_location_type(rhs);
  int loff = 0;
  int roff = 0;
  char *laddr = NULL;
  char *raddr = NULL;
  int64_t lv = 0;
  int64_t rv = 0;

  switch (node->kind) {
  case ND_ADD:    opb="addb"; opa="adca"; break;
  case ND_SUB:    opb="subb"; opa="sbca"; break;
  case ND_BITAND: opb="andb"; opa="anda"; break;
  case ND_BITOR:  opb="orab"; opa="oraa"; break;
  case ND_BITXOR: opb="eorb"; opa="eora"; break;
  default: assert(0);
  }

  if (L == 1) is_long_constant(lhs,&lv);
  if (R == 1) is_long_constant(rhs,&rv);

  if (L==2 || L==4) loff = gen_addr_x(lhs);
  if (R==2 || R==4) roff = gen_addr_x(rhs);
  if (L==3)         laddr = is_var_addr_constant(lhs);
  if (R==3)         raddr = is_var_addr_constant(rhs);

  for (int i = 3; i >= 0; i--) {
    char *ld = (i==3) ? "ldab" : "ldaa";
    char *st = (i==3) ? "stab" : "staa";
    char *op = (i==3) ? opb    : opa;
    int   sh = (3-i)*8;

    switch (L) {
    case 1: println("\t%s #%d",    ld, (int)((lv >> sh) & 0xFF)); break;
    case 2: // THRU
    case 4: println("\t%s %d,x",   ld, loff+i);                   break;
    case 3: println("\t%s %s+%d",  ld, laddr, i);                 break;
    default: assert(0);
    }

    switch (R) {
    case 1: println("\t%s #%d",    op, (int)((rv >> sh) & 0xFF)); break;
    case 2: // THRU
    case 4: println("\t%s %d,x",   op, roff+i);                   break;
    case 3: println("\t%s %s+%d",  op, raddr, i);                 break;
    default: assert(0);
    }

    println("\t%s @long+%d", st, i);
  }
  return true;
}

bool can_direct_long2(Node *node)
{
  Node *lhs = skip_empty_cast(node->lhs);
  Node *rhs = skip_empty_cast(node->rhs);
  int L = long_location_type(lhs);
  int R = long_location_type(rhs);

  if (!L || !R)             return false;
  if (L==1 || L==3)         return true;
  if (R==1 || R==3)         return true;
  if (L==2 && R==2)         return true;

  return false;
}
