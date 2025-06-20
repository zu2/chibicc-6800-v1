#include "chibicc.h"

static FILE *output_file;
int depth;
static Obj *current_fn;
IX_Type	IX_Dest = IX_None;

static void gen_stmt(Node *node);


__attribute__((format(printf, 1, 2)))
void println(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(output_file, fmt, ap);
  va_end(ap);
  fprintf(output_file, "\n");
}

__attribute__((format(printf, 1, 2)))
void printout(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(output_file, fmt, ap);
  va_end(ap);
}

int count(void) {
  static int i = 1;
  return i++;
}

char *new_label(char *fmt)
{
  char *buf = calloc(1,strlen(fmt)+20); // 20?

  sprintf(buf, fmt, count());

  return buf;
}

void push1(void) {	// push char parameter
  println("\tpshb");
  depth+=1;
}

void push(void) {
  println("\tpshb");
  println("\tpsha");
  depth+=2;
}

void pop1(void) {
  println("\tpulb");
  depth-=1;
}
void popa(void) {
  println("\tpula");
  depth-=1;
}
void pop(void) {
  println("\tpula");
  println("\tpulb");
  depth-=2;
}

void popx(void) {
  if (opt_O == 's') {
    println("\tjsr __popx");
  } else {
    println("\ttsx");
    println("\tldx 0,x");
    println("\tins");
    println("\tins");
  }
  IX_Dest = IX_None;
  depth-=2;
}

static void ins(int n)
{
  depth -= n;
  while (n-->0) {
    println("\tins");
  }
}
static void des(int n)
{
  depth -= n;
  while (n-->0) {
    println("\tdes");
  }
}

static void ldab_i(int n)
{
  if ((n & 0x00ff)==0) {
    println("\tclrb");
  } else {
    println("\tldab #<%d",n);
  }
}

static void ldd_i(int n)
{
  if ((n & 0x00ff)==0) {
    println("\tclrb");
  } else {
    println("\tldab #<%d",n);
  }

  if ((n & 0x0ff00)==0) {
    println("\tclra");
  } else if ((n & 0x00ff) == ((n & 0x0ff00)>>8)) {
    println("\ttba");
  } else {
    println("\tldaa #>%d",n);
  }
}

static void and_i(int n)
{
  if ((n & 0x00ff)==0) {
    println("\tclrb");
  } else if ((n & 0x00ff)!=0x00ff) {
    println("\tandb #<$%02x",n);
  }
  if ((n & 0x0ff00)==0) {
    println("\tclra");
  } else if ((n & 0xff00)!=0xff00) {
    println("\tanda #>$%02x",n);
  }
}

static void pushl(void) {
  if (opt_O == 's') {
    println("\tjsr __push32");
    IX_Dest = IX_None;
  }else{
    println("\tldab @long+3");
    println("\tpshb");
    println("\tldab @long+2");
    println("\tpshb");
    println("\tldab @long+1");
    println("\tpshb");
    println("\tldab @long");
    println("\tpshb");
  }
  depth+=4;
}

static void pushf(void) {
  pushl();
}

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
int align_to(int n, int align) {
  return n;	// 6800 has no alignment restrictions.
//  return (n + align - 1) / align * align;
}

static void negd()
{
  println("\tnega");
  println("\tnegb");
  println("\tsbca #0");
}

static void ldx_bp()
{
  if (IX_Dest != IX_BP){
    if (opt_g >= '2') {
      println("; stack depth = %d",depth);
    }
    if(depth==0 && !current_fn->use_alloca) {
      println("\ttsx");
    }else{
      println("\tldx @bp");	// TSX cannot be used because of VLA/alloca
    }
  }
  IX_Dest = IX_BP;
}

static void tfr_dx()
{
  if (opt_O == 's') {
    println("\tjsr __tfr_dx");
  }else{
    println("\tstab @tmp1+1");
    println("\tstaa @tmp1");
    println("\tldx @tmp1");
  }
  IX_Dest = IX_None;
}

static void load32x(int off)
{
  if (opt_O == 's') {
    if (off==0) {
      println("\tjsr __load32x");
    }else if (off<=255) {
      println("\tldab #<%d",off);
      println("\tjsr __load32bx");
      IX_Dest = IX_None;
    }else{
      ldd_i(off);
      println("\tjsr __load32dx");
      IX_Dest = IX_None;
    }
  }else{
    println("\tldab %d,x",off+3);
    println("\tstab @long+3");
    println("\tldab %d,x",off+2);
    println("\tstab @long+2");
    println("\tldab %d,x",off+1);
    println("\tstab @long+1");
    println("\tldab %d,x",off);
    println("\tstab @long");
  }
}

//
// store @long to off,x
//
static void store32x(int off)
{
  if (off == 0) {
    println("\tjsr __store32x");
  }else if (off > 255 || off<0) {           // Offset too large for 1 byte
    ldd_i(off);
    println("\tjsr __store32dx");
    IX_Dest = IX_None;
  }else if (opt_O == 's' || off>252) {      // -Os enabled or   long offset > 255
    println("\tldab #%d",off);
    println("\tjsr __store32bx");
    IX_Dest = IX_None;
  }else{
    println("\tldab @long+3");
    println("\tstab %d,x",off+3);
    println("\tldab @long+2");
    println("\tstab %d,x",off+2);
    println("\tldab @long+1");
    println("\tstab %d,x",off+1);
    println("\tldab @long");
    println("\tstab %d,x",off);
  }
  return;
}

static char *helper_savex[] = {
  "__eq16","__ne16", "__lt16s", "__lt16u", "__gt16s", "__gt16u",
  "__le16s", "__le16u", "__ge16s", "__ge16u",
  NULL
};

void helper(char *s)
{
 println("\tjsr %s",s);
 for (char **p=helper_savex; *p!=NULL; p++){
   if(strcmp(s,*p)==0)
     return;
 }
 IX_Dest = IX_None;
}

//
// node is local variable ?
//
bool is_local_var(Node *node)
{
  if (node->kind == ND_VAR
  &&  node->var->ty->kind != TY_VLA
  &&  node->var->is_local ) {
    return 1;
  }
  return 0;
}

//
// node is global variable?
//
bool is_global_var(Node *node)
{
   if (node->kind != ND_VAR)
     return 0;
   if (node->var->ty->kind == TY_VLA)
     return 0;
   if (node->var->is_local)
     return 0;
   if (node->ty->kind == TY_FUNC)
     return 0;
   if (node->ty->kind == TY_ARRAY)
     return 0;

   return 1;
}

//
// node is global array?
//
bool is_global_array(Node *node)
{
   if (node->kind != ND_VAR)
     return 0;
   if (node->var->ty->kind == TY_VLA)
     return 0;
   if (node->var->is_local)
     return 0;
   if (node->ty->kind == TY_FUNC)
     return 0;
   if (node->ty->kind == TY_ARRAY)
     return 1;

   return 0;
}

//
// node is global variable or global array with constant subscript
//
bool is_global_var_or_array(Node *node)
{
   if (node->kind != ND_VAR)
     return 0;
   if (node->var->ty->kind == TY_VLA)
     return 0;
   if (node->var->is_local)
     return 0;
   if (node->ty->kind == TY_FUNC)
     return 0;
   if (node->ty->kind == TY_ARRAY) {  // TODO:XXX
     return 0;
   }

   return 1;
}

Type *is_integer_constant(Node *node, int64_t *val)
{
  if (node->kind == ND_CAST && node->ty->kind == TY_INT) {
    node = node->lhs;
  }
  if (node->kind != ND_NUM)
    return NULL;
  if (!is_integer(node->ty))
    return NULL;

  *val = node->val;

  return node->ty;
}

Type *is_flonum_constant(Node *node, double *val)
{
  if (node->kind != ND_NUM)
    return NULL;
  if (!is_flonum(node->ty))
    return NULL;

  *val = node->fval;

  return node->ty;
}

//
// node is array name?
//
bool is_var_array(Node *node)
{
  return (node->kind == ND_VAR && node->ty->kind == TY_ARRAY);
}


//
// cf. https://www.zukeran.org/shin/d/2024/12/30/6800-programing-11/
//
bool gen_shl(Type *ty, uint64_t val)
{
  if (!is_integer(ty))
    return false;
  if (val<0)
    return false;
  if (val==0)
    return true;            // do nothing
  if (val>=ty->size*8)      // TODO: bit field
    return false;
  switch(ty->size){
  case 1:
    switch(val) {
    case 6:                 // 8cyc, 5bytes
      println("\trorb");
      println("\trorb");
      println("\trorb");
      println("\tandb #$C0");
      return true;
    case 7:                 // 6cyc, 4bytes
      println("\trorb");
      println("\trorb");
      println("\tandb #$80");
      return true;
    }
    for (int i=0; i<val; i++) {   // 1-5,  2*n cyc, n bytes
      println("\taslb");
    } 
    return true;
  case 2: {                 // short and int    // TODO: -Os
    switch(val) {           // Tricky but fast and compact
    case 6:                 // 22cyc, 10bytes
      println("\tpsha");    // exchange A<->B
      println("\ttba");
      println("\tpulb");
      println("\tlsrb");
      println("\trora");
      println("\trorb");
      println("\trora");
      println("\trorb");
      println("\tandb #$C0");
      return true;
    case 7:                 // 10cyc, 6bytes
      println("\tlsra");
      println("\ttba");
      println("\trora");
      println("\trorb");
      println("\tandb #$80");
      return true;
    case 14:                // 12cyc, 6bytes
    case 15:                // 8cyc,  4bytes
      println("\tclra");
      for (int i=0; i<16-val; i++) {
        println("\tlsrb");
        println("\trora");
      }
      println("\tclrb");
      return true;
    }
    if (val>=8) {                 // 8-13, 2*(n-8)+4 cyc, (n-8)+2 bytes
      println("\ttba");
      println("\tclrb");
      for (int i=0; i<val-8; i++) {
        println("\tasla");
      }
      return true;
    }
    for (int i=0; i<val; i++) {   // 1-5, 4*n cyc, 2*n bytes
      println("\taslb");
      println("\trola");
    } 
    return true;
  }
  case 4:
    ldd_i(val);
    println("\tjsr __shl32");
    return true;
  }
  assert(0); // what's?
}

bool gen_shr(Type *ty, uint64_t val)
{
  if (!is_integer(ty))
    return false;
  if (val==0)
    return true;
  if (val<0)
    return false;
  if (val>=ty->size*8)
    return false;
  switch(ty->size){
  case 1:
    for (int i=0; i<val; i++) {
      if (ty->is_unsigned) {
        println("\tlsrb");
      }else{
        println("\tasrb");
      } 
    }
    return true;
  case 2:
    if (val>4) {
      println("; %s %ld", ty->is_unsigned? "lsrd": "asrd", val);
    }
    if (ty->is_unsigned) {
      switch(val) {
      case 1:  // 4cyc,  2bytes
      case 2:  // 8cyc,  4bytes
      case 3:  // 12cyc, 6bytes
      case 4:  // 16cyc, 8bytes
      case 5:  // 20cyc, 10bytes
        for (int i=0; i<val; i++) {
          println("\tlsra");
          println("\trorb");
        }
        return true;
      case 6:   // 18cyc, 10bytes
        println("\taslb");
        println("\trola");
        println("\trolb");
        println("\trola");
        println("\trolb");
        println("\tpshb");
        println("\ttab");
        println("\tpula");
        println("\tanda #$03");
        return true;
      case 7:   // 10cyc, 6bytes
        println("\taslb");
        println("\trola");
        println("\ttab");
        println("\trola");
        println("\tanda #$01");
        return true;
      case 8:   // 4cyc,  4bytes
      case 9:   // 6cyc,  5bytes
      case 10:  // 8cyc,  6bytes
      case 12:  // 10cyc, 7bytes
      case 13:  // 12cyc, 8bytes
        println("\ttab");
        for (int i=8; i<val; i++) {
          println("\tlsrb");
        }
        println("\tclra");
        return true;
      case 14:  // 12cyc, 6bytes
      case 15:  // 8cyc,  4bytes
        println("\tclrb");
        for (int i=val; i<=15; i++) {
          println("\tasla");
          println("\trolb");
        }
        println("\tclra");
        return true;
      }
    } else {
      switch(val) {
      case 1:  // 4cyc,  2bytes
      case 2:  // 8cyc,  4bytes
      case 3:  // 12cyc, 6bytes
      case 4:  // 16cyc, 8bytes
      case 5:  // 20cyc, 10bytes
      case 6:  // 24cyc, 12bytes
        for (int i=0; i<val; i++) {
          println("\tasra");
          println("\trorb");
        }
        return true;
      case 7:   // 10cyc, 6bytes
        println("\taslb");
        println("\trola");
        println("\ttab");
        println("\tldaa #0");
        println("\tsbca #0");
        return true;
      case 8:   // 4cyc,  6bytes
      case 9:   // 6cyc,  7bytes
      case 10:  // 8cyc,  8bytes
      case 12:  // 10cyc, 9bytes
      case 13:  // 12cyc, 10bytes
        println("\ttab");
        println("\tasla");
        println("\tldaa #0");
        println("\tsbca #0");
        for (int i=8; i<val; i++) {
          println("\tasrb");
        }
        return true;
      case 14:  // 12cyc, 7bytes
        println("\tclrb");
        println("\tasla");
        println("\tsbcb #0");
        println("\tasla");
        println("\ttba");
        println("\trolb");
        return true;
      case 15:  // 8cyc,  5bytes
        println("\tclrb");
        println("\tasla");
        println("\tsbcb #0");
        println("\ttba");
        return true;
      }
    }
    break;
  case 4:
    for (int i=0; i<val; i++) {
      if (ty->is_unsigned) {
        println("\tlsr @long");
      }else{
        println("\tasr @long");
      }
      println("\trol @long+1");
      println("\trol @long+2");
      println("\trol @long+3");
    } 
    return true;
  }
  assert(0); // what's?
}

//
// jsr mul16x16 uses 10bytes; unrolled if smaller, even for Os
//
//    pshb          // 1 4
//    psha          // 1 4
//    ldab #xx      // 2 2
//    clra          // 2 1
//    jsr mul16x16  // 9 3
//    ins           // 4 1
//    ins           // 4 1
//
bool
gen_mul16(Node *node)
{
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
        if (opt_O == 's')
          return false;
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
        if (opt_O == 's')
          return false;
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
        if (opt_O == 's')
          return false;
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
        if (opt_O == 's')
          return false;
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
      case 100:
        println("\tjsr __mul100");
        return true;
      }
    }
  }
  return false;
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static void gen_addr(Node *node)
{
  switch (node->kind) {
  case ND_VAR:
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA) {
      println("\tldab @bp+1");
      println("\tldaa @bp");
      if (node->var->offset) {
        println("\taddb #<%d",node->var->offset);
        println("\tadca #>%d",node->var->offset);
      }
      tfr_dx();
      println("\tldab 1,x");
      println("\tldaa 0,x");
      return;
    }

    // Local variable
    if (node->var->is_local) {
      println("\tldab @bp+1");
      println("\tldaa @bp");
      if (node->var->offset) {
        println("\taddb #<%d",node->var->offset);
        println("\tadca #>%d",node->var->offset);
      }
      return;
    }

    // Here, we generate an absolute address of a function or a global
    // variable.

    // Function and Global variable
    println("\tldab #<_%s", node->var->name);
    println("\tldaa #>_%s", node->var->name);
    return;
  case ND_DEREF:
    gen_expr(node->lhs);
    return;
  case ND_COMMA:
    gen_expr(node->lhs);
    gen_addr(node->rhs);
    return;
  case ND_MEMBER:
    gen_addr(node->lhs);
    if (node->member->offset) {
      println("\taddb #<%d",node->member->offset);
      println("\tadca #>%d",node->member->offset);
    }
    return;
  case ND_FUNCALL:
    if (node->ret_buffer) {
      gen_expr(node);
      return;
    }
    break;
  case ND_ASSIGN:
  case ND_COND:
    if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION) {
      gen_expr(node);
      return;
    }
    break;
  case ND_VLA_PTR:
    println("\tldab @bp+1");
    println("\tldaa @bp");
    if (node->var->offset) {
      println("\taddb #<%d",node->var->offset);
      println("\tadca #>%d",node->var->offset);
    }
    return;
  }

  error_tok(node->tok, "not an lvalue");
}

static int ldx_x(Type *ty,int off);
int gen_expr_x(Node *node,bool save_d);
bool test_expr_x(Node *node);

int gen_expr_x_sub(Node *node,bool save_d,bool test)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;
  int64_t val;

  switch(node->kind) {
  case ND_NULL_EXPR:
    return false;
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
      return false;
    case TY_LONG:
      if (test) return true;
      println("\tldx @long+2");
      return 0;
    case TY_BOOL:
    case TY_CHAR:
      return false;
    case TY_SHORT:
    case TY_INT:
    case TY_PTR:
    case TY_ENUM:
      if (test) return true;
      println("\tldx #%d",(unsigned int)((node->val & 0x0ffff)));
      IX_Dest = IX_None;
      return 0;
    default: 
      break;
    }
  } // ND_NUM
  case ND_NEG:
    return false;
  case ND_VAR:
    if (test_addr_x(node)) {
      if (test) return true;
      off = gen_addr_x(node,true);
      off = ldx_x(node->ty,off);
      return off;
    }
    return 0;
  case ND_MEMBER: {
    Member *mem = node->member;
    if (mem->is_bitfield) {     // bitfield cannot be move to IX
      return false;
    }
    if (test_addr_x(node)) {
      if (test) return true;
      off = gen_addr_x(node,true);
      off = ldx_x(node->ty,off);
      return off;
    }
    return false;
  }; // ND_MEMBER:
  case ND_DEREF: {
    if (test_expr_x(lhs)) {
      if (test) return true;
      off = gen_expr_x(lhs,true);
      off = ldx_x(node->ty,off);
      return off;
    }
    return 0;
  } // ND_DEREF
  case ND_ADDR: {
    if (test_addr_x(lhs)) {
      if (test) return true;
      off = gen_addr_x(lhs,true);
      return off;
    }
    return false;
  } // ND_ADDR;
//case ND_ASSIGN:
//case ND_STMT_EXPR:
//case ND_COMMA:
  case ND_CAST: {
    if (test_expr_x(lhs)) {
      if (test) return true;
      off = gen_expr_x(lhs,true);
      return off;
    }
    return false;
  } // ND_CAST:
// case ND_MEMZERO:
// case ND_COND:
// case ND_NOT:
// case ND_BITNOT:
// case ND_LOGAND:
// case ND_LOGOR:
// case ND_FUNCALL:
// case ND_LABEL_VAL:
#if 1
  // rethink. check ARRAY or Pointer
  case ND_ADD:
    if (lhs->kind      == ND_CAST
    &&  lhs->ty->kind  == TY_PTR
    &&  test_addr_x(lhs->lhs)
    &&  rhs->kind      == ND_CAST
    &&  rhs->ty->kind  == TY_PTR
    &&  rhs->lhs->kind == ND_NUM
    &&  is_integer(rhs->lhs->ty)) {
      if (lhs->lhs->kind == ND_VAR
      &&  lhs->lhs->var->is_local) {
        off = lhs->lhs->var->offset + rhs->lhs->val;
        if (off<=252) {
          if (test) return true;
          off = gen_addr_x(lhs->lhs,true) + rhs->lhs->val;
          return off;
        }
      }
    }
    //(+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) ua +0 ) 6)
    if (lhs->kind     == ND_VAR
    &&  lhs->ty->kind == TY_ARRAY
    &&  lhs->var->is_local
    &&  is_integer_constant(rhs,&val)) {
      off = lhs->var->offset + val;
      if (off <= 252) {
        if (test) return true;
        return gen_addr_x(lhs,true) + val;
      }
    }
    if (lhs->kind     == ND_VAR
    &&  lhs->ty->kind == TY_ARRAY
    &&  !lhs->var->is_local
    &&  is_integer_constant(rhs,&val)) {
      if (val <= 252) { // TODO: Preferably, use label + constant instead of IX.
        if (test) return true;
        return gen_addr_x(lhs,true) + val;
      }
    }
    return false;
#endif
  default:
    return false;
  }
  if (test) {
    return false;
  }
  error_tok(node->tok, "invalid expression at %s node->kind %d",__func__,node->kind);
}

int gen_expr_x(Node *node,bool save_d)
{
  return gen_expr_x_sub(node,save_d,false);
}

bool test_expr_x(Node *node)
{
  return  gen_expr_x_sub(node,true,true);
}


// Compute the absolute address of a given node in IX.
// It's an error if a given node does not reside in memory.
int gen_addr_x_sub(Node *node,bool save_d,bool test)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;

  switch (node->kind) {
  case ND_VAR:
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA){
      if (node->var->offset<=254) {
        if (test) return 1;
        ldx_bp();
        println("\tldx %d,x	; gen_addr_x():TY_LDA ",node->var->offset);
        IX_Dest = IX_None;
        return 0;
      }
      goto fallback;
    }
    // Local variable
    if (node->var->is_local) {
      if (node->var->offset <= 254){
        if (test) return 1;
        ldx_bp();
        return node->var->offset;
      }
      goto fallback;
    }
    // Function
    if (node->ty->kind == TY_FUNC) {
      if (test) return 1;
      println("\tldx #_%s",node->var->name);
      IX_Dest = IX_None;
      return 0;
    }
    // maybe Global variable
    if (test) return 1;
    println("\tldx #_%s",node->var->name);
    IX_Dest = IX_None;
    return 0;
  case ND_DEREF:
    if (!test_expr_x(node->lhs)) {
      return 0;
    }
    if (test) {
      return true;
    }
    off = gen_expr_x(node->lhs,false);
    return off;
  case ND_COMMA:
    return false;
  case ND_MEMBER:
    if (!test_addr_x(lhs)) {
      return 0;
    }
    if (test) {
      return true;
    }
    off = gen_addr_x(node->lhs,save_d) + node->member->offset;
    if (off<=252) {
      return off;
    }
    // fall back
    if (save_d)
      push();
    ldd_i(off);
    println("\tjsr __adx");
    if (off == 0) {
      return off;
    }
    if (save_d)
      pop();
    return 0;
  case ND_FUNCALL:
    return false;
    // TODO: Consider the correct address generation method
    if (node->ret_buffer) {
      if (test_expr_x(node)) {
        if (test) return true;
        off = gen_expr_x(node,false);
        return off;
      }
      return false;
    }
    return false;
  case ND_ASSIGN:
  case ND_COND:
    return false;
    if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION) {
      if (test_expr_x(node)) {
        if (test) {
          return true;
        }
        off = gen_expr_x(node,false);
        return off;
      }
      return false;
    }
    break;
  case ND_VLA_PTR:
    return false;
    if (node->var->offset <= 252) {
      if (test) {
        return true;
      }
      ldx_bp();
      return node->var->offset;
    }
    goto fallback;
// (+ (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) a +0 ))
//    (ND_CAST TY_PTR(10) 1))
  }
  if (test) return false;
  error_tok(node->tok, "not an lvalue at gen_addr_x, node->kind %d",node->kind);
  // fallback to gen_addr()
fallback:
  if (test) return 0;
  println("; fall back to gen_addr() save_d %d",save_d);
  if (save_d)
    push();
  gen_addr(node);
  tfr_dx();
  if (save_d)
    pop();
  return 0;
}

int gen_addr_x(Node *node,bool save_d)
{
  int off = gen_addr_x_sub(node,save_d,false);
  return off;
}

bool test_addr_x(Node *node)
{
  int r =  gen_addr_x_sub(node,true,true);
  return r;
}

static void load32i(uint32_t val)
{
  uint16_t hi = (uint16_t)((val>>16)&0x0ffff);
  uint16_t lo = (uint16_t)(val&0x0ffff);

  if (opt_O == 's' && hi!=lo) {
    println("\tjsr __load32i");
    println("\t.word %u",hi);
    println("\t.word %u",lo);
    IX_Dest = IX_None;
    return;
  }
  println("\tldx #%u",lo);
  println("\tstx @long+2");
  if (lo+1 == hi){
    println("\tinx");
  }else if (lo-1 == hi){
    println("\tdex");
  }else if (hi!=lo) {
    println("\tldx #%u",hi);
  }
  println("\tstx @long");
  IX_Dest = IX_None;
}

// Load a value from where %rax is pointing to.
static void load(Type *ty) {
  switch (ty->kind) {
  case TY_ARRAY:
  case TY_STRUCT:
  case TY_UNION:
  case TY_FUNC:
  case TY_VLA:
    // If it is an array, do not attempt to load a value to the
    // register because in general we can't load an entire array to a
    // register. As a result, the result of an evaluation of an array
    // becomes not the array itself but the address of the array.
    // This is where "array is automatically converted to a pointer to
    // the first element of the array in C" occurs.
    return;
  case TY_FLOAT:
    println("\tjsr __load32");		// @long = (AccAB)");
    IX_Dest = IX_None;
//  println("  movss (%%rax), %%xmm0");
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
//  println("  movsd (%%rax), %%xmm0");
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
//  println("  fldt (%%rax)");
    return;
  }

// char *insn = ty->is_unsigned ? "movz" : "movs";

  // When we load a char or a short value to a register, we always
  // extend them to the size of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  if (ty->size == 1){
    if (opt_O == '2') {
      println("\tstab @tmp1+1");
      println("\tstaa @tmp1");
      println("\tldx  @tmp1");
      println("\tclra");
      println("\tldab 0,x");
      if (!ty->is_unsigned) {
        char *label = new_label("L_%d");
        println("\tbpl %s",label);
        println("\tcoma");
        println("%s:",label);
      }
    }else if (ty->is_unsigned){
      println("\tjsr __load8u");
    }else{
      println("\tjsr __load8s");
    }
  }else if (ty->size == 2){
    if (opt_O == '2') {
      println("\tstab @tmp1+1");
      println("\tstaa @tmp1");
      println("\tldx  @tmp1");
      println("\tldab 1,x");
      println("\tldaa 0,x");
    }else{
      println("\tjsr __load16");
    }
  }else if (ty->size == 4){
    println("\tjsr __load32");
  }else{
    fprintf(stderr,"; load error, ty->size=%d\n",ty->size);
    assert(0);
  }
  IX_Dest = IX_None;
}

bool can_load_x(Type *ty)
{
  switch(ty->kind) {
  case TY_ARRAY:
  case TY_STRUCT:
  case TY_UNION:
  case TY_FUNC:
  case TY_VLA:
    return false;
  case TY_DOUBLE:
  case TY_LDOUBLE:
    return false;
  case TY_PTR:
    return true;
  }
  return is_integer(ty);
}

static int ldx_x(Type *ty,int off)
{
  if (can_load_x(ty)) {
    println("\tldx %d,x",off);
    IX_Dest = IX_None;
    return 0;
  }
  return off;
}

static void load_x(Type *ty,int off) {
  // Note: Do not destroy IX in this routine.
  switch (ty->kind) {
  case TY_ARRAY:
  case TY_STRUCT:
  case TY_UNION:
  case TY_FUNC:
  case TY_VLA:
    // If it is an array, do not attempt to load a value to the
    // register because in general we can't load an entire array to a
    // register. As a result, the result of an evaluation of an array
    // becomes not the array itself but the address of the array.
    // This is where "array is automatically converted to a pointer to
    // the first element of the array in C" occurs.
    return;
  case TY_LONG:
  case TY_FLOAT:
    load32x(off);
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
//  println("  movsd (%%rax), %%xmm0");
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
//  println("  fldt (%%rax)");
    return;
  }

  // When we load a char or a short value to a register, we always
  // extend them to the size of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  if (ty->size == 1){
    println("\tldab %d,x",off);
  }else if (ty->size == 2){
    println("\tldab %d,x",off+1);
    println("\tldaa %d,x",off);
  }else if (ty->size == 4){
    println("\tldab %d,x",off+3);
    println("\tstab @long+3");
    println("\tldab %d,x",off+2);
    println("\tstab @long+2");
    println("\tldab %d,x",off+1);
    println("\tstab @long+1");
    println("\tldab %d,x",off);
    println("\tstab @long");
  }else
    println("  mov (%%rax), %%rax");
}

void load_var(Node *node)
{
  switch (node->ty->kind) {
  case TY_ARRAY:
  case TY_STRUCT:
  case TY_UNION:
  case TY_FUNC:
  case TY_VLA:
    gen_addr(node);
    load(node->ty);
    return;
  }
  if (is_global_var(node)){
    switch(node->ty->size) {
    case 1:
      println("\tldab _%s",  node->var->name);
      break;
    case 2:
      println("\tldab _%s+1",node->var->name);
      println("\tldaa _%s",  node->var->name);
      break;
    case 4:
      if (opt_O == 's') {
        println("\tldx #_%s",node->var->name);
        println("\tjsr __load32x");
      }else{
        println("\tldx _%s+2",node->var->name);
        println("\tstx @long+2");
        println("\tldx _%s",  node->var->name);
        println("\tstx @long");
      }
      IX_Dest = IX_None;
      break;
    default:
      assert(0);
    }
    return;
  }
  if (test_addr_x(node)) {
    int off = gen_addr_x(node,false);
    load_x(node->ty,off);
    return;
  }
  gen_addr(node);
  load(node->ty);
  return;
}

// Store D to an address that the stack top is pointing to.
static void store(Type *ty) {

  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    if (ty->size==0)
      return;
    println("; store struct/union from AB to *TOS, size %d in IX",ty->size);
    println("\tldx  #%d",ty->size);
    println("\tjsr  __copy_struct");	// remove tos
    depth -= 2;
    IX_Dest = IX_None;
    return;
  case TY_FLOAT:
    popx();
    println("\tjsr __store32x");	// store @long to (0-3,x)");
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
    return;
  }

  popx();
  if (ty->size == 1){
    println("\tstab 0,x");
  }else if (ty->size == 2){
    println("\tstab 1,x");
    println("\tstaa 0,x");
  }else if (ty->size == 4){
    println("\tjsr __store32x");	// store @long to (0-3,x)");
  }else
    assert(0);
}

static void store_x(Type *ty,int off) {
  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    println("; store struct/union from *TOS to IX+off, size %d",ty->size);
    println("\tpshb");
    println("\tpsha");
    if (off!=0) {
      ldd_i(off);
      println("\tjsr __adx");
    }
    ldd_i(ty->size);
    println("\tjsr  __copy_struct2");
    IX_Dest = IX_None;
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
    return;
  }
  switch (ty->size) {
  case 1:
    println("\tstab %d,x",off);
    break;
  case 2:
    println("\tstab %d,x",off+1);
    println("\tstaa %d,x",off);
    break;
  case 4:
    store32x(off);	// if opt_O, destroy IX
    break;
  default:
    println("  mov %%rax, (%%rdi)");
    assert(0);
  }
}

void cmp_zero(Type *ty) {

  switch(ty->kind){
  case TY_BOOL:
  case TY_CHAR:
    println("\ttstb");
    return;
  case TY_SHORT:
  case TY_INT:
  case TY_ENUM:
  case TY_PTR:
    println("\taba");
    println("\tadca #0");
    return;
  case TY_LONG:
    println("\tjsr __iszero32");
    IX_Dest = IX_None;
    return;
  case TY_FLOAT:
    println("\tjsr __f32iszero");
    IX_Dest = IX_None;
    return;
  case TY_DOUBLE:
  case TY_LDOUBLE:
    error("; cmp_zero ty: %d DOUBLE/LDOUBLE",ty->kind);
  }
  error("; cmp_zero ty: %d",ty->kind);
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };

static int getTypeId(Type *ty) {
  switch (ty->kind) {
  case TY_BOOL:
  case TY_CHAR:
    return ty->is_unsigned ? U8 : I8;
  case TY_SHORT:
    return ty->is_unsigned ? U16 : I16;
  case TY_INT:
    return ty->is_unsigned ? U16 : I16;
  case TY_LONG:
    return ty->is_unsigned ? U32 : I32;
  case TY_FLOAT:
    return F32;
  case TY_DOUBLE:
  case TY_LDOUBLE:
    assert(ty->kind!=TY_DOUBLE && ty->kind!=TY_LDOUBLE);
    return F32;
  }
  return U16;
}

// The table for type casts
static char i8i16[]  = "clra\n\tasrb\n\trolb\n\tsbca #0";
static char i8u16[]  = "clra\n\tasrb\n\trolb\n\tsbca #0";
static char i8i32[]  = "jsr __s8to32";
static char i8u32[]  = "jsr __s8to32";
static char u8i16[]  = "clra";
static char u8u16[]  = "clra";
static char u8i32[]  = "jsr __u8to32";
static char u8u32[]  = "jsr __u8to32";
static char i16i32[] = "jsr __s16to32";
static char i16u32[] = "jsr __s16to32";
static char u16i32[] = "jsr __u16to32";
static char u16i64[] = ";jsr __u16i64 " __FILE__;
//static char u16u32[] = "jsr __u16to32";
static char u16f32[] = "jsr __u16tof32";
static char u16f64[] = ";jsr __u16tof32" __FILE__;

static char i16i64[] = ";jsr __i16i64 " __FILE__;
static char i16f32[] = "jsr __i16tof32";
static char i16f64[] = "; jsr __i16f64 " __FILE__;

static char i32i8[] = "ldab @long+3";
static char i32u8[] = "ldab @long+3";
static char i32i16[] = "ldab @long+3\n\tldaa @long+2";
static char i32u16[] = "ldab @long+3\n\tldaa @long+2";
static char i32f32[] = "jsr __i32tof32";
static char i32i64[] = "movsxd %eax, %rax";
static char i32f64[] = "cvtsi2sdl %eax, %xmm0";

static char u32f32[] = "jsr __u32tof32";
static char u32i64[] = "mov %eax, %eax";
static char u32f64[] = "mov %eax, %eax; cvtsi2sdq %rax, %xmm0";

static char i64i32[] = "; i64i32 " __FILE__;
static char i64u32[] = "; i64u32 " __FILE__;
static char i64f32[] = "cvtsi2ssq %rax, %xmm0";
static char i64f64[] = "cvtsi2sdq %rax, %xmm0";

static char u64f32[] = "cvtsi2ssq %rax, %xmm0";
static char u64f64[] =
  "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
  "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
  "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";

static char f32i8[] = "jsr __f32toi8";
static char f32u8[] = "jsr __f32tou8";
static char f32i16[] = "jsr __f32toi16";
static char f32u16[] = "jsr __f32tou16";
static char f32i32[] = "jsr __f32toi32";
static char f32u32[] = "jsr __f32tou32";
static char f32i64[] = "cvttss2siq %xmm0, %rax";
static char f32u64[] = "cvttss2siq %xmm0, %rax";
static char f32f64[] = "cvtss2sd %xmm0, %xmm0";

static char f64i8[] = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax";
static char f64u8[] = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax";
static char f64i16[] = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax";
static char f64u16[] = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";
static char f64i32[] = "cvttsd2sil %xmm0, %eax";
static char f64u32[] = "cvttsd2siq %xmm0, %rax";
static char f64i64[] = "cvttsd2siq %xmm0, %rax";
static char f64u64[] = "cvttsd2siq %xmm0, %rax";
static char f64f32[] = "cvtsd2ss %xmm0, %xmm0";

// ex. i32i16: i32->i16
static char *cast_table[][11] = {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  i8i16,   i8i32,  i16i64, NULL,  i8u16,   i8u32,  i16i64, i16f32, i16f64, NULL}, // i8
  {NULL,  NULL,   i16i32, i16i64, NULL,  NULL,   i16u32, i16i64, i16f32, i16f64, NULL}, // i16
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, NULL}, // i32
  {i32i8, i32i16, i64i32, NULL,   i32u8, i32u16, i64u32, NULL,   i64f32, i64f64, NULL}, // i64

  {NULL,  u8i16,   u8i32,  i16i64, NULL,  u8u16,   u8u32,  i16i64, i16f32, i16f64, NULL}, // u8
  {NULL,  NULL,   u16i32, i16i64, NULL,  NULL,   u16i32, u16i64, u16f32, u16f64, NULL}, // u16
  {i32i8, i32i16, NULL,   u32i64, i32u8, i32u16, NULL,   u32i64, u32f32, u32f64, NULL}, // u32
  {i32i8, i32i16, i64i32, NULL,   i32u8, i32u16, i64u32, NULL,   u64f32, u64f64, NULL}, // u64

  {f32i8, f32i16, f32i32, f32i64, f32u8, f32u16, f32u32, f32u64, NULL,   f32f64, NULL}, // f32
  {f64i8, f64i16, f64i32, f64i64, f64u8, f64u16, f64u32, f64u64, f64f32, NULL,   NULL}, // f64
  {NULL,  NULL,   NULL,   NULL,   NULL,  NULL,   NULL,   NULL,   NULL,   NULL,   NULL}, // f80
};

static void cast(Type *from, Type *to) {
  if (to->kind == TY_VOID)
    return;

  if (to->kind == TY_BOOL) {
    cmp_zero(from);
    int c = count();
    println("\tbeq L_false_%d", c);
    println("\tclra");
    println("\tldab #1");
    println("\tbra L_end_%d", c);
    println("L_false_%d:", c);
    println("\tclra");
    println("\tclrb");
    println("L_end_%d:", c);
    return;
  }

  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  if (cast_table[t1][t2]){
    println("\t%s", cast_table[t1][t2]);
    if(strncmp("jsr",cast_table[t1][t2],3)==0)
      IX_Dest = IX_None;
  }
}

static int is_empty_cast(Type *from, Type *to) {
  if (to->kind == TY_VOID)
    return 0;
#if 0
  if (to->kind == TY_BOOL)
    return 0;
#endif
  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  return cast_table[t1][t2]==NULL;
}

static void push_struct(Type *ty) {
  println("; stack depth = %d",depth);
  int sz = ty->size;
  assert(sz != 0);

  tfr_dx();
  if (ty->size<=8) {
    for (int i = ty->size-1; i >=0; i-- ){
      println("\tldab %d,x",i);
      println("\tpshb");
    }
  }else{
    ldd_i(sz);
    println("\tjsr __push_struct_x");
  }
  depth += sz;
  println("; stack depth = %d",depth);
}


void
gen_direct_pushl_sub(int val)
{
  static int a,b;

  if(val==-1){
    a = -1;
    b = -1;
    return;
  }
  if (a==val){
    println("\tpsha");
  }else if (b==val){
    println("\tpshb");
  }else if (val==0){
    if (b==-1 || a!=-1){
      println("\tclrb");
      println("\tpshb");
      b = 0;
    }else{
      println("\tclra");
      println("\tpsha");
      a = 0;
    }
  }else if (b==-1 || a!=-1){
    println("\tldab #%d",val);
    println("\tpshb");
    b = val;
  }else{
    println("\tldaa #%d",val);
    println("\tpsha");
    a = val;
  }
}

void
gen_direct_pushl(int64_t val)
{
  int v0 = (int)(val & 0x000000FF);
  int v1 = (int)((val & 0x0000FF00)>>8);
  int v2 = (int)((val & 0x00FF0000)>>16);
  int v3 = (int)((val & 0xFF000000)>>24);
  gen_direct_pushl_sub(-1);
  gen_direct_pushl_sub(v0);
  gen_direct_pushl_sub(v1);
  gen_direct_pushl_sub(v2);
  gen_direct_pushl_sub(v3);
  depth+=4;
}

void
pushlx(int off)
{
  if (opt_O == 's') {
    ldd_i(off);
    println("\tjsr __push32dx");
    IX_Dest = IX_None;
  }else{
    println("\tldab %d,x",off+3);
    println("\tpshb");
    println("\tldab %d,x",off+2);
    println("\tpshb");
    println("\tldab %d,x",off+1);
    println("\tpshb");
    println("\tldab %d,x",off);
    println("\tpshb");
  }
  depth+=4;
}

static void push_args2(Node *args,bool is_variadic)
{
  if (!args)
    return;
  push_args2(args->next,is_variadic);

  switch (args->ty->kind) {
  case TY_DOUBLE:
  case TY_LDOUBLE: 
    assert(args->ty->kind!=TY_DOUBLE && args->ty->kind!=TY_LDOUBLE);
    break;
  case TY_STRUCT:
  case TY_UNION:
    gen_expr(args);
    push_struct(args->ty);
    break;
  case TY_BOOL:
  case TY_CHAR: {
#if 0
    if (args->kind       == ND_CAST
    &&  args->lhs->kind  == ND_NUM
    &&  is_integer(args->lhs->ty)) {
      println("\tldab #<%ld",args->lhs->val);
#endif
    int64_t val;
    if (is_integer_constant(args, &val)) {
      println("\tldab #<%ld",val);
    }else{
      gen_expr(args);
    }
    if (args->pass_by_stack){
      if (is_variadic) {
        cast(args->ty, ty_int);
        push();
      }else{
        push1();
      }
    }
    break;
  } // TY_CHAR,TY_BOOL
  case TY_FLOAT:
    if (args->pass_by_stack && args->kind==ND_NUM) {
      union { float f32; uint32_t u32; } u = { args->fval };
      println("; push float %e, %08x",u.f32,u.u32);
      gen_direct_pushl(u.u32);
    }else{
      gen_expr(args);
      if (args->pass_by_stack){
        pushf();
      }
    }
    break;
  case TY_LONG: {
    if (!args->pass_by_stack){
        gen_expr(args);
	break;
    }
    int64_t val = args->val;
    switch(args->kind){
    case ND_CAST:
      if (is_empty_cast(args->lhs->ty, args->ty)
      &&  args->lhs->kind == ND_NUM){
        gen_direct_pushl(args->lhs->val);
        break;
      }
      if ((args->lhs->ty->kind==TY_INT || args->lhs->ty->kind==TY_SHORT)
      &&  args->lhs->kind==ND_NUM
      &&  args->lhs->val>=0) {
        gen_direct_pushl(args->lhs->val);
        break;
      }
      gen_expr(args);
      pushl();
      break;
    case ND_NUM:
      gen_direct_pushl(val);
      break;
    case ND_VAR:
      if (test_addr_x(args)){
        int off = gen_addr_x(args,false);
        pushlx(off);
      }else{
        gen_expr(args);
        pushl();
      }
      break;
    default:
      gen_expr(args);
      pushl();
    }
    break;
  } // TY_LONG
  default:
    gen_expr(args);
    if (args->pass_by_stack){
      push();
    }
    break;
  }
}

// Load function call arguments. Arguments are already evaluated and
// stored to the stack as local variables. What we need to do in this
// function is to load them to registers or push them to the stack as
// specified by the chibicc-6800 API.
//
// - Only first one argument passed by AccAB or @long: (1/2/4bytes)
// - No alignment
// - Other arguments are pushed onto the stack from right to left.
//
static int push_args(Node *node)
{
  // レジスタ渡しとスタック渡しを判別し、スタックサイズを計算する
  int reg_passable = !node->ret_buffer;
  int stack = 0;
  for (Node *arg = node->args; arg; arg = arg->next) {
    // check first parameter
    switch (arg->ty->kind) {
    case TY_VOID:
    case TY_DOUBLE:
    case TY_LDOUBLE:
      assert(0);
    case TY_STRUCT:
    case TY_UNION:
      reg_passable = 0;
      arg->pass_by_stack = 1;
      stack += arg->ty->size;
      break;
      reg_passable = 0;
      arg->pass_by_stack = 1;
      stack += 2;
      break;
    default: // TY_BOOL,CHAR,SHORT,INT,LONG,FLOAT,ENUM,PTR,FUNC,TY_ARRAY,TY_VLA
      if (reg_passable) {
        reg_passable = 0;
        arg->pass_by_stack = 0;
      }else if (node->lhs->ty->is_variadic) {
        arg->pass_by_stack = 1;
        stack += ((arg->ty->kind==TY_ARRAY||arg->ty->kind==TY_VLA))?
		2: (arg->ty->size==1? 2: arg->ty->size);
      }else{
        arg->pass_by_stack = 1;
        stack += ((arg->ty->kind==TY_ARRAY||arg->ty->kind==TY_VLA))?
		2: arg->ty->size;
      }
      break;
    }
  }
  push_args2(node->args,node->lhs->ty->is_variadic);

  // If the return type is a struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument (in Acc A,B).
  // MC6800: all struct/union passes the pointer
  if (node->ret_buffer) { // && node->ty->size > 16)
    println("; return type is struct/union, the caller passes a pointer");
    println("\tldab @bp+1	; %d",node->ret_buffer->offset);
    println("\tldaa @bp");
    if (node->ret_buffer->offset) {
      println("\taddb #<%d",node->ret_buffer->offset);
      println("\tadca #>%d",node->ret_buffer->offset);
    }
  }

  return stack;
}

//
// return struct from local area to *first_arg
//   AccA,B: point to return struct addr
//   var: first arg area
//
static void copy_struct_mem(void) {
  Type *ty = current_fn->ty->return_ty;
//Obj *var = current_fn->params;

  // TODO: memcpy
  println("; copy_struct_mem %s %d",__FILE__,__LINE__);
  println("\tstab @tmp2+1");
  println("\tstaa @tmp2");
  println("\tldab @bp+1");
  println("\tldaa @bp");
  println("\taddb #<%d",current_fn->stack_size);
  println("\tadca #>%d",current_fn->stack_size);
  println("\tstab @tmp3+1");
  println("\tstaa @tmp3");
  println("\tldx @tmp3");
  println("\tldx 0,x");
  println("\tstx @tmp3");
//assume max struct size<256
  for (int i = 0; i < ty->size; i++) {
    println("\tldx @tmp2");
    println("\tldab %d,x",i);
    println("\tldx @tmp3");
    println("\tstab %d,x",i);
  }
  println("\tldab @tmp3+1");
  println("\tldaa @tmp3");
  IX_Dest = IX_None;
}

static void builtin_alloca(void) {
  assert(current_fn->alloca_bottom);

  // Shift the temporary area by %rdi.
  // println("; %%di has alloca size");
  println("\tstab @tmp4+1	; alloca size");
  println("\tstaa @tmp4");
  // println(";	__alloca_bottom__ -> cx");
  // The area between alloca_bottom and SP is the stack currently in use. Move this.
  if (current_fn->alloca_bottom->offset) {
    println("\tldab @bp+1	; IX =  &__alloca_bottom");
    println("\tldaa @bp");
    println("\taddb #<%d",current_fn->alloca_bottom->offset);
    println("\tadca #>%d",current_fn->alloca_bottom->offset);
    tfr_dx();	// tfr_dx uses @tmp1
  }else{
    ldx_bp();
    println("\tstx @tmp1	; save __alloca_bottom");
  }
  println("\tsts @tmp2	; save old SP");	// sp -= alloca size
  println("\tldab @tmp2+1");
  println("\tldaa @tmp2");
  println("\tsubb @tmp4+1");	// alloca size
  println("\tsbca @tmp4");
  println("\tstab @tmp3+1");
  println("\tstaa @tmp3");
  println("\tlds @tmp3	; get new SP");
  int c1 = count();
  int c2 = count();
  // The program moved the stack pointer to implement alloca,
  //     so the original stack data must be copied.
  // Push the data from __alloca_bottom to @tmp2 onto the new SP
  println("\tldx 0,x");		// get old SP bottom
  println("L_%d:",c1);
  println("\tcpx @tmp2");
  println("\tbeq L_%d",c2);
  println("\tldab 0,x");
  println("\tpshb");
  println("\tdex");
  println("\tbra L_%d",c1);
  println("L_%d:",c2);

  println("; Move alloca_bottom pointer.");
  println("\tldx @tmp1");
  IX_Dest = IX_None;
  println("\tldab 1,x	; make new __alloca_bottom__");
  println("\tldaa 0,x");
  println("\tsubb @tmp4+1	; alloca size");
  println("\tsbca @tmp4");
  println("\tstab 1,x");	// return alloca(size);
  println("\tstaa 0,x");
  println(";");
}

//
// If node is a simple expression, it is computed directly without pushing it onto the stack.
//
static bool gen_direct_sub(Node *node,char *opb, char *opa, int test)
{
  int64_t val;

  switch(node->kind){
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:		// TODO: Avoid unnecessary type promotion
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test) return 1;
      if (strcmp(opb,"addb")==0 && node->val==0 ) { // subb used for compare
        return 1;
      }
      if (strcmp(opb,"ldab")==0) {
        if (node->ty->size == 1) {
          ldab_i((uint16_t)node->val);
        }else{
          ldd_i((uint16_t)node->val);
        }
      } else {
        println("\t%s #<%u", opb, (uint16_t)node->val);
        if (opa)
          println("\t%s #>%u", opa, (uint16_t)node->val);
      }
      return 1;
    default:
      return 0;
    }
  } // ND_NUM
  case ND_VAR: {
    if (node->var->ty->kind != TY_VLA ){
      if(node->var->is_local){
        if (node->ty->kind==TY_ARRAY) {
          if (test) return true;
          println("\t%s @bp+1",opb);
          println("\t%s @bp",opa);
          if (strcmp(opb,"addb")==0 && node->var->offset==0) {
            return 1;
          }
          println("\t%s #<%d",opb,node->var->offset);
          println("\t%s #>%d",opa,node->var->offset);
          return 1;
        }
        if (!test_addr_x(node)) return 0;
        int off = gen_addr_x(node,true);
        if (node->ty->kind==TY_CHAR || node->ty->kind==TY_BOOL){
          if (test) {
            return node->ty->is_unsigned;
          }
          println("\t%s %d,x",opb,off);
          if (strcmp(opb,"stab") && opa) {
            println("\t%s #0",opa);
          }
        }else{
          if (test) return 1;
          println("\t%s %d,x",opb,off+1);
          println("\t%s %d,x",opa,off);
        }
        return 1;
      }else{
        // global
        if (node->ty->kind==TY_FUNC)
          return 0;
//      if (node->ty->kind==TY_CHAR && !node->ty->is_unsigned && !opa)
//          return 0;
        if (test) return 1;
        if (node->ty->kind==TY_CHAR || node->ty->kind==TY_BOOL) {
   	      if (!strcmp(opb,"stab")) {
            println("\t%s _%s",opb,node->var->name);
            return 1;
          }
          println("\t%s _%s",opb,node->var->name);
          if (opa)
            println("\t%s #0",opa);
	        return 1;
	      }
        if (node->ty->kind==TY_ARRAY) {
          println("\t%s #<_%s",opb,node->var->name);
          println("\t%s #>_%s",opa,node->var->name);
	        return 1;
        }
        println("\t%s _%s+1",opb,node->var->name);
        println("\t%s _%s",opa,node->var->name);
        return 1;
      }
    }
    return 0;
  } // ND_VAR
  case ND_DEREF:
    switch(node->lhs->kind){
    case ND_CAST:
      if (!is_integer(node->ty) || node->ty->kind==TY_LONG)
        return 0;
      if (node->lhs->ty->kind  == TY_PTR
      &&  node->lhs->lhs->kind == ND_NUM
      &&  is_integer(node->lhs->lhs->ty)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s %ld",opb,node->lhs->lhs->val);
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s %ld+1",opb,node->lhs->lhs->val);
          println("\t%s %ld",opb,node->lhs->lhs->val);
          return 1;
        }
      }
      break;
    case ND_ADD: {
      // global array[const]
      Node *lhs = node->lhs->lhs;
      Node *rhs = node->lhs->rhs;
      int64_t val;
      if (lhs->kind == ND_CAST
      &&  lhs->ty->kind  == TY_PTR
      &&  is_global_array(lhs->lhs)
      &&  rhs->kind == ND_CAST
      &&  rhs->ty->kind == TY_PTR
      &&  is_integer_constant(rhs->lhs,&val)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (val==0) {
            println("\t%s _%s",opb,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->lhs->var->name,val);
          }
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->lhs->var->name);
            println("\t%s _%s",  opa,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->lhs->var->name,val);
            println("\t%s _%s+%ld",  opa,lhs->lhs->var->name,val);
          }
          return 1;
        }
      }
      // (ND_DEREF ty_int (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) perm1 global) 0)
      //                                   lhs                               rhs
      if (is_global_array(lhs)
      &&  is_integer_constant(rhs,&val)) {
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) return 1;
          if (val==0) {
            println("\t%s _%s",opb,lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->var->name,val);
          }
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return 1;
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->var->name);
            println("\t%s _%s",  opa,lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->var->name,val);
            println("\t%s _%s+%ld",  opa,lhs->var->name,val);
          }
          return 1;
        }
      }
    } // ND_ADD
    break;
    } // ND_DEREF → switch ND_ADD:
    return 0;
  case ND_CAST:
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_sub(node->lhs, opb, opa, test))
      return 1;
    if ((node->ty->kind == TY_INT || node->ty->kind == TY_SHORT)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  gen_direct_sub(node->lhs, opb, opa, test)) {
      return 1;
    }
    if (node->ty->kind      == TY_PTR
    &&  gen_direct_sub(node->lhs, opb, opa, test))
      return 1;
    // (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) m +0 )
    if (node->ty->kind == TY_PTR
    &&  node->lhs->kind == ND_VAR
    &&  node->lhs->var->is_local
    &&  node->lhs->ty->kind == TY_ARRAY){
       if (test)
         return 1;
      println("\taddb @bp+1");
      println("\tadca @bp");
      if (node->lhs->var->offset){
        println("\taddb #<%d",node->lhs->var->offset);
        println("\tadca #>%d",node->lhs->var->offset);
      }
      return 1;
    }
    return 0;
  default:
    return 0;
  }
  return 0;
}

bool can_direct(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,1);	// test mode

  return r;
}

bool gen_direct(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0);
}

//
// Commutative 16-bit arithmetic processing
//
int gen_direct_lr(Node *node, char *opb, char *opa)
{
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);

    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if (gen_direct(node->rhs,opb,opa))
        return 1;
      assert(0);
    }
    if (can_direct(node->lhs)){
      gen_expr(node->rhs);
      if (gen_direct(node->lhs,opb,opa))
        return 1;
      assert(0);
    }
    return 0;
}

static int gen_direct_long_and(int64_t v,char *opa, char *opb){
  uint32_t v1 = v & 0x000000FF;
  uint32_t v2 = v & 0x0000FF00;
  uint32_t v3 = v & 0x00FF0000;
  uint32_t v4 = v & 0xFF000000;

  if (v1==0){
    println("\tclr @long+3");
  }else if (v1 != 0x000000FF){
    println("\tldab @long+3");
    println("\t%s #%u", opb, v1);
    println("\tstab @long+3");
  }
  if (v2 == 0){
    println("\tclr @long+2");
  }else if (v2 != 0x0000FF00){
    println("\tldaa @long+2");
    println("\t%s #%u", opa, v2>>8);
    println("\tstaa @long+2");
  }
  if (v3 == 0){
    println("\tclr @long+1");
  }else if (v3 != 0x00FF0000){
    println("\tldaa @long+1");
    println("\t%s #%u", opa, v3>>16);
    println("\tstaa @long+1");
  }
  if (v4 == 0){
    println("\tclr @long");
  }else if (v4 != 0xFF000000){
    println("\tldaa @long");
    println("\t%s #%u", opa, v4>>24);
    println("\tstaa @long");
  }

  return 1;
}

static int gen_direct_long_or(uint64_t v,char *opa, char *opb){
  uint32_t v1 = v & 0x000000FF;
  uint32_t v2 = v & 0x0000FF00;
  uint32_t v3 = v & 0x00FF0000;
  uint32_t v4 = v & 0xFF000000;
  bool b_is_ff = 0;

  if (v1==0x000000FF) {
    println("\tldab #$FF");
    println("\tstab @long+3");
    b_is_ff = 1;
  }else if (v1) {
    println("\tldab @long+3");
    println("\t%s #%u", opb, v1);
    println("\tstab @long+3");
  }
  if (v2 == 0x0000FF00){
    if (!b_is_ff) {
      println("\tldab #$FF");
      b_is_ff = 1;
    }
    println("\tstab @long+2");
  }else if (v2) {
    println("\tldaa @long+2");
    println("\t%s #%u", opa, v2>>8);
    println("\tstaa @long+2");
  }
  if (v3 == 0x00FF0000){
    if (!b_is_ff) {
      println("\tldab #$FF");
      b_is_ff = 1;
    }
    println("\tstab @long+1");
  }else if (v3) {
    println("\tldaa @long+1");
    println("\t%s #%u", opa, v3>>16);
    println("\tstaa @long+1");
  }
  if (v4 == 0xFF000000){
    if (!b_is_ff) {
      println("\tldab #$FF");
      b_is_ff = 1;
    }
    println("\tstab @long");
  }else if (v4) {
    println("\tldaa @long");
    println("\t%s #%u", opa, v4>>24);
    println("\tstaa @long");
  }

  return 1;
}
static int gen_direct_long_xor(uint64_t v,char *opa, char *opb){
  uint32_t v1 = v & 0x000000FF;
  uint32_t v2 = v & 0x0000FF00;
  uint32_t v3 = v & 0x00FF0000;
  uint32_t v4 = v & 0xFF000000;

  if (v1) {
    println("\tldab @long+3");
    println("\t%s #%u", opb, v1);
    println("\tstab @long+3");
  }
  if (v2) {
    println("\tldaa @long+2");
    println("\t%s #%u", opa, v2>>8);
    println("\tstaa @long+2");
  }
  if (v3) {
    println("\tldaa @long+1");
    println("\t%s #%u", opa, v3>>16);
    println("\tstaa @long+1");
  }
  if (v4) {
    println("\tldaa @long");
    println("\t%s #%u", opa, v4>>24);
    println("\tstaa @long");
  }

  return 1;
}
//
//
// shift operation
//
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
    IX_Dest = IX_None;
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
    IX_Dest = IX_None;
    return 1;
  case 8:
    println("\tldx @long+1");
    println("\tstx @long");
    println("\tldab @long+3");
    println("\tstab @long+2");
    println("\tclr @long+3");
    IX_Dest = IX_None;
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
      int c = count();
      println("\ttst @long");
      println("\tbpl L_%d",c);
      println("\tdeca");
      println("L_%d:",c);
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
    IX_Dest = IX_None;
    return 1;
  case 8:
    println("\tldx @long+1");
    println("\tstx @long+2");
    println("\tldab @long");
    println("\tstab @long+1");
    println("\tstaa @long");
    IX_Dest = IX_None;
    return 1;
  }
  return 0;
}

//
// long version:
// If rhs is a simple expression, it is computed directly without pushing it onto the stack.
//
static int gen_direct_long_sub(Node *rhs,char *opb, char *opa, int test)
{
  if (opt_O == 's')
    return 0;
  switch(rhs->kind){
  case ND_NUM: {
    switch (rhs->ty->kind) {
    case TY_LONG:
      if (test)
        return 1;
      // TODO: Branching with strcmp is not pretty. think it later.
      if (strcmp(opb,"andb")==0)
        return gen_direct_long_and(rhs->val,opa,opb);
      if (strcmp(opb,"orab")==0)
        return gen_direct_long_or(rhs->val,opa,opb);
      if (strcmp(opb,"eorb")==0)
        return gen_direct_long_xor(rhs->val,opa,opb);
      println("\tldab @long+3");
      println("\t%s #%u", opb, (uint32_t) (rhs->val & 0x000000FF));
      println("\tstab @long+3");
      println("\tldaa @long+2");
      println("\t%s #%u", opa, (uint32_t)((rhs->val & 0x0000FF00)>>8));
      println("\tstaa @long+2");
      println("\tldaa @long+1");
      println("\t%s #%u", opa, (uint32_t)((rhs->val & 0x00FF0000)>>16));
      println("\tstaa @long+1");
      println("\tldaa @long");
      println("\t%s #%u", opa, (uint32_t)((rhs->val & 0xFF000000)>>24));
      println("\tstaa @long");
      return 1;
    default:
      return 0;
    }
  } // ND_NUM
  case ND_VAR: {
    if (rhs->var->ty->kind != TY_VLA){
      if (!test_addr_x(rhs)) return 0;
      if (rhs->var->is_local){
        if (test) return 1;
        int off = gen_addr_x(rhs,true);
        println("\tldab @long+3");
        println("\t%s %d,x",opb,off+3);
        println("\tstab @long+3");
        println("\tldaa @long+2");
        println("\t%s %d,x",opa,off+2);
        println("\tstaa @long+2");
        println("\tldaa @long+1");
        println("\t%s %d,x",opa,off+1);
        println("\tstaa @long+1");
        println("\tldaa @long");
        println("\t%s %d,x",opa,off);
        println("\tstaa @long");
        return 1;
      }else{ // global
        if (test) return 1;
        println("\tldx #_%s",rhs->var->name);
        IX_Dest = IX_None;
        println("\tldab @long+3");
        println("\t%s 3,x",opb);
        println("\tstab @long+3");
        println("\tldaa @long+2");
        println("\t%s 2,x",opa);
        println("\tstaa @long+2");
        println("\tldaa @long+1");
        println("\t%s 1,x",opa);
        println("\tstaa @long+1");
        println("\tldaa @long");
        println("\t%s 0,x",opa);
        println("\tstaa @long");
        return 1;
      }
    }
    return 0;
  } // ND_VAR
  case ND_CAST:
    if(is_empty_cast(rhs->lhs->ty, rhs->ty))
      return gen_direct_long_sub(rhs->lhs, opb, opa, test);
  default:
    if (test_addr_x(rhs)) {
      if (test) return 1;
      int off = gen_addr_x(rhs,false);
      println("\tldab @long+3");
      println("\t%s %d,x",opb,off+3);
      println("\tstab @long+3");
      println("\tldaa @long+2");
      println("\t%s %d,x",opa,off+2);
      println("\tstaa @long+2");
      println("\tldaa @long+1");
      println("\t%s %d,x",opa,off+1);
      println("\tstaa @long+1");
      println("\tldaa @long");
      println("\t%s %d,x",opa,off);
      println("\tstaa @long");
      return 1;
    }
    return 0;
  }
  return 0;
}

static int can_direct_long(Node *rhs)
{
  int r = gen_direct_long_sub(rhs,NULL,NULL,1);	// test mode

  return r;
}

static int gen_direct_long(Node *rhs,char *opb, char *opa)
{
  return gen_direct_long_sub(rhs,opb,opa,0);
}

//
int gen_direct_long2(Node *node,char *opb, char *opa)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int L =  ((lhs->kind == ND_NUM) && (lhs->ty->kind == TY_LONG))*2
        +  ((lhs->kind == ND_VAR) && test_addr_x(lhs) &&  lhs->var->is_local);
  int R =  ((rhs->kind == ND_NUM) && (rhs->ty->kind == TY_LONG))*2
        +  ((rhs->kind == ND_VAR) && test_addr_x(rhs) &&  rhs->var->is_local);
  int loff = 0;
  int roff = 0;
  uint64_t lv;
  uint64_t rv;

//
//  If both L and R are local variables, IX is @bp for both, so this is fine.
//  Should IX change in the future, this could cause problems.
//
  if (L==1)
    loff = gen_addr_x(lhs,false);
  else
    lv   = lhs->val;
  if (R==1)
    roff = gen_addr_x(rhs,false);
  else
    rv   = rhs->val;
  
  if (L==1)
    println("\tldab %d,x",loff+3);
  else
    println("\tldab #%d",(int)(lv & 0x000000FF));
  if (R==1)
    println("\t%s %d,x",opb,roff+3);
  else
    println("\t%s #%d",opb,(int)(rv & 0x000000FF));
  println("\tstab @long+3");
  if (L==1)
    println("\tldaa %d,x",loff+2);
  else
    println("\tldaa #%d",(int)(lv & 0x0000FF00)>>8);
  if (R==1)
    println("\t%s %d,x",opa,roff+2);
  else
    println("\t%s #%d",opa,(int)(rv & 0x0000FF00)>>8);
  println("\tstaa @long+2");
  if (L==1)
    println("\tldaa %d,x",loff+1);
  else
    println("\tldaa #%d",(int)(lv & 0x00FF0000)>>16);
  if (R==1)
    println("\t%s %d,x",opa,roff+1);
  else
    println("\t%s #%d",opa,(int)(rv & 0x00FF0000)>>16);
  println("\tstaa @long+1");
  if (L==1)
    println("\tldaa %d,x",loff);
  else
    println("\tldaa #%d",(int)(lv & 0xFF000000)>>24);
  if (R==1)
    println("\t%s %d,x",opa,roff);
  else
    println("\t%s #%d",opa,(int)(rv & 0xFF000000)>>24);
  println("\tstaa @long");
  return 1;
}

int can_direct_long2(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  int L =  ((lhs->kind == ND_NUM) && (lhs->ty->kind == TY_LONG))*2
        +  ((lhs->kind == ND_VAR) && test_addr_x(lhs) &&  lhs->var->is_local);
  int R =  ((rhs->kind == ND_NUM) && (rhs->ty->kind == TY_LONG))*2
        +  ((rhs->kind == ND_VAR) && test_addr_x(rhs) &&  rhs->var->is_local);
  if (!L || !R){
    return 0;
  }
  if (L==1 && !test_addr_x(lhs))
    return 0;
  if (R==1 && !test_addr_x(lhs))
    return 0;

  return 1;
}

static int isVARorNUM(Node *node)
{
  return (node->kind==ND_VAR || node->kind==ND_NUM);
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

static void gen_funcall(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "alloca")) {
    gen_expr(node->args);
    println("; ND_FUNCALL: call builtin_alloca()  %s %d",__FILE__,__LINE__);
    builtin_alloca();
    return;
  }
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "__builtin_va_start_addr")) {
    println("; __builtin_va_start_addr: current_fn->params");
    for (Obj *var = current_fn->params; var; var = var->next) {
      println("; '%s', kind:%d",var->name, var->ty->kind);
    }
    println(";__builtin_va_start_addr(var)");
    for (Node *arg = node->args; arg && arg->var; arg = arg->next) {
      println("; '%s', kind:%d",arg->var->name, arg->ty->kind);
    }
    if (!current_fn->ty->is_variadic) {
      error_tok(node->tok, "__builtin_va_start_addr: not variadic function");
    }
    if (node->args->kind != ND_VAR
    ||  !node->args->var->is_local
    ||  node->args->var->ty->kind == TY_VLA) {
      error_tok(node->tok, "__builtin_va_start_addr: bad arg");
    }
    // var is the first parameter and pass-by-register ?
    int passed_by_reg = 0;
    if (!strcmp(current_fn->params->name,node->args->var->name)) {
      passed_by_reg = 1;
      switch (current_fn->params->ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
      case TY_DOUBLE:
      case TY_LDOUBLE:
        passed_by_reg = 0;
      }
      if (current_fn->ty->return_ty->kind == TY_STRUCT
      ||  current_fn->ty->return_ty->kind == TY_UNION  ){
        passed_by_reg = 0;
      }
    }
    // If the specified variable is passed in a register,
    //    the remaining variable arguments (varargs) are located
    //    at the top of the stack.
    // If the specified variable is passed on the stack, 
    //   the remaining variable arguments (varargs) reside in the stack memory
    //   starting at its address plus the size of the variable.
    int next_offset = node->args->var->offset
	            + node->args->var->ty->size
		    + (passed_by_reg? 4: 0);	// skip @bp and return address
    println("\tldab @bp+1");
    println("\tldaa @bp");
    println("\taddb #<%d",next_offset);
    println("\tadca #>%d",next_offset);
    return;
  }

  int stack_args = push_args(node);

  if (node->lhs->kind == ND_VAR && node->lhs->ty->kind == TY_FUNC){
    println("\tjsr _%s",node->lhs->var->name);
  }else if (test_expr_x(node->lhs)) { // TODO: gen_expr_x
    int off = gen_expr_x(node->lhs,true);
    println("\tjsr %d,x",off);
  }else{
    if (node->args && !node->args->pass_by_stack) {
      switch (node->args->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\tpshb");
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_PTR:
      case TY_ENUM:
        println("\tpshb");
        println("\tpsha");
        break;
      case TY_LONG:
      case TY_FLOAT:
        println("\tpush32");
        break;
      default:
        assert(0);
      }
    }else if (node->ret_buffer) {
      println("\tpshb");
      println("\tpsha");
    }
    gen_expr(node->lhs);
    println("\tstab @tmp1+1");
    println("\tstaa @tmp1");
    if (node->args && !node->args->pass_by_stack) {
      switch (node->args->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\tpulb");
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_PTR:
      case TY_ENUM:
        println("\tpula");
        println("\tpulb");
        break;
      case TY_LONG:
      case TY_FLOAT:
        println("\tpop32");
        break;
      default:
        assert(0);
      }
    }else if (node->ret_buffer) {
      println("\tpula");
      println("\tpulb");
    }
    println("\tldx @tmp1");
    println("\tjsr 0,x");
  }
  IX_Dest = IX_None;
  
  // Removes pushed arguments before calling a function for speed
  if (opt_O == 's' && stack_args>4) {         // Smaller, but too slow
    println("\tjsr __ins_i");
    if (stack_args>255) {
      assert(0);
    }
    println("\t.byte  %d",stack_args);
    depth-=stack_args;
  }else if (stack_args>20 ||                       // Both faster and smaller
           (opt_O != 's' && stack_args*4 > 34)) {  // Faster, but larger
    println("; ins*%d",stack_args);
    println("\tstaa @tmp2");		// 4
    println("\tsts @tmp1");		// 5
    println("\tldaa @tmp1+1");	// 3
    println("\tadda #<%d",stack_args);// 2
    println("\tstaa @tmp1+1"); 	// 4
    println("\tldaa @tmp1");		// 3
    println("\tadca #>%d",stack_args);// 2
    println("\tstaa @tmp1");		// 4
    println("\tlds @tmp1");		// 4
    println("\tldaa @tmp2");		// 3
    depth-=stack_args;
  }else{
    ins(stack_args);
  }
  // If the return value is a type shorter than an int,
  // the upper bytes contain garbage, so we correct it.
#if 0
  switch (node->ty->kind) {
  case TY_BOOL:
    println("\tclra");
    return;
  case TY_CHAR:
    println("\tclra");
    if (!node->ty->is_unsigned){
      println("\tasrb");
      println("\trolb");
      println("\tsbca #0");
    }
    return;
  }
#endif
  return;
}

static void opeq(Node *node)
{
  int64_t val;

  switch(node->kind){
  case ND_ADDEQ: {
    switch(node->ty->kind) {
    case TY_FLOAT:
      gen_addr(node->lhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tjsr __push32x");
      IX_Dest = IX_None;
      gen_expr(node->rhs);
      println("\tjsr __addf32tos");
      break;
    case TY_LONG:
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\tldx 0,x");
      IX_Dest = IX_None;
      println("\tjsr __add32x");
      break;
    case TY_BOOL:
    case TY_CHAR:
      if (node->lhs->ty->is_unsigned && test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        int off = gen_addr_x(node->lhs,true);
        println("\taddb %d,x",off);
        println("\tadca #0");
        println("\tstab %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\tldx 0,x");
      println("\taddb 0,x");
      println("\tadca #0");
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        int off = gen_addr_x(node->lhs,true);
        println("\taddb %d,x",off+1);
        println("\tadca %d,x",off);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\tldx 0,x");
      println("\taddb 1,x");
      println("\tadca 0,x");
      break;
    default:
      assert(0);
    }
    IX_Dest = IX_None;
    store(node->ty);
    return;
  }
  case ND_SUBEQ: {
    switch(node->ty->kind) {
    case TY_FLOAT:
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\tjsr __push32");
      println("\ttsx");
      println("\tldx 4,x");
      println("\tjsr __load32x");
      IX_Dest = IX_None;
      println("\tjsr __subf32tos");
      break;
    case TY_LONG:
      gen_addr(node->lhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tjsr __push32x");
      IX_Dest = IX_None;
      gen_expr(node->rhs);
      println("\tjsr __sub32tos");
      break;
    case TY_BOOL:
    case TY_CHAR:
      if (node->lhs->ty->is_unsigned && test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        negd();
        int off = gen_addr_x(node->lhs,true);
        println("\taddb %d,x",off);
        println("\tadca #0");
        println("\tstab %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      negd();
      println("\ttsx");
      println("\tldx 0,x");
      println("\taddb 0,x");
      println("\tadca #0");
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test_addr_x(node->lhs)) {
        if (is_integer_constant(node->rhs,&val)) {
          int off = gen_addr_x(node->lhs,true);
          println("\tldab %d,x",off+1);
          println("\tldaa %d,x",off);
          println("\tsubb #<%ld",val);
          println("\tsbca #>%ld",val);
          println("\tstab %d,x",off+1);
          println("\tstaa %d,x",off);
          IX_Dest = IX_None;
          return;
        }
        gen_expr(node->rhs);
        negd();
        int off = gen_addr_x(node->lhs,true);
        println("\taddb %d,x",off+1);
        println("\tadca %d,x",off);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      negd();
      println("\ttsx");
      println("\tldx 0,x");
      println("\taddb 1,x");
      println("\tadca 0,x");
      break;
    default:
      assert(0);
    }
    store(node->ty);
    return;
  }
  case ND_MULEQ: {
    gen_addr(node->lhs);
    push();
    switch(node->ty->kind) {
    case TY_FLOAT:
      println("\ttsx");
      println("\tldx 0,x");
      println("\tjsr __push32x");
      IX_Dest = IX_None;
      gen_expr(node->rhs);
      println("\tjsr __mulf32tos");
      break;
    case TY_LONG:
      println("\ttsx");
      println("\tldx 0,x");
      println("\tjsr __push32x");
      IX_Dest = IX_None;
      gen_expr(node->rhs);
      println("\tjsr __mul32tos");
      break;
    case TY_BOOL:
    case TY_CHAR: 
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 0,x");
      println("\tclra");
      IX_Dest = IX_None;
      push();
      gen_expr(node->rhs);
      println("\tjsr __mul16x16");
      ins(2);
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 1,x");
      println("\tldaa 0,x");
      IX_Dest = IX_None;
      push();
      gen_expr(node->rhs);
      println("\tjsr __mul16x16");
      ins(2);
      break;
    default:
      assert(0);
    }
    store(node->ty);
    return;
  }
  case ND_DIVEQ: {
    gen_addr(node->lhs);
    push();
    switch(node->ty->kind) {
    case TY_FLOAT:
      gen_expr(node->rhs);
      pushl();
      println("\ttsx");
      println("\tldx 4,x");
      println("\tjsr __load32x");
      IX_Dest = IX_None;
      println("\tjsr __divf32tos");
      depth -= 4;
      break;
    case TY_LONG:
      gen_expr(node->rhs);
      pushl();
      println("\ttsx");
      println("\tldx 4,x");
      println("\tjsr __load32x");
      if (node->ty->is_unsigned) {
        println("\tjsr __div32x32u");
      }else{
        println("\tjsr -_div32x32s");
      }
      depth -= 4;
      IX_Dest = IX_None;
      break;
    case TY_BOOL:
    case TY_CHAR: 
      gen_expr(node->rhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 0,x");
      println("\tclra");
      if (node->ty->is_unsigned) {
        println("\tjsr __div16x16u");
      }else{
        println("\tjsr __div16x16s");
      }
      ins(2);
      IX_Dest = IX_None;
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      gen_expr(node->rhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 1,x");
      println("\tldaa 0,x");
      if (node->ty->is_unsigned) {
        println("\tjsr __div16x16u");
      }else{
        println("\tjsr __div16x16s");
      }
      ins(2);
      IX_Dest = IX_None;
      break;
    default:
      assert(0);
    }
    IX_Dest = IX_None;
    store(node->ty);
    return;
  }
  case ND_MODEQ: {
    gen_addr(node->lhs);
    push();
    switch(node->ty->kind) {
    case TY_LONG:
      gen_expr(node->rhs);
      pushl();
      println("\ttsx");
      println("\tldx 4,x");
      println("\tjsr __load32x");
      if (node->ty->is_unsigned) {
        println("\tjsr __rem32x32u");
      }else{
        println("\tjsr __rem32x32s");
      }
      depth -= 4;
      IX_Dest = IX_None;
      break;
    case TY_BOOL:
    case TY_CHAR: 
      gen_expr(node->rhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 0,x");
      println("\tclra");
      if (node->ty->is_unsigned) {
        println("\tjsr __rem16x16u");
      }else{
        println("\tjsr __rem16x16s");
      }
      ins(2);
      IX_Dest = IX_None;
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      gen_expr(node->rhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 1,x");
      println("\tldaa 0,x");
      if (node->ty->is_unsigned) {
        println("\tjsr __rem16x16u");
      }else{
        println("\tjsr __rem16x16s");
      }
      ins(2);
      IX_Dest = IX_None;
      break;
    default:
      assert(0);
    }
    IX_Dest = IX_None;
    store(node->ty);
    return;
  }
  case ND_ANDEQ:
  case ND_OREQ:
  case ND_XOREQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      pushl();
      println("\ttsx");
      println("\tldx 4,x");
      println("\tjsr __load32x");
      switch(node->kind) {
      case ND_ANDEQ:
        println("\tjsr __and32tos");
        break;
      case ND_OREQ:
        println("\tjsr __or32tos");
        break;
      case ND_XOREQ:
        println("\tjsr __xor32tos");
        break;
      }
      depth -= 4;
      IX_Dest = IX_None;
      break;
    case TY_BOOL:
    case TY_CHAR: 
      if (node->lhs->ty->is_unsigned && test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        int off = gen_addr_x(node->lhs,true);
        switch(node->kind) {
        case ND_ANDEQ:
          println("\tclra");
          println("\tandb %d,x",off);
          break;
        case ND_OREQ:
          println("\torab %d,x",off);
          break;
        case ND_XOREQ:
          println("\teorb %d,x",off);
          break;
        default:
          assert(0);
        }
        println("\tstab %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\tldx 0,x");
      switch (node->kind) {
      case ND_ANDEQ:
        println("\tandb 0,x");
        break;
      case ND_OREQ:
        println("\torab 0,x");
        break;
      case ND_XOREQ:
        println("\teorb 0,x");
        break;
      }
      println("\tclra");
      IX_Dest = IX_None;
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      if (test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        int off = gen_addr_x(node->lhs,true);
        switch(node->kind) {
        case ND_ANDEQ:
          println("\tandb %d,x",off+1);
          println("\tanda %d,x",off);
          break;
        case ND_OREQ:
          println("\torab %d,x",off+1);
          println("\toraa %d,x",off);
          break;
        case ND_XOREQ:
          println("\teorb %d,x",off+1);
          println("\teora %d,x",off);
          break;
        default:
          assert(0);
        }
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\tldx 0,x");
      switch (node->kind) {
      case ND_ANDEQ:
        println("\tandb 1,x");
        println("\tanda 0,x");
        break;
      case ND_OREQ:
        println("\torab 1,x");
        println("\toraa 0,x");
        break;
      case ND_XOREQ:
        println("\teorb 1,x");
        println("\teora 0,x");
        break;
      }
      IX_Dest = IX_None;
      break;
    default:
      error_tok(node->tok,"invalid operand &,|,^");
      assert(0);
    }
    IX_Dest = IX_None;
    store(node->ty);
    return;
  }
  case ND_SHREQ:
  case ND_SHLEQ: {
    int64_t val;

    switch(node->ty->kind) {
    case TY_LONG:
      if (is_global_var(node->lhs)) {
        gen_expr(node->rhs);
        println("\tldx #_%s",node->lhs->var->name);
        println("\tjsr __load32x");
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl32");
        }else if (node->lhs->ty->is_unsigned) {
          println("\tjsr __shr32u");
        }else{
          println("\tjsr __shr32s");
        }
        IX_Dest = IX_None;
        println("\tldx #_%s",node->lhs->var->name);
        store_x(node->ty,0);
        return;
      }else{
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        push1();
        println("\ttsx");
        println("\tldx 1,x");
        println("\tjsr __load32x");
        pop1();
      }
      if (node->kind == ND_SHLEQ) {
        println("\tjsr __shl32");
      }else if (node->lhs->ty->is_unsigned) {
        println("\tjsr __shr32u");
      }else{
        println("\tjsr __shr32s");
      }
      break;
    case TY_BOOL:
    case TY_CHAR: 
      if (test_addr_x(node->lhs)) {
        if (is_integer_constant(node->rhs, &val)){
          int off = gen_addr_x(node->lhs,true);
          println("\tldab %d,x",off);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            if (gen_shl(node->lhs->ty,val)) {
              println("\tstab %d,x",off);
              return;
            }
          }else if (gen_shr(node->lhs->ty,val)) {
            println("\tstab %d,x",off);
            return;
          }
          println("\tclrb");
          return;
        }
        gen_expr(node->rhs);
        push1();
        int off = gen_addr_x(node->lhs,true);
        println("\tldab %d,x",off);
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl16");
        }else if (node->lhs->ty->is_unsigned) {
          println("\tjsr __shr16u");
        }else{
          println("\tjsr __shr16s");
        }
        println("\tstab %d,x",off);
        ins(1);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs); 
      push();
      gen_expr(node->rhs);
      push1();
      println("\ttsx");
      println("\tldx 1,x");
      println("\tldab 0,x");
      println("\tclra");
      if (node->kind == ND_SHLEQ) {
        println("\tjsr __shl16");
      }else if (node->lhs->ty->is_unsigned) {
        println("\tjsr __shr16u");
      }else{
        println("\tjsr __shr16s");
      }
      println("\tstab 0,x");
      ins(1);
      IX_Dest = IX_None;
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      if (test_addr_x(node->lhs)) {
        if (is_integer_constant(node->rhs, &val)){
          int off = gen_addr_x(node->lhs,true);
          println("\tldab %d,x",off+1);
          println("\tldaa %d,x",off);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            if (gen_shl(node->lhs->ty,val)) {
              println("\tstab %d,x",off+1);
              println("\tstaa %d,x",off);
//            cast(node->lhs->ty, node->ty);
              return;
            }
          }else if (gen_shr(node->lhs->ty,val)) {
            println("\tstab %d,x",off+1);
            println("\tstaa %d,x",off);
//          cast(node->lhs->ty, node->ty);
            return;
          }
          println("\tclrb");
          println("\tclra");
          return;
        }
        gen_expr(node->rhs);
        push1();
        int off = gen_addr_x(node->lhs,true);
        println("\tldab %d,x",off+1);
        println("\tldaa %d,x",off);
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl16");
        }else if (node->lhs->ty->is_unsigned) {
          println("\tjsr __shr16u");
        }else{
          println("\tjsr __shr16s");
        }
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        ins(1);
        IX_Dest = IX_None;
        return;
      }
      gen_addr(node->lhs); 
      push();
      gen_expr(node->rhs);
      push1();
      println("\ttsx");
      println("\tldx 1,x");
      println("\tldab 1,x");
      println("\tldaa 0,x");
      if (node->kind == ND_SHLEQ) {
        println("\tjsr __shl16");
      }else if (node->lhs->ty->is_unsigned) {
        println("\tjsr __shr16u");
      }else{
        println("\tjsr __shr16s");
      }
      ins(1);
      IX_Dest = IX_None;
      break;
    default:
      assert(0);
    }
    IX_Dest = IX_None;
    store(node->ty);
    return;
  }
  default:
    assert(0);
  }
}

// Generate code for a given node.
void gen_expr(Node *node) {
  node = optimize_expr(node);
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;
  int64_t val;

  switch (node->kind) {
  case ND_NULL_EXPR:
    return;
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT: {
      union { float f32; uint32_t u32; } u = { node->fval };
      load32i(u.u32);
      return;
    }
    case TY_DOUBLE:
    case TY_LDOUBLE: {
      error_tok(node->tok, "gen_expr: double not implemented yet");
      return;
    }
    case TY_BOOL:
    case TY_CHAR:
      if(node->val==0){
        println("\tclrb");
      }else{
        println("\tldab #<%u", (uint16_t)node->val);
      }
      return;
    case TY_INT:
    case TY_SHORT:
    case TY_PTR:
    case TY_ENUM:
      ldd_i((uint16_t)node->val);
      return;
    case TY_LONG:
      load32i(node->val);
      return;
    }
    error_tok(node->tok, "gen_expr: not implemented yet token");
    return;
  }
  case ND_POST_INCDEC: {
    if (node->rhs->kind != ND_NUM){
      assert(0);
    }
    int val = node->rhs->val;
    int off;
    if (test_addr_x(node->lhs)){
      off = gen_addr_x(node->lhs,false);
    }else{
      off = 0;
      gen_addr(node->lhs);
      tfr_dx();
    }
    switch (node->lhs->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
      if (node->retval_unused) {
        switch(val){
        case 1:
          println("\tinc %d,x",off);
          break;
        case -1:
          println("\tdec %d,x",off);
          break;
        default:
          println("\tldab %d,x",off);
          println("\taddb #%d",val);
          println("\tstab %d,x",off);
          break;
        }
      }else{
          switch(val){
          case 1:
            println("\tldab %d,x",off);
            println("\tincb");
            println("\tstab %d,x",off);
            println("\tdecb");
            break;
          case -1:
            println("\tldab %d,x",off);
            println("\tdecb");
            println("\tstab %d,x",off);
            println("\tincb");
            break;
          default:
            println("\tldab %d,x",off);
            println("\taddb #%d",val);
            println("\tstab %d,x",off);
            println("\tsubb #%d",val);
            break;
          }
      }
      break;
      // TY_BOOL, TY_CHAR
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (node->retval_unused && val==1) {
        char *label = new_label("L_%d");
        println("\tinc %d,x",off+1);
        println("\tbne %s",label);
        println("\tinc %d,x",off);
        println("%s:",label);
      }else if (val>0) {
        println("\tldab %d,x",off+1);
        println("\tldaa %d,x",off);
        println("\taddb #<%d",val);
        println("\tadca #>%d",val);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        if (!node->retval_unused) {
          println("\tsubb #<%d",val);
          println("\tsbca #>%d",val);
        }
      } else { // val<0
        val = abs(val);
        println("\tldab %d,x",off+1);
        println("\tldaa %d,x",off);
        println("\tsubb #<%d",val);
        println("\tsbca #>%d",val);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        if (!node->retval_unused) {
          println("\taddb #<%d",val);
          println("\tadca #>%d",val);
        }
      }
      break;
    default:
      assert(0);
    }
    return;
  } // ND_POST_INCDEC
  case ND_PRE_INCDEC: {
    if (node->rhs->kind != ND_NUM){
      assert(0);
    }
    int val = node->rhs->val;
    int off;
    if (test_addr_x(node->lhs)){
      off = gen_addr_x(node->lhs,false);
    }else{
      off = 0;
      gen_addr(node->lhs);
      tfr_dx();
    }
    switch (node->lhs->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
      if (node->retval_unused) {
        switch(val){
        case 1:
          println("\tinc %d,x",off);
          break;
        case -1:
          println("\tdec %d,x",off);
          break;
        default:
          println("\tldab %d,x",off);
          println("\taddb #%d",val);
          println("\tstab %d,x",off);
          break;
        }
      }else{
        println("\tldab %d,x",off);
        switch(val){
        case 1:
          println("\tincb");
          break;
        case -1:
          println("\tdecb");
          break;
        default:
          println("\taddb #%d",val);
          break;
        }
        println("\tstab %d,x",off);
      }
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (node->retval_unused && val==1) {
        char *label = new_label("L_%d");
        println("\tinc %d,x",off+1);
        println("\tbne %s",label);
        println("\tinc %d,x",off);
        println("%s:",label);
      }else if (val>0) {
        println("\tldab %d,x",off+1);
        println("\tldaa %d,x",off);
        println("\taddb #<%d",val);
        println("\tadca #>%d",val);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
      }else{ // val<0
        val = abs(val);
        println("\tldab %d,x",off+1);
        println("\tldaa %d,x",off);
        println("\tsubb #<%d",val);
        println("\tsbca #>%d",val);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
      }
      break;
    default:
      assert(0);
    }
    return;
  } // ND_PRE_INCDEC
  case ND_NEG:
    gen_expr(node->lhs);

    switch (node->ty->kind) {
    case TY_FLOAT:
      println("\tldab @long	; negate float");
      println("\teorb #$80");
      println("\tstab @long");
      return;
    case TY_DOUBLE:
    case TY_LDOUBLE:
      error_tok(node->tok, "gen_expr: double not implemented yet");
      return;
    case TY_LONG:
      println("\tjsr __neg32");
      IX_Dest = IX_None;
      return;
    case TY_CHAR:
      println("\tnegb");
      return;
    }
    negd();
    return;
  //   ND_NEG end
  case ND_VAR:
    load_var(node);
    return;
  case ND_MEMBER: {
    if (can_load_x(node->ty) && test_addr_x(node)) {
      off = gen_addr_x(node,false);
      load_x(node->ty,off);
    }else{
      gen_addr(node);
      load(node->ty);
    }

    Member *mem = node->member;
    if (mem->is_bitfield) {
      println("; bitfield mem->bit_width=%d, mem->bit_offset=%d, %s %d",
		      mem->bit_width, mem->bit_offset, __FILE__, __LINE__);
      println(";  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);
      gen_shr(ty_uint, mem->bit_offset);
      unsigned int mask = (unsigned int)(1L << mem->bit_width) - 1;
      and_i(mask & 0xffff);
      if (!mem->ty->is_unsigned && mem->bit_width!=16) {
        char *label = new_label("L_%d");
        if (mem->bit_width<=8)  {
          println("\tbitb #<$%04x",(int)(1L << (mem->bit_width-1)));
        }else{
          println("\tbita #<$%04x",(int)(1L << (mem->bit_width-9)));
        }
        println("\tbeq %s",label);
        if (mem->bit_width<=8)  {
          if (mem->bit_width!=8) {
            println("\torb #<$%04x",(~mask)&0xffff);
          }
          println("\tora #$ff");
        }else{
          println("\tora #>$%04x",(~mask)&0xffff);
        }
        println("%s:",label);
      }
    }
    return;
  }
  case ND_DEREF: {
    Node *lhs = node->lhs;
    // TODO: global var deref
    if (can_direct(node)) {
      gen_direct(node,"ldab","ldaa");
      return;
    }
    if (can_load_x(node->ty) && test_expr_x(lhs)){
      int off = gen_expr_x(lhs,false);
      load_x(node->ty,off);
      return;
    }
    gen_expr(lhs);
    load(node->ty);
    return;
  } // ND_DEREF:
  case ND_ADDR:
    gen_addr(node->lhs);
    return;
  case ND_ASSIGN: {
    Type    *ty;
    int64_t val;

    node = optimize_expr(node);
    Node *lhs = node->lhs;
    Node *rhs = node->rhs;

    if (node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield) {
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      popx();
      if (!node->retval_unused){
        push();
      }

      // If the lhs is a bitfield, we need to read the current value
      // from memory and merge it with a new value.
      Member *mem = node->lhs->member;
      println("; bitfieled node->ty->size=%d, mem->bit_width=%d, mem->bit_offset=%d, %s %d",
		      node->ty->size,mem->bit_width, mem->bit_offset, __FILE__, __LINE__);
      and_i((unsigned short)(1L << mem->bit_width) - 1);
      gen_shl(ty_uint,mem->bit_offset);
      uint16_t mask = ((1L << mem->bit_width) - 1) << mem->bit_offset;
      println("\teorb 1,x");
      println("\teora 0,x");
      and_i(mask);
      println("\teorb 1,x");
      println("\teora 0,x");
      store_x(node->ty,0);
      if (!node->retval_unused){
        pop();
      }
      return;
    }
    if (can_direct(lhs) && lhs->ty->size <= 2) {
      gen_expr(rhs);
      gen_direct(lhs,"stab","staa");
      return;
    }
    if (node->lhs->kind == ND_DEREF
    &&  is_integer(node->lhs->ty)
    &&  node->lhs->ty->size <= 2
    &&  node->lhs->lhs->kind == ND_CAST
    &&  node->lhs->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->lhs->lhs,&val)) {
      gen_expr(node->rhs);
      gen_direct(node->lhs,"stab","staa");
      return;
    }
    if (is_global_var(node->lhs)
    &&  node->ty->size <= 2
    &&  can_direct(node->lhs)) {
      gen_expr(node->rhs);
      gen_direct(node->lhs,"stab","staa");
      return;
    }
    if ((ty = is_integer_constant(node->rhs,&val))
    &&  ty->size <= 2
    &&  is_integer(node->ty)
    &&  node->ty->size <= 2) {
      if (can_direct(node->lhs)) {
        gen_direct(node->rhs,"ldab","ldaa");
        gen_direct(node->lhs,"stab","staa");
      }else if (test_addr_x(node->lhs)){
        int off = gen_addr_x(node->lhs,true);
        gen_expr(node->rhs);
        store_x(node->ty,off);
      }else{
        gen_addr(node->lhs);
        tfr_dx();
        gen_direct(node->rhs,"ldab","ldaa");
        store_x(node->ty,0);
      }
      return;
    }
    if (!(node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield)
    &&  test_addr_x(node->lhs)){
      gen_expr(node->rhs);
      int off = gen_addr_x(node->lhs,true);
      store_x(node->ty,off);
      return;
    }
    gen_addr(node->lhs);
    push();
    gen_expr(node->rhs);
    store(node->ty);
    return;
  } // ND_ASSIGN
  case ND_ADDEQ:
  case ND_SUBEQ:
  case ND_MULEQ:
  case ND_DIVEQ:
  case ND_MODEQ:
  case ND_ANDEQ:
  case ND_OREQ:
  case ND_XOREQ:
  case ND_SHLEQ:
  case ND_SHREQ:
    opeq(node);
    return;
  case ND_STMT_EXPR:
    for (Node *n = node->body; n; n = n->next)
      gen_stmt(n);
    return;
  case ND_COMMA:
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    return;
  case ND_CAST:
    if (node->ty->kind==TY_LONG
    &&  (node->lhs->ty->kind==TY_INT || node->lhs->ty->kind==TY_SHORT)
    &&  node->lhs->kind==ND_NUM
    &&  node->lhs->val>=0) {
      gen_expr(node->lhs);
      println("\tstab @long+3");
      println("\tstaa @long+2");
      println("\tclr  @long+1");
      println("\tclr  @long");
      return;
    }
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
    return;
  case ND_MEMZERO:
    // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
    if (node->var->ty->name) {
      println("; ND_MEMZERO %.*s size=%d, offset=%d,  %s %d",
                 node->var->ty->name->len, node->var->ty->name->loc,
                 node->var->ty->size, node->var->offset, __FILE__, __LINE__);
    }else{
      println("; ND_MEMZERO (noname) size=%d, offset=%d,  %s %d",
                 node->var->ty->size, node->var->offset, __FILE__, __LINE__);
    }
    if (node->var->ty->size <= 4
    && node->var->ty->size + node->var->offset < 256) {
      ldx_bp();
      println("\tclrb");
      for (int i=0; i<node->var->ty->size; i++){
        println("\tstab %d,x",node->var->offset+i);
      }
    } else if (node->var->offset < 256) {
      ldx_bp();
      println("\tldab #%d",node->var->ty->size);
      println("\tclra");
      int c = count();
      println("L_memzero_%d:", c);
      println("\tstaa %d,x",node->var->offset);
      println("\tinx");
      println("\tdecb");
      println("\tbne L_memzero_%d", c);
      IX_Dest = IX_None;
    } else {
      println("\tldab @bp+1");
      println("\tldaa @bp");
      if (node->var->offset) {
        println("\taddb #<%d",node->var->offset);
        println("\tadca #>%d",node->var->offset);
      }
      tfr_dx();
      println("\tldab #%d",node->var->ty->size);
      println("\tclra");
      int c = count();
      println("L_memzero_%d:", c);
      println("\tstaa 0,x");
      println("\tinx");
      println("\tdecb");
      println("\tbne L_memzero_%d", c);
      IX_Dest = IX_None;
    }
    return;
  case ND_COND: {
    int c = count();
    char L_else[30];
    char L_end[30];
    sprintf(L_else,"L_else_%d",c);
    sprintf(L_end, "L_end_%d",c);

    Node *cond;
    cond = optimize_expr(node->cond);

    if (!gen_jump_if_false(cond,L_else)){
      gen_expr(cond);
      if (!is_compare_or_not(cond))
        cmp_zero(cond->ty);
      println("\tjeq %s",L_else);
    }
    gen_expr(node->then);
    println("\tjmp %s",L_end);
    println("%s:", L_else);
    IX_Dest = IX_None;
    gen_expr(node->els);
    println("%s:", L_end);
    IX_Dest = IX_None;
    return;
  }
  case ND_NOT: {
    node = optimize_expr(node);
    gen_expr(node->lhs);
    if (is_compare_or_not(node->lhs)) {
      println("\tdecb");
      println("\tnegb");
      return;
    }
    cmp_zero(node->lhs->ty);
    int c = count();
    println("\tbeq L_false_%d", c);
    println("\tclra");
    println("\tclrb");
    println("\tbra L_end_%d", c);
    println("L_false_%d:", c);
    println("\tclra");
    println("\tldab #1");
    println("L_end_%d:", c);
    return;
  }
  case ND_BITNOT:
    assert(is_integer(node->ty));
    gen_expr(node->lhs);
    switch(node->ty->size){
    case 1:
      println("\tclra");
      println("\tcomb");
      println("\tcoma");
      break;
    case 2:
      println("\tcomb");
      println("\tcoma");
      return;
    case 4:
      println("\tcom @long+3");
      println("\tcom @long+2");
      println("\tcom @long+1");
      println("\tcom @long");
      return;
    }
    assert(0);
  case ND_LOGAND: {
    int c = count();
    int need_bool = 0;
    char L_false[32];
    char L_end[32];
    sprintf(L_false,"L_false_%d",c);
    sprintf(L_end,  "L_end_%d",c);

    if (gen_jump_if_false(node->lhs,L_false)){
      need_bool = 1;
    }else{
      gen_expr(node->lhs);
      if (is_compare_or_not(node->lhs)) {
        println("\tjeq %s",L_end);
      }else{
        cmp_zero(node->lhs->ty);
        need_bool = 1;
        println("\tjeq %s",L_false);
      }
    }
    if (gen_jump_if_false(node->rhs,L_false)){
      need_bool = 1;
    }else{
      gen_expr(node->rhs);
      if (is_compare_or_not(node->rhs)) {
        println("\tbra %s",L_end);
      }else{
        cmp_zero(node->rhs->ty);
        println("\tbeq %s",L_false);
        need_bool = 1;
      }
    }
    if (need_bool) {
      println("\tclra");
      println("\tldab #1");
      println("\tbra %s",L_end);
      println("%s:",L_false);
      println("\tclra");
      println("\tclrb");
    }
    println("L_end_%d:", c);
    IX_Dest = IX_None;
    return;
  }
  case ND_LOGOR: {
    int c = count();
    int need_bool = 0;
    char L_true[32], L_end[32];
    sprintf(L_true,"L_true_%d",c);
    sprintf(L_end, "L_end_%d" ,c);

    if (gen_jump_if_true(node->lhs,L_true)) {
      need_bool = 1;
    }else{
      gen_expr(node->lhs);
      if (is_compare_or_not(node->lhs)) {
        println("\tjne %s",L_end);
      }else{
        cmp_zero(node->lhs->ty);
        println("\tjne %s",L_true);
        need_bool = 1;
      }
    }
    if (gen_jump_if_true(node->rhs,L_true)) {
      need_bool = 1;
    }else{
      gen_expr(node->rhs);
      if (is_compare_or_not(node->rhs)){
        println("\tbra %s",L_end);
      }else{
        cmp_zero(node->rhs->ty);
        need_bool = 1;
        println("\tbne %s",L_true);
      }
    }
    if (need_bool) {
      println("\tclra");
      println("\tclrb");
      println("\tbra %s",L_end);
      println("%s:",L_true);
      println("\tclra");
      println("\tldab #1");
    }
    println("%s:", L_end);
    IX_Dest = IX_None;
    return;
  }
  case ND_FUNCALL:
    gen_funcall(node);
    return;
  case ND_LABEL_VAL:
    println("\tldab #<_%s",node->unique_label);
    println("\tldaa #>_%s",node->unique_label);
    return;
  case ND_CAS:
  case ND_EXCH:
    assert(0);
  case ND_BULKINIT:
    // (ND_BULKINIT str NULL +2 _L_55 12)
    println("; memcpy(%s,%s,%d);",node->var->name,node->bulk_data->name,node->ty->size);
    ldd_i(node->ty->size);
    push();
    println("\tldab #<_%s", node->bulk_data->name);
    println("\tldaa #>_%s", node->bulk_data->name);
    push();
    println("\tldab @bp+1");
    println("\tldaa @bp");
    if (node->var->offset) {
      println("\taddb #<%d",node->var->offset);
      println("\tadca #>%d",node->var->offset);
    }
    println("\tjsr _memcpy");
    ins(4);
    return;
  }
  // Above is a unary operator
  //
  // Below is a binary operator
  switch (node->lhs->ty->kind) {
  case TY_FLOAT: {
    if (node->rhs->kind == ND_NUM && node->rhs->ty->kind==TY_FLOAT) {
      union { float f32; uint32_t u32; } u = { node->rhs->fval };
      println("; push float %e, %08x",u.f32,u.u32);
      gen_direct_pushl(u.u32);
    }else{
      gen_expr(node->rhs);	// xmm1
      pushf();
    }
    gen_expr(node->lhs);	// xmm0
    switch (node->kind) {
    case ND_ADD:
      println("\tjsr __addf32tos");
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_SUB:
      println("\tjsr __subf32tos");
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MUL:
      println("\tjsr __mulf32tos");
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_DIV:
      println("\tjsr __divf32tos");
      IX_Dest = IX_None;
      depth -= 4;
      return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_GT:
    case ND_GE: {
      char L_cmpf1[32];
      char L_cmpf2[32];
      int c = count();
      sprintf(L_cmpf1, "L_%d_1_cmpf",c);
      sprintf(L_cmpf2, "L_%d_2_cmpf",c);
      println("\tjsr __cmpf32tos");	// @long cmp  TOS");
      println("\tbcc %s",L_cmpf1);	// when carry=1, compare NaN
      if (node->kind == ND_NE) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      println("\tclra");
      println("\tbra %s",L_cmpf2);
      println("%s:",L_cmpf1);
      println("\tclra");
      if (node->kind == ND_EQ) {
        println("; ND_EQ");
        println("\teorb #1");		// 00->01, 01->00, FF->FE
      } else if (node->kind == ND_NE) {
        println("; float ND_NE");	// EQ:AccB==0, other FF or 01
      } else if (node->kind == ND_LT) {	// AccB:FF true, other false
        println("; ND_LT");
        println("\tlsrb");		// FF->7F, other:00
      } else if (node->kind == ND_GT) {	// AccB:01 true, other false
        println("; ND_GT");
        println("\tincb");		// FF,00,01 -> 00,01,02
        println("\tlsrb");		//          -> 00,00,01
      } else if (node->kind == ND_LE) { // AccB:FF,00 true, other false
        println("; ND_LE");
        println("\tdecb");		// FF,00,01 -> FE,FF,00
        println("\taslb");		//          -> FC,FE,00  C=1,1,0
        println("\trolb");		//          -> F9,FD,00
      } else if (node->kind == ND_GE) { // AccB:00,01 true, other false
        println("; ND_GE");
        println("\tsubb #2");		// AccB:00,01 Carry, other NC
        println("\trolb");
      } else {
        error_tok(node->tok, "invalid expression");
      }
      println("\tandb #1");
      println("%s:",L_cmpf2);
      depth -= 4;
      IX_Dest = IX_None;
      return;
      }
    }

    error_tok(node->tok, "invalid expression");
  } // TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE: {
    error_tok(node->tok, "double not implemented");
  }
  case TY_LONG: {
    switch (node->kind) {
    case ND_ADD:
      if (test_addr_x(node->lhs) && !test_addr_x(node->rhs)){
        node = swap_lr(node);
      }
      if (can_direct_long2(node)){
        gen_direct_long2(node,"addb","adca");
        return;
      }
      gen_expr(node->lhs);
      if (can_direct_long(node->rhs)){
        if (gen_direct_long(node->rhs,"addb","adca")){
          return;
        }
        assert(0);
      }
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __add32tos");	// @long += TOS, pull TOS
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_SUB:
      if (can_direct_long2(node)){
        gen_direct_long2(node,"subb","sbca");
        return;
      }
      if (can_direct_long(node->rhs)){
        gen_expr(node->lhs);
        if (gen_direct_long(node->rhs,"subb","sbca")){
          return;
        }
        assert(0);
      }
      gen_expr(node->lhs);
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __sub32tos");	 // @long = TOS - @long, pull TOS");
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MUL:
      switch (node->lhs->kind) {
      case ND_NUM:
        gen_direct_pushl(node->lhs->val);
        break;
      case ND_VAR:
        if (test_addr_x(node->lhs)){
          int off = gen_addr_x(node->lhs,false);
          pushlx(off);
          break;
        }
	// THRU
      default:
        gen_expr(node->lhs);
        pushl();
      }
      gen_expr(node->rhs);
      println("\tjsr __mul32tos");	// @long *= TOS, pull TOS");
//  println("  imul %s, %s", di, ax);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_DIV:
      gen_expr(node->rhs);
      pushl();
      gen_expr(node->lhs);
      if (node->ty->is_unsigned) {
        println("\tjsr __div32x32u");	// @long /= TOS, pull TOS");
      }else{
        println("\tjsr __div32x32s");	// @long /= TOS, pull TOS");
      }
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MOD:
      gen_expr(node->rhs);
      pushl();
      gen_expr(node->lhs);
      if (node->ty->is_unsigned) {
        println("\tjsr __rem32x32u");	// @long %%= TOS, pull TOS");
      }else{
        println("\tjsr __rem32x32s");	// @long %%= TOS, pull TOS");
      }
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_BITAND:
      gen_expr(node->lhs);
      if (can_direct_long(node->rhs)){
        if (gen_direct_long(node->rhs,"andb","anda")){
          return;
        }
        assert(0);
      }
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __and32tos");
//    println("  and %s, %s", di, ax);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_BITOR:
      gen_expr(node->lhs);
      if (can_direct_long(node->rhs)){
        if (gen_direct_long(node->rhs,"orab","oraa")){
          return;
        }
        assert(0);
      }
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __or32tos");
//    println("  or %s, %s", di, ax);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_BITXOR:
      gen_expr(node->lhs);
      if (can_direct_long(node->rhs)){
        if (gen_direct_long(node->rhs,"eorb","eora")){
          return;
        }
        assert(0);
      }
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __xor32tos");
//    println("  xor %s, %s", di, ax);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_GT:
    case ND_GE:
      if (node->rhs->kind == ND_NUM && node->rhs->ty->kind==TY_LONG) {
        gen_direct_pushl(node->rhs->val);
      }else if (test_addr_x(node->rhs)){
        int off = gen_addr_x(node->rhs,false);
        pushlx(off);
      }else{
        gen_expr(node->rhs);
        pushl();
      }
      gen_expr(node->lhs);
      // Since push in the order is rhs -> lhs, the conditions are reversed.
      // Should I change the name?
      char sc = (node->lhs->ty->is_unsigned)? 'u': 's';
      if (node->kind == ND_EQ) {
        println("\tjsr __eq32");
      } else if (node->kind == ND_NE) {
        println("\tjsr __ne32");
      } else if (node->kind == ND_LT) {
        println("\tjsr __gt32%c",sc);
      } else if (node->kind == ND_LE) {
        println("\tjsr __ge32%c",sc);
      } else if (node->kind == ND_GT) {
        println("\tjsr __lt32%c",sc);
      } else if (node->kind == ND_GE) {
        println("\tjsr __le32%c",sc);
      }
      depth -= 4;
      IX_Dest = IX_None;
      return;
    //
    // Shift operations are not performed by usual_arith_conv() in type.c. 
    // The node and lhs are long, but the type of rhs is unknown.
    // Modify type.c to shift by 1 byte (char) to reduce the size.
    // When calling the helper function, provide the shift amount in AccB.
    //
    case ND_SHL:
    case ND_SHR:
      if (is_integer_constant(node->rhs,&val)) {
        val &= 0x00ff;
        gen_expr(node->lhs);
        if (val==0)
          return;
        if (node->kind == ND_SHL && gen_direct_shl_long(node,val))
          return;
        if (node->kind == ND_SHR && gen_direct_shr_long(node,val))
          return;
        println("\tldab #%ld",val);
      }else{
        gen_expr(node->rhs);
        push1();
        gen_expr(node->lhs);
        pop1();
      }
      if (node->kind == ND_SHL) {
        println("\tjsr __shl32");
      }else if (node->lhs->ty->is_unsigned) {	// ND_SHR
        println("\tjsr __shr32u");
      }else{
        println("\tjsr __shr32s");
      }
      IX_Dest = IX_None;
      return;
    default:
      error_tok(node->tok, "TY_LONG: invalid expression");
    }
  } // TY_LONG:
  }
  // The following is a binary operator, length less than or equal to an int
  switch (node->kind) {
  case ND_ADD: {
    if (gen_direct_lr(node,"addb","adca"))
      return;
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->ty->kind == TY_CHAR
    &&  node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_INT
    &&  node->rhs->lhs->ty->kind == TY_CHAR
    &&  node->rhs->lhs->ty->is_unsigned ) {
      gen_expr(node->lhs->lhs);
      push1();
      gen_expr(node->rhs->lhs);
      popa();
      println("\taba");
      println("\ttab");
      println("\tldaa #0");
      println("\tadca #0");
      return;
    }
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->ty->kind == TY_CHAR
    &&  !node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_INT
    &&  node->rhs->lhs->ty->kind == TY_CHAR
    &&  !node->rhs->lhs->ty->is_unsigned ) {
      char *label = new_label("L_%d");
      gen_expr(node->lhs->lhs);
      push1();
      gen_expr(node->rhs->lhs);
      println("\tclra");
      println("\ttsx");
      println("\taddb 0,x");
      println("\tbvc %s",label);
      println("\tdeca");
      println("%s:",label);
      ins(1);
      return;
    }
    if (node->rhs->kind     == ND_CAST
    &&  node->rhs->ty->kind == TY_INT
    &&  !node->rhs->ty->is_unsigned
    &&  node->rhs->lhs->ty->kind == TY_CHAR
    &&  !node->rhs->lhs->ty->is_unsigned
    &&  test_addr_x(node->rhs->lhs)) {
      gen_expr(node->lhs);
      off = gen_addr_x(node->rhs->lhs,true);
      char *label = new_label("L_%d");
      println("\ttst %d,x",off);
      println("\tbpl %s",label);
      println("\tdeca");
      println("%s:",label);
      println("\taddb %d,x",off);
      println("\tadca #0");
      return;
    }
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\taddb 1,x");
    println("\tadca 0,x");
    ins(2);
    IX_Dest = IX_None;
    return;
  } // ND_ADD
  case ND_SUB:
    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if(gen_direct(node->rhs,"subb","sbca"))
        return;
      assert(0);
    }
    if (can_direct(node->lhs)){
      gen_expr(node->rhs);
      negd();
      if(gen_direct(node->lhs,"addb","adca"))
        return;
      assert(0);
    }
    if (test_addr_x(node->rhs)){
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs,true);
      println("\tsubb %d+1,x",off);
      println("\tsbca %d,x",off);
      return;
    }
    if (test_addr_x(node->lhs)){
      gen_expr(node->rhs);
      negd();
      int off = gen_addr_x(node->lhs,true);
      println("\taddb %d+1,x",off);
      println("\tadca %d,x",off);
      return;
    }
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->ty->kind == TY_CHAR
    &&  node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_INT
    &&  node->rhs->lhs->ty->kind == TY_CHAR
    &&  node->rhs->lhs->ty->is_unsigned ) {
      gen_expr(node->lhs->lhs);
      push1();
      gen_expr(node->rhs->lhs);
      popa();
      println("\tsba");
      println("\ttab");
      println("\tldaa #0");
      println("\tsbca #0");
      return;
    }
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->ty->kind == TY_CHAR
    &&  !node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_INT
    &&  node->rhs->lhs->ty->kind == TY_CHAR
    &&  !node->rhs->lhs->ty->is_unsigned ) {
      gen_expr(node->lhs->lhs);
      println("\teorb #$80");
      push1();
      gen_expr(node->rhs->lhs);
      println("\teorb #$80");
      popa();
      println("\tsba");
      println("\ttab");
      println("\tldaa #0");
      println("\tsbca #0");
      return;
    }
    gen_expr(node->rhs);		// TODO: lhs to rhs
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
//  cast(node->lhs->ty, node->ty);
//    println("  sub %s, %s", di, ax);
    println("\ttsx");
    println("\tsubb 1,x");
    println("\tsbca 0,x");
    ins(2);
    IX_Dest = IX_None;
    return;
  case ND_MUL:
    gen_expr(node->lhs);
    if (node->lhs->ty ==  node->ty
    &&  gen_mul16(node)) {
      return;
    }
//  cast(node->lhs->ty, node->ty);
    push();
    gen_expr(node->rhs);
//  cast(node->lhs->ty, node->ty);
    println("\tjsr __mul16x16");
    ins(2);
    IX_Dest = IX_None;
    return;
  case ND_DIV:
    if (node->lhs->ty ==  node->ty){
      switch(node->rhs->kind){
      case ND_NUM:
        switch (node->rhs->ty->kind) {
        case TY_INT:
        case TY_SHORT:
          switch(node->rhs->val){
          case 2:
            gen_expr(node->lhs);
            if (node->lhs->ty->is_unsigned){
              println("\tasra");
              println("\trorb");
            }else{
              println("\tasra");
              println("\trola");
              println("\tadcb #0");
              println("\tadca #0");
              println("\tasra");
              println("\trorb");
            }
            return;
          }
        }
      }
    }
    gen_expr(node->rhs);
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
//  cast(node->lhs->ty, node->ty);
    if (node->ty->is_unsigned) {
      println("\tjsr __div16x16u");
    }else{
      println("\tjsr __div16x16s");
    }
    ins(2);
    IX_Dest = IX_None;
    return;
  case ND_MOD:
    gen_expr(node->rhs);
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
//  cast(node->lhs->ty, node->ty);
    if (node->ty->is_unsigned) {
      println("\tjsr __rem16x16u");
    }else{
      println("\tjsr __rem16x16s");
    }
    ins(2);
    IX_Dest = IX_None;
    return;
  case ND_BITAND:
    if (gen_direct_lr(node,"andb","anda")){
      return;
    }
    if (node->ty->kind == TY_BOOL
    &&  node->ty->kind == TY_CHAR) {
      gen_expr(node->lhs);
      push1();
      gen_expr(node->rhs);
      println("\ttsx");
      IX_Dest = IX_None;
      println("\tandb 0,x");
      ins(1);
      return;
    }
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\tandb 1,x");
    println("\tanda 0,x");
    ins(2);
    return;
  case ND_BITOR:
    if (gen_direct_lr(node,"orb","ora"))
      return;

    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\torab 1,x");
    println("\toraa 0,x");
    ins(2);
    return;
  case ND_BITXOR:
    if (gen_direct_lr(node,"eorb","eora"))
      return;

    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\teorb 1,x");
    println("\teora 0,x");
    ins(2);
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if(!gen_direct(node->rhs,"subb","sbca"))
        assert(0);
    }else{
      gen_expr(node->rhs);
      push();
      gen_expr(node->lhs);
      println("\ttsx");
      IX_Dest = IX_None;
      println("\tsubb 1,x");
      println("\tsbca 0,x");
      ins(2);
    }
    if (node->kind == ND_EQ) {
      println("\tjsr __eq16");
    } else if (node->kind == ND_NE) {
      println("\tjsr __ne16");
    } else if (node->kind == ND_LT) {
      if (node->lhs->ty->is_unsigned)
        println("\tjsr __lt16u");
      else
        println("\tjsr __lt16s");
    } else if (node->kind == ND_LE) {
      if (node->lhs->ty->is_unsigned)
        println("\tjsr __le16u");
      else
        println("\tjsr __le16s");
    } else if (node->kind == ND_GT) {
      if (node->lhs->ty->is_unsigned)
        println("\tjsr __gt16u");
      else
        println("\tjsr __gt16s");
    } else if (node->kind == ND_GE) {
      if (node->lhs->ty->is_unsigned)
        println("\tjsr __ge16u");
      else
        println("\tjsr __ge16s");
    }
    return;
  case ND_SHL: {
    int64_t val;

    if (is_integer_constant(node->rhs, &val)){
      gen_expr(node->lhs);
      if (gen_shl(node->lhs->ty,val)) {
        return;
      }
      println("\tclrb");
      println("\tclra");
      return;
    }
    gen_expr(node->rhs);
    push1();
    gen_expr(node->lhs);
//  shl16: AccAB << TOS(8bit)
    println("\tjsr __shl16");
    ins(1);
    IX_Dest = IX_None;
    return;
  } // ND_SHL
  case ND_SHR: {
    int64_t val;

    if (is_integer_constant(node->rhs, &val)){
      gen_expr(node->lhs);
      if (gen_shr(node->lhs->ty,val)) {
//      cast(node->lhs->ty, node->ty);
        return;
      }
      println("\tclrb");
      println("\tclra");
      return;
    }
    gen_expr(node->rhs);
    push1();
    gen_expr(node->lhs);
//  cast(node->lhs->ty, node->ty);
//  shr16: AccAB >> TOS(16bit)
    if (node->lhs->ty->is_unsigned){
      println("\tjsr __shr16u");
    }else{
      println("\tjsr __shr16s");
    }
    ins(1);
    IX_Dest = IX_None;
    return;
  } // ND_SHR
  default:
  }
  error_tok(node->tok, "invalid expression");
}

void stmt_dump(char *p)
{
  char s[1024];
  char *q = s;
  static char *pp = NULL;

  if (opt_g < '1') {
    return;
  }
  if (pp!=p) {
    if(*p && *p!=';' && *p!='\r' && *p!='\n'){
      while(*p && *p!=';' && *p!='\r' && *p!='\n'){
        p--;
      }
      p++;
      while (isspace(*p)) {
	p++;
      }
    }
  }
  while(*p && *p!='\r' && *p!='\n'){
    *q++ = *p++;
  }
  *q = '\0';
  if (pp!=p && strcmp(s,";") && s[0]){
    println("; %.75s",s);
    pp = p;
  }
}

static void gen_stmt(Node *node)
{
  // When the -g option is added, 
  // the original C source code is embeddedwithin the assembly source.
  stmt_dump(node->loc);
  // With -g2 or higher, AST node details are also embedded;
  // Assembly may sometimes fail in such cases.
  if (opt_g >= '2') {
    ast_node_dump(node);
  }

  switch (node->kind) {
  case ND_IF: {
    Node *cond = node->cond;
    int c = count();
    char L_else[32];
    char L_end[32];
    if (node->els){
      sprintf(L_else,"L_else_%d",c);
      sprintf(L_end, "L_end_%d"  ,c);
    }else{
      sprintf(L_end, "L_end_%d"  ,c);
      strcpy(L_else,L_end);
    }
    cond = optimize_expr(cond);
    if (!gen_jump_if_false(cond,L_else)){
      gen_expr(cond);
      if (!is_compare_or_not(cond))
        cmp_zero(cond->ty);
      println("\tjeq %s",L_else);
    }
    gen_stmt(node->then);
    if (node->els){
      println("\tjmp %s", L_end);
      println("%s:", L_else);
      IX_Dest = IX_None;
      gen_stmt(node->els);
    }
    println("L_end_%d:", c);
    IX_Dest = IX_None;
    return;
  }
  case ND_FOR: {
    Node *cond = node->cond;
    int64_t val;
    char if_false[30];
    int c = count();
    sprintf(if_false,"_%s",node->brk_label);
    if (node->init)
      gen_stmt(node->init);
    println("L_begin_%d:", c);
    IX_Dest = IX_None;
    if (cond) {
      cond = optimize_expr(cond);
      if (is_integer_constant(cond,&val)) {
        if (val==0) {
          println("\tjmp %s", if_false);
        }
      }else{
        if (!gen_jump_if_false(cond,if_false)){
          gen_expr(cond);
          if (!is_compare_or_not(cond))
            cmp_zero(cond->ty);
          println("\tjeq %s", if_false);
        }
      }
    }
    gen_stmt(node->then);
    println("_%s:", node->cont_label);
    IX_Dest = IX_None;
    if (node->inc) {
      stmt_dump(node->inc->loc);
      node->inc->retval_unused = true;
      gen_expr(node->inc);
    }
    println("\tjmp L_begin_%d", c);
    println("%s:", if_false);
    IX_Dest = IX_None;
    return;
  }
  case ND_DO: {
    Node *cond = node->cond;
    int c = count();
    int64_t val;
    char L_begin[30];
    sprintf(L_begin,"L_begin_%d", c);
    println("%s:",L_begin);
    IX_Dest = IX_None;
    gen_stmt(node->then);
    println("_%s:", node->cont_label);
    stmt_dump(cond->loc);
    cond = optimize_expr(cond);
    if (is_integer_constant(cond,&val)) {
      if (val!=0) {
        println("\tjmp %s", L_begin);
        IX_Dest = IX_None;
      }
    }else{
      if (!gen_jump_if_true(cond,L_begin)){
        gen_expr(cond);
        if (!is_compare_or_not(cond))
          cmp_zero(cond->ty);
        println("\tjne %s", L_begin);
      }
      IX_Dest = IX_None;
    }
    println("_%s:", node->brk_label);
    return;
  } // ND_DO
  case ND_SWITCH: {
    bool has_case_ranges = 0;

    for (Node *n = node->case_next; n; n = n->case_next) {
      if (n->begin != n->end) {
        has_case_ranges = 1;
        break;
      }
    }
    if (node->cond->ty->size == 2 && !has_case_ranges) {
      if (test_addr_x(node->cond)) {
        int off = gen_addr_x(node->cond,false);
        println("\tldx %d,x",off);
      }else{
        gen_expr(node->cond);
        tfr_dx();
      }
      IX_Dest = IX_None;
    }else{
      gen_expr(node->cond);
    }
    for (Node *n = node->case_next; n; n = n->case_next) {
      // TODO: 32bit case
      stmt_dump(n->loc);
      if (n->begin == n->end) {
        switch (node->cond->ty->size) {
        case 1:
          println("\tcmpb #<%ld",n->begin);
          println("\tjeq _%s", n->label);
          break;
        case 2:
          println("\tcpx #%ld",n->begin);
          println("\tjeq _%s", n->label);
          break;
        case 4: {
          int c = count();
          println("\tldx #%ld	; %ld",n->begin & 0x0ffff,n->begin);
          println("\tcpx @long+2");
          println("\tbne L_case_%d",c);
          println("\tldx #%ld",(n->begin>>16)&0x0ffff);
          println("\tcpx @long");
          println("\tjeq _%s", n->label);
          println("L_case_%d:",c);
          IX_Dest = IX_None;
          break;
        }
        default:
          assert(0);
        }
        continue;
      }
      // [GNU] Case ranges
      switch (node->cond->ty->size) {
      case 1:
        println("\ttba");
        println("\tsuba #%ld", n->begin);
        println("\tcmpa #%ld", n->end - n->begin);
        println("\tjcs _%s",   n->label);
        break;
      case 2:
        println("\tpshb");
        println("\tpsha");
        println("\tsubb #<%ld", n->begin);
        println("\tsbca #>%ld", n->begin);
        println("\tsubb #<%ld", n->end - n->begin);
        println("\tsbca #>%ld", n->end - n->begin);
        println("\tpula");
        println("\tpulb");
        println("\tjcs _%s", n->label);
        break;
      case 4:
        // TODO: case range
        assert(0);
      }
    }

    if (node->default_case)
      println("\tjmp _%s", node->default_case->label);

    println("\tjmp _%s", node->brk_label);
    IX_Dest = IX_None;
    gen_stmt(node->then);
    println("_%s:", node->brk_label);
    IX_Dest = IX_None;
    return;
  } // ND_SWITCH
  case ND_CASE:
    println("_%s:", node->label);
    IX_Dest = IX_None;
    gen_stmt(node->lhs);
    return;
  case ND_BLOCK:
    for (Node *n = node->body; n; n = n->next)
      gen_stmt(n);
    return;
  case ND_GOTO:
    println("\tjmp _%s", node->unique_label);
    return;
  case ND_GOTO_EXPR:
    gen_expr(node->lhs);
    println("\tpshb	; jmp [AccD]");
    println("\tpsha");
    println("\trts");
    println(";");
    return;
  case ND_LABEL:
    println("_%s:", node->unique_label);
    IX_Dest = IX_None;
    gen_stmt(node->lhs);
    return;
  case ND_RETURN:
    if (node->lhs) {
      gen_expr(optimize_expr(node->lhs));
      Type *ty = node->lhs->ty;
      switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
        copy_struct_mem();
        break;
      }
    }

    println("\tjmp L_return_%d", current_fn->function_no);
//    println("\tjmp L_return_%s", current_fn->name);
    return;
  case ND_EXPR_STMT:
    node->lhs->retval_unused = true;
    gen_expr(optimize_expr(node->lhs));
    return;
  case ND_ASM:
    if (strchr(node->asm_str,':')==NULL) {
      println("\t%s", node->asm_str);
    }else{
      println("%s", node->asm_str);
    }
    return;
  }

  error_tok(node->tok, "invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function)
      continue;

    // If a function has many parameters, only first one parameters is
    // passed by register (AB or @long).
    // The first passed-by-stack parameter resides at SP+2
    int top = 0;

    int gp = 0;	// if gp==0 can use reg_param.
		//
    fn->stack_size = 0;
    int has_implicit_reg_param = 0;
    switch (fn->ty->return_ty->kind){
    case TY_STRUCT:
    case TY_UNION:
      has_implicit_reg_param = 1;
      gp++;
    }

    // list of param
    for (Obj *var = fn->params; var; var = var->next) {
      Type *ty = var->ty;

      var->reg_param = 0;	// pass by registe? 1:reg, 0:stack

      switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
        gp++;
        var->offset = -2;	// STRUCT/UNION must pass via stack
        continue;
      case TY_DOUBLE:
      case TY_LDOUBLE:
        assert(ty->kind!=TY_DOUBLE && ty->kind!=TY_LDOUBLE);
        break;
      default:
        if (gp++<1){		// only one args pass by register
          var->offset = -1;
          var->reg_param = 1;	// reg param mark
          continue;
        }
        var->offset = -2;		// stack param mark
      }
    }

    // Calculate size of local variable area and Assign offsets to locals
    int ret_skipped = 0;
    for (Obj *var = fn->locals; var; var = var->next) { // locals & args
      if (var->offset>0)
        continue;

      if (var->offset == -1) {	// レジスタ引数
        fn->stack_size = top;
        var->offset = top;
        top += var->ty->size + 4;	// skip old @bp, ret addr
        ret_skipped = 1;
        continue;
      }else if (var->offset == -2){	// stack param
        if (!ret_skipped) {
          fn->stack_size = top;
          top += 4 + (has_implicit_reg_param?2:0);
          ret_skipped = 1;
        }
        var->offset = top;
        top += var->ty->size;
        continue;
      }else{ // ローカル変数の割り当て
        var->offset = top;
        top += var->ty->size;
        fn->stack_size = top;
      }
    }
  }
}

static void emit_data(Obj *prog) {
  for (Obj *var = prog; var; var = var->next) {
    if (var->is_function || !var->is_definition)
      continue;

    if (!var->is_static)
      println("\t.export _%s", var->name);

    int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)
      ? MAX(16, var->align) : var->align;

    // Common symbol
    if (opt_fcommon && var->is_tentative) {
      println(";  .comm %s, %d, %d", var->name, var->ty->size, align);
      println("_%s:",var->name);
      switch(var->ty->size){
      case 1: println("\t.byte 0");
        break;
      case 2:	println("\t.word 0");
        break;
      case 4:	println("\t.word 0");
        println("\t.word 0");
        break;
      default:println("\t.blkb %d",var->ty->size);
        break;
      }
      continue;
    }

    // .data or .tdata in x64
    //   .section .data  ; 初期化済み静的データ
    //   .section .tdata ; Thread Local Storage
    //   .section .rodata ; 書き換え不可能データ
    if (var->init_data) {
      println(";");
      println(";\t.type %s, @object", var->name);
      println(";\t.size %s, %d", var->name, var->ty->size);
      if (var->is_literal)
        println("\t.literal");
      else
        println("\t.data");

      println("_%s:", var->name);

      Relocation *rel = var->rel;
      int pos = 0;
      println("; var->ty->size = %d",var->ty->size);
      while (pos < var->ty->size) {
        if (rel && rel->offset == pos) {
          println("\t.word _%s%+ld", *rel->label, rel->addend);
          rel = rel->next;
          pos += 2;
        } else {
          if (isprint(var->init_data[pos])
          &&  var->init_data[pos]!='\\'   ){
            println("\t.byte %d	; '%c'", var->init_data[pos],
                                         var->init_data[pos]);
          }else{
            println("\t.byte %d", var->init_data[pos]);
          }
          pos++;
        }
      }
      continue;
    }

    // .bss or .tbss
    println("\t.bss");
    println("_%s:\n\t.ds %d", var->name,var->ty->size);
  }
}

static void emit_text(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function || !fn->is_definition)
      continue;

    // No code is emitted for "static inline" functions
    // if no one is referencing them.
    if (!fn->is_live)
      continue;

    if (!fn->is_static)
      println("\t.export _%s", fn->name);

    println("\t.code");
//    println("\t.type %s, @function", fn->name);
    println("_%s:", fn->name);
    current_fn = fn;
    current_fn->function_no = count();
    println("; fn->body->kind=%d",fn->body->kind);
    if(fn->body->kind==ND_BLOCK && fn->body->body==NULL){	// empty function
      println("\trts	; empty function");
      println(";");
      IX_Dest = IX_None;
      continue;
    }
    // Prologue
    println("; function %s prologue emit_text %s %d",fn->name,__FILE__,__LINE__);
    println("; function %s use alloca/vla %d",fn->name,fn->use_alloca);
    if (!fn->params && !fn->stack_size && !fn->use_alloca) {
      println("; function has no params & locals");
      IX_Dest = IX_None;
      depth = 0;
      goto no_params_locals;
    }

    // only one argument pass via Acc A,B, @long
    // save passed-by-register arguments to the stack
    int reg_param_size = 0;
    // 返り値がSTRUCT/UNIONの場合は、レジスタ引数に返り値のアドレスが入る
    switch (fn->ty->return_ty->kind){
    case TY_STRUCT:
    case TY_UNION:
      reg_param_size = 2;
      break;
    default:
      for (Obj *var = fn->params; var; var = var->next) {
        if (var->reg_param) {
          reg_param_size = var->ty->size;
          break;
        }
      }
    }
    if (opt_O == 's') {
      println("\tjsr __prologue_%d",reg_param_size);
    } else {
      if (reg_param_size)
        println("\tstaa @tmp1");
      println("\tldaa @bp+1");			// push old @bp
      println("\tpsha");
      println("\tldaa @bp");
      println("\tpsha");
      if (reg_param_size)
        println("\tldaa @tmp1");

      switch (reg_param_size){
      case 0:
        break;
      case 1:
        push1();
        break;
      case 2:
        push();
        break;
      case 4:
        pushl();
        break;
      default:
        assert(0);
      }
    }
    // make base pointer
    if (fn->stack_size<=5){			// 5 for speed, 13 for size
      des(fn->stack_size);
      println("\ttsx");	 	      // 4 1
      println("\tstx @bp");			// 5 2
      IX_Dest = IX_BP;
    }else if (opt_O == 's') {
      println("\tsts @bp");
      if (fn->stack_size-1<=255) {
        println("\tldab #%u",fn->stack_size-1);
        println("\tjsr __sub_bp_b");
      }else{
        println("\tldab #<%u",fn->stack_size-1);
        println("\tldaa #>%u",fn->stack_size-1);
        println("\tjsr __sub_bp_d");
      } 
      println("\ttxs");
      IX_Dest = IX_BP;
    }else{					// make new bp
      println("\tsts @bp");     // 5 2	total 31cyc,17bytes
      println("\tldab @bp+1");	// 3 2
      println("\tldaa @bp");		// 3 2
      println("\tsubb #<%u",fn->stack_size-1);	// 2 2
      println("\tsbca #>%u",fn->stack_size-1);	// 2 2
      println("\tstab @bp+1");	// 4 2
      println("\tstaa @bp");		// 4 2
      println("\tldx @bp");			// 4 2
      println("\ttxs");         // 4 1
      IX_Dest = IX_BP;
    }
    depth = 0;
    if (fn->alloca_bottom) {
      if (fn->alloca_bottom->offset<256){
        println("\tstx %d,x	; save sp to __alloca_bottom__",fn->alloca_bottom->offset);
      }else{
        println("\taddb #<%d",fn->alloca_bottom->offset);
        println("\tadca #>%d",fn->alloca_bottom->offset);
        tfr_dx();
        println("\tldab @bp+1");
        println("\tldaa @bp");
        println("\tstab 1,x	; save sp to __alloca_bottom__");
        println("\tstaa 0,x");
      }
    }
    // Emit code
no_params_locals:
    gen_stmt(fn->body);
//    assert(depth == 0);

    // [https://www.sigbus.info/n1570#5.1.2.2.3p1] The C spec defines
    // a special rule for the main function. Reaching the end of the
    // main function is equivalent to returning 0, even though the
    // behavior is undefined for the other functions.
    if (strcmp(fn->name, "main") == 0){
      println("\tclrb");
      println("\tclra");
    }

    // Epilogue
    println("L_return_%d:", fn->function_no);
    println("; function %s epilogue emit_text %s %d",fn->name,__FILE__,__LINE__);
    println("; recover sp, fn->stack_size=%d reg_param_size=%d",
	   	    	fn->stack_size,reg_param_size);
    println("; fn->ty->return_ty->size = %d", fn->ty->return_ty->size);
    println("; function %s use alloca/vla %d",fn->name,fn->use_alloca);
    if (!fn->params && !fn->stack_size & !fn->use_alloca) {
      println("; function has no params & locals");
      IX_Dest = IX_None;
      goto no_params_locals2;
    }
    if (fn->stack_size + reg_param_size <= 10){
      println("; fn->stack_size %d, reg_param_size %d",fn->stack_size,reg_param_size);
      int npops = fn->stack_size + reg_param_size - 1;
      if (npops>=0) {
        println("\tlds @bp");
        for(int i=0; i<npops; i++)
          println("\tins");
      }else{
        println("\tldx @bp");
        for(int i=0; i<abs(npops)-1; i++)
          println("\tdex");	// if use des, be corrupted when interrupt.
        println("\ttxs");
      }
    }else {
      switch (fn->ty->return_ty->kind){
      case TY_VOID:
      case TY_LONG:
      case TY_FLOAT:
        break;
      default:
        println("\tpshb");
      }
      if (opt_O == 's' && fn->stack_size+reg_param_size-1 < 256) {
        println("\tldab #%u",fn->stack_size+reg_param_size-1);
        println("\tjsr __add_bp_b");
      }else{
        println("\tldab @bp+1");					// 3 2 // 18 12
        println("\taddb #<%u",fn->stack_size+reg_param_size-1);	// 2 2
        println("\tstab @bp+1");					// 4 2
        println("\tldab @bp");			      // 3 2
        println("\tadcb #>%u",fn->stack_size+reg_param_size-1);	// 2 2
        println("\tstab @bp");					  // 4 2
      }
      switch (fn->ty->return_ty->kind){
      case TY_VOID:
      case TY_LONG:
      case TY_FLOAT:
        break;
      default:
        println("\tpulb");
      }
      println("\tlds @bp");		// remove local variables // 4 2
    }
    switch (fn->ty->return_ty->kind){
    case TY_VOID:
    case TY_LONG:
    case TY_FLOAT:
      if (opt_O == 's') {
        println("\tjmp __pulbp_rts");
      }else{
        println("\tpulb");
        println("\tstab @bp");
        println("\tpulb");
        println("\tstab @bp+1");
        println("\trts");		// 5 1
        println(";");
      }
      goto epilogue_end;
      ;
    default:
      popx();
      println("\tstx @bp");
      println("\trts");		// 5 1
      println(";");
      goto epilogue_end;
    }
no_params_locals2:
    println("\trts");		// 5 1
    println(";");
epilogue_end:
  }
}

void codegen(Obj *prog, FILE *out) {
  output_file = out;

  println("\t.setcpu 6800");

  File **files = get_input_files();
  for (int i = 0; files[i]; i++)
    println(";\t.file %d \"%s\"", files[i]->file_no, files[i]->name);

  assign_lvar_offsets(prog);
  emit_data(prog);
  emit_text(prog);
}
