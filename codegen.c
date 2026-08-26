#include "chibicc.h"

static FILE *output_file;
int depth;
static Obj *current_fn;

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

char *new_label_count(char *fmt,int c)
{
  char *buf = calloc(1,strlen(fmt)+20); // 20?

  sprintf(buf, fmt, c);

  return buf;
}

char *new_label(char *fmt)
{
  return new_label_count(fmt,count());
}

char *new_jump_label(void)
{
  return new_label("L_%d");
}

typedef struct label_link {
  char *label;
  struct label_link *next;
} label_link_t;

label_link_t *llink = NULL;

static bool check_used_label(char *s)
{
  label_link_t *p = llink;

  while (p != NULL) {
    if (strcmp(s,p->label)==0) {
      return true;
    }
    p = p->next;
  }
  return false;
}

static void mark_used_label(char *s)
{
  if (check_used_label(s)) {
    return;
  }

  label_link_t *p = malloc(sizeof(label_link_t));

  p->label = s;
  p->next = llink;
  llink = p;
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
  if (opt('O','s')) {
    println("\tjsr __popx");
  } else {
    println("\ttsx");
    println("\tldx 0,x");
    println("\tins");
    println("\tins");
  }
  IX_invalidate();
  depth-=2;
}

//
// Use ins_only(n) to preserve flags.
//
void ins(int n)
{
  for (int i=0; i<n; i++) {
    println("\tins");
  }
  depth -= n;

  return;
}

void sign_extend()
{
  println("\tclra");
  println("\tasrb");
  println("\trolb");
  println("\tsbca #0");
}

// 
// Removes stack args (flags may be affected).
//
void remove_args(int n)
{
  assert(depth>=n && n>=0);
  if (opt('g','2')) {
    println("; remove_args(%d), depth=%d",n,depth);
  }
  if (n==0) {
    return;
  }
  // function has @bp, and not use alloca()
  if ((current_fn->params
    || current_fn->stack_size)
  &&  !current_fn->use_alloca) {
    if (depth==n) { // Removes all stack args.
      if (n>=3 || (n>=2 && opt('O','2'))) {
        ldx_bp();
        println("\ttxs");
        depth = 0;
        return;
      }
    }
    if ((opt('O','s') && n*2 > depth+3) // Fewer bytes
    ||  (n*2 > depth+2)) {              // Reduce cycles?
      ldx_bp();
      for (int i=n; i<depth; i++) {
        println("\tdex");
        IX_invalidate();
      }
      println("\ttxs");
      depth -= n;
      return;
    }
  }
  if (opt('O','s')) {
    if (n<=8) {                      // Smaller than sts/lds
      ins(n);
      return;
    }
    if (n<=20) {                     // Smallest, but slower
      println("\tjsr __ins_i");
      println("\t.byte  %d",n);
      IX_invalidate();
      depth-=n;
      return;
    }
  }else{
    if (n*4 <= 34) {                 // Faster than sts/lds
      ins(n);
      return;
    }
  }
  println("; ins*%d",n);
  println("\tstaa @tmp2");		// 4
  println("\tsts @tmp1");		// 5
  println("\tldaa @tmp1+1");	// 3
  println("\tadda #<%d",n);// 2
  println("\tstaa @tmp1+1"); 	// 4
  println("\tldaa @tmp1");		// 3
  println("\tadca #>%d",n);// 2
  println("\tstaa @tmp1");		// 4
  println("\tlds @tmp1");		// 4
  println("\tldaa @tmp2");		// 3
  depth-=n;
}

static void des(int n)
{
  depth -= n;
  while (n-->0) {
    println("\tdes");
  }
}

void ldab_i(int n)
{
  if ((n & 0x00ff)==0) {
    println("\tclrb");
  } else {
    println("\tldab #<%d",n);
  }
}

void ldd_i(int n)
{
  ldab_i(n);

  if ((n & 0x0ff00)==0) {
    println("\tclra");
  } else if ((n & 0x00ff) == ((n & 0x0ff00)>>8)) {
    println("\ttba");
  } else {
    println("\tldaa #>%d",n);
  }
}

void andb_i(int n)
{
  if ((n & 0x00ff)==0) {
    println("\tclrb");
  } else if ((n & 0x00ff)!=0x00ff) {
    println("\tandb #<%u",n);
  }
}

void and_i(int n)
{
  andb_i(n);
  if ((n & 0x0ff00)==0) {
    println("\tclra");
  } else if ((n & 0xff00)!=0xff00) {
    println("\tanda #>%u",n);
  }
}

void orab_i(int n)
{
  if ((n & 0x00ff)!=0) {
    println("\torab #<%u",n);
  }
}

void ora_i(int n)
{
  orab_i(n);
  if ((n & 0xff00)!=0) {
    println("\toraa #>%u",n);
  }
}

void eorb_i(int n)
{
  if ((n & 0x00ff)==0) {
    ;
  } else if ((n & 0x00ff)==0x00ff) {
    println("\tcomb");
  } else {
    println("\teorb #<%u",n);
  }
}

void eor_i(int n)
{
  eorb_i(n);

  if ((n & 0xff00)==0) {
    ;
  } else if ((n & 0xff00)==0xff00) {
    println("\tcoma");
  } else {
    println("\teora #>%u",n);
  }
}

void pushl(void) {
  // push32/32x/32bx/32dx destroy IX, which may require reloading IX later.
  // Generating the value directly at -O2 can reduce this overhead.
  if (opt('O','2')) {
    println("\tldab @long+3");
    println("\tpshb");
    println("\tldab @long+2");
    println("\tpshb");
    println("\tldab @long+1");
    println("\tpshb");
    println("\tldab @long");
    println("\tpshb");
    depth+=4;
    return;
  }
  println("\tjsr __push32");
  IX_invalidate();
  depth+=4;
}

static void popl(void) {
  if (opt('O','2')) {
    println("\tpulb");
    println("\tstab @long");
    println("\tpulb");
    println("\tstab @long+1");
    println("\tpulb");
    println("\tstab @long+2");
    println("\tpulb");
    println("\tstab @long+3");
    depth-=4;
    return;
  }
  println("\tjsr __pop32");
  IX_invalidate();
  depth-=4;
}

void pushf(void) {
  pushl();
}

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
int align_to(int n, int align) {
  return n;	// 6800 has no alignment restrictions.
//  return (n + align - 1) / align * align;
}

void negd()
{
  println("\tnega");
  println("\tnegb");
  println("\tsbca #0");
}

void inx_dex(int n)
{
  for (int i=0; i<n; i++) {
    println("\tinx");
  }
  for (int i=0; i>n; i--) {
    println("\tdex");
  }
  if (n) {
    IX_invalidate();
  }
}

void abx()
{
  println("\tjsr __abx");
  IX_invalidate();
}

void adx()
{
  println("\tjsr __adx");
  IX_invalidate();
}

void ldx_bp()
{
  if (IX_Dest == IX_BP){
    if (opt('g','3')) {
      println("; optimize ldx @bp");
    }
  } else {
    if (opt('g','3')) {
      println("; stack depth = %d",depth);
    }
    if(depth==0 && !current_fn->use_alloca) {
      println("\ttsx");
    }else if (!current_fn->use_bp) {
      // function has no @bp. depth = @bp - sp
      println("\ttsx");
      for (int i=0; i<depth; i++) {
        println("\tinx");
      }
    }else{
      println("\tldx @bp");	// TSX cannot be used because of VLA/alloca
    }
  }
  IX_Dest = IX_BP;
}

void ldx_nX(int off)
{
  println("\tldx %d,x",off);
  if (IX_Dest != IX_BP) {
    IX_invalidate();
    return;
  }
  IX_Dest = IX_PTR;
  IX_PTR_off = off;
}

// C11 6.3.2.1p3,p4: an array or a function is converted to a pointer, so the value is the address
bool is_decay_type(Type *ty)
{
  switch (ty->kind) {
  case TY_ARRAY:
  case TY_FUNC:
  case TY_VLA:
    return true;
  }
  return false;
}

void ldx_bp_nX(int off)
{
  if (IX_Dest == IX_PTR && IX_PTR_off == off){
    if (opt('g','3')) {
      println("; optimize ldx %d,x",off);
    }
    return;
  }
  ldx_bp();
  ldx_nX(off);
}

void ldx_EXT(Node *node)
{
  assert(node->kind == ND_VAR);
  Obj *var = node->var;
  assert(!var->is_local);

  if (IX_Dest == IX_EXT && strcmp(var->name,IX_EXT_var)==0) {
    if (opt('g','3')) {
      println("; optimize ldx _%s",var->name);
    }
    return;
  }
  println("\tldx _%s",var->name);
  IX_Dest = IX_EXT;
  IX_EXT_var = var->name;
}

void cpx_EXT(Node *node)
{
  assert(node->kind == ND_VAR);
  Obj *var = node->var;
  assert(!var->is_local);

  println("\tcpx _%s",var->name);
}

void stx_EXT(Node *node)
{
  assert(node->kind == ND_VAR);
  Obj *var = node->var;
  assert(!var->is_local);

  println("\tstx _%s",var->name);
  IX_Dest = IX_EXT;
  IX_EXT_var = var->name;
}

void invalidate_EXT(Node *node)
{
  assert(node->kind == ND_VAR);
  Obj *var = node->var;
  assert(!var->is_local);

  if (IX_Dest == IX_EXT
  &&  strcmp(var->name,IX_EXT_var)==0) {
    IX_invalidate();
  }
}

void ldx_IMM_STR(char *s)
{
  if (IX_Dest == IX_IMM_STR
  &&  strcmp(IX_IMM_str,s)==0) {
    if (opt('g','3')) {
      println("; optimize ldx #%s",s);
    }
    return;
  }
  println("\tldx #%s",s);
  IX_Dest    = IX_IMM_STR;
  IX_IMM_str = s;
}

void ldx_IMM_VAR(char *s)
{
  char *p = calloc(1,strlen(s)+2);
  strcat(strcpy(p,"_"),s);
  ldx_IMM_STR(p);
}

void tfr_dx()
{
  println("\tjsr __tfr_dx");
  IX_invalidate();
}

static void load32x(int off)
{
  if (off==0) {
    println("\tjsr __load32x");
  }else if (1<=off && off<=255) {
    ldab_i(off);
    println("\tjsr __load32bx");
    IX_invalidate();
  }else{
    ldd_i(off);
    println("\tjsr __load32dx");
    IX_invalidate();
  }
}

//
// store @long to off,x
//
static void store32x(int off)
{
  if (off == 0) {
    println("\tjsr __store32x");
  }else if (1<=off && off<=255) {
    ldab_i(off);
    println("\tjsr __store32bx");
    IX_invalidate();
  }else{
    ldd_i(off);
    println("\tjsr __store32dx");
    IX_invalidate();
  }
  return;
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
// node is local array ?
//
bool is_local_array(Node *node)
{
  if (node->kind != ND_VAR)
    return 0;
  if (node->var->ty->kind == TY_VLA)
    return 0;
  if (!node->var->is_local)
    return 0;
  if (node->ty->kind == TY_ARRAY)
     return 1;
  return 0;
}

//
// node is local array with constant subscript
//
// (ND_DEREF ty_uchar (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) g global) 0))
//
bool is_local_array_with_constant(Node *node)
{
  int64_t val;

  if (node->kind  == ND_DEREF
  &&  node->lhs->kind == ND_ADD
  &&  is_local_array(node->lhs->lhs)
  &&  is_integer_constant(node->lhs->rhs,&val)) {
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
// (ND_DEREF ty_uchar (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) g global) 0))
//
bool is_global_array_with_constant(Node *node)
{
  int64_t val;

  if (node->kind  == ND_DEREF
  &&  node->lhs->kind == ND_ADD
  &&  is_global_array(node->lhs->lhs)
  &&  is_integer_constant(node->lhs->rhs,&val)) {
    return 1;
  }

  return 0;
}

Type *is_integer_constant(Node *node, int64_t *val)
{
  if (!node->ty)
    return NULL;

  if (node->kind != ND_NUM || !is_integer_or_ptr(node->ty))
    return NULL;

  if (val) {
    *val = node->val;
  }

  return node->ty;
}


Type *is_pointer_constant(Node *node, int64_t *val)
{
  if (node->kind == ND_CAST && node->ty->kind == TY_PTR) {
    node = node->lhs;
  }
  if (node->kind != ND_NUM || !is_integer_or_ptr(node->ty)){
    return NULL;
  }

  if (val) {
    *val = node->val;
  }

  return node->ty;
}

Type *is_long_constant(Node *node, int64_t *val)
{
  if (node->kind == ND_CAST && node->ty->kind == TY_LONG) {
    node = node->lhs;
  }
  if (node->kind != ND_NUM || !is_integer(node->ty)) {
    return NULL;
  }
  if (val) {
    *val = node->val;
  }

  return node->ty;
}

Type *is_flonum_constant(Node *node, double *val)
{
  if (node->kind != ND_NUM ||!is_flonum(node->ty)) {
    return NULL;
  }

  *val = node->fval;

  return node->ty;
}

//
// cf. https://www.zukeran.org/shin/d/2024/12/30/6800-programing-11/
//
void gen_shl(Type *ty, uint64_t val)
{
  assert(is_int8(ty)||is_int16(ty));
  if (val==0) return;

  if (val>=ty->size*8){     // TODO: bit field
    switch(ty->size) {
    case 1: println("\tclrb");
            return;
    case 2: println("\tclrb");
            println("\tclra");
            return;
    default:assert(0);
    }
  }
  switch(ty->size){
  case 1:
    switch(val) {
    case 6:                 // 8cyc, 5bytes
      println("\trorb");
      println("\trorb");
      println("\trorb");
      println("\tandb #$C0");
      return;
    case 7:                 // 6cyc, 4bytes
      println("\trorb");
      println("\trorb");
      println("\tandb #$80");
      return;
    }
    for (int i=0; i<val; i++) {   // 1-5,  2*n cyc, n bytes
      println("\taslb");
    } 
    return;
  case 2: {                 // short and int
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
      return;
    case 7:                 // 10cyc, 6bytes
      println("\tlsra");
      println("\ttba");
      println("\trora");
      println("\trorb");
      println("\tandb #$80");
      return;
    case 14:                // 12cyc, 6bytes
    case 15:                // 8cyc,  4bytes
      println("\tclra");
      for (int i=0; i<16-val; i++) {
        println("\tlsrb");
        println("\trora");
      }
      println("\tclrb");
      return;
    }
    if (val>=8) {                 // 8-13, 2*(n-8)+4 cyc, (n-8)+2 bytes
      println("\ttba");
      println("\tclrb");
      for (int i=0; i<val-8; i++) {
        println("\tasla");
      }
      return;
    }
    for (int i=0; i<val; i++) {   // 1-5, 4*n cyc, 2*n bytes
      println("\taslb");
      println("\trola");
    } 
    return;
  }
  }
  assert(0); // 1 and 2 bytes only. gen_direct_shl_long does long.
}

void gen_shr(Type *ty, uint64_t val)
{
  assert (is_int8(ty)||is_int16(ty));
  if (val==0) return;

  if (val>=ty->size*8) {
    if (ty->is_unsigned) {
      switch(ty->size) {
      case 1: println("\tclrb");
              return;
      case 2: println("\tclrb");
              println("\tclra");
              return;
      default:assert(0);
      }
    }else{ // signed
      switch(ty->size) {
      case 1: println("\taslb");
              println("\tldab #0");
              println("\tsbcb #0");
              return;
      case 2: println("\tclrb");
              println("\tasla");
              println("\tsbcb #0");
              println("\ttba");
              return;
      default:assert(0);
      }
    }
    return;
  }
  switch(ty->size){
  case 1:
    for (int i=0; i<val; i++) {
      if (ty->is_unsigned) {
        println("\tlsrb");
      }else{
        println("\tasrb");
      } 
    }
    return;
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
        return;
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
        return;
      case 7:   // 10cyc, 6bytes
        println("\taslb");
        println("\trola");
        println("\ttab");
        println("\trola");
        println("\tanda #$01");
        return;
      case 8:   // 4cyc,  4bytes
      case 9:   // 6cyc,  5bytes
      case 10:  // 8cyc,  6bytes
      case 11:  // 10cyc, 7bytes
      case 12:  // 12cyc, 7bytes
      case 13:  // 14cyc, 8bytes
        println("\ttab");
        for (int i=8; i<val; i++) {
          println("\tlsrb");
        }
        println("\tclra");
        return;
      case 14:  // 12cyc, 6bytes
      case 15:  // 8cyc,  4bytes
        println("\tclrb");
        for (int i=val; i<=15; i++) {
          println("\tasla");
          println("\trolb");
        }
        println("\tclra");
        return;
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
        return;
      case 7:   // 10cyc, 6bytes
        println("\taslb");
        println("\trola");
        println("\ttab");
        println("\tsbca #0");
        println("\tsba");
        return;
      // https://www.zukeran.org/shin/d/2024/12/30/6800-programing-11/
      case 8:   // 10cyc,  5bytes.
      case 9:   // 12cyc,  6bytes
      case 10:  // 14cyc,  7bytes
      case 11:  // 16cyc,  8bytes
      case 12:  // 18cyc,  9bytes
      case 13:  // 20cyc,  10bytes
      {
        char *label = new_jump_label();
        println("\ttab");
        println("\tbpl %s",label);
        println("\tdeca");
        println("%s:",label);
        println("\tsba");
        for (int i=8; i<val; i++) {
          println("\tasrb");
        }
        return;
      }
      case 14:  // 12cyc, 7bytes
        println("\tclrb");
        println("\tasla");
        println("\tsbcb #0");
        println("\tasla");
        println("\ttba");
        println("\trolb");
        return;
      case 15:  // 8cyc,  5bytes
        println("\tclrb");
        println("\tasla");
        println("\tsbcb #0");
        println("\ttba");
        return;
      }
    }
    return;
  }
  assert(0); // 1 and 2 bytes only. gen_direct_shr_long does long.
}


// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
void gen_addr(Node *node)
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

void gen_expr_x(Node *node);
int gen_decayed_x(Node *node);
bool test_decayed_x(Node *node);
bool test_expr_x(Node *node);
static int addr_x_offset(Node *node);

// Find the global variable at the bottom of an address constant.
// &gd.in.arr[1] -> Obj *gd, *off = 0 + 4 + 2
// (ND_ADD (ND_MEMBER arr:4 (ND_MEMBER in:0 (ND_VAR gd))) 2)
static Obj *find_base_var(Node *node, int64_t *off)
{
  int64_t val;

  if (node->kind == ND_MEMBER
  &&  !node->member->is_bitfield) {
    *off += node->member->offset;
    return find_base_var(node->lhs,off);
  }
  if (node->kind == ND_DEREF
  &&  (node->lhs->ty->kind == TY_ARRAY
    || (node->lhs->kind == ND_ADD && is_decay_type(node->lhs->lhs->ty)))) {
    return find_base_var(node->lhs,off);
  }
  if (node->kind == ND_ADD
  &&  is_decay_type(node->lhs->ty)
  &&  is_integer_constant(node->rhs,&val)) {
    *off += val;
    return find_base_var(node->lhs,off);
  }
  if (node->kind != ND_VAR)
    return NULL;
  if (node->var->ty->kind == TY_VLA)
    return NULL;
  if (node->var->is_local)
    return NULL;
  if (node->ty->kind == TY_FUNC)
    return NULL;
  return node->var;
}

// Find the fixed address of an lvalue. The address of a local variable
// depends on the frame pointer, so a local variable returns NULL.
char *is_var_addr_constant(Node *node)
{
  int64_t off = 0;
  Obj *var = find_base_var(node,&off);

  if (var) {
    char *p = calloc(1,strlen(var->name)+32);
    if (off==0) {
      sprintf(p,"_%s",var->name);
    }else{
      sprintf(p,"_%s%+ld",var->name,off);
    }
    return p;
  }
  return NULL;
}

// Find the global variable under an expression whose value is an address.
// garr + 1     -> the array itself is an address
// &gd.in.x + 1 -> the operand of '&' is an object, so find_base_var() takes over
static Obj *find_base_addr(Node *node, int64_t *off)
{
  int64_t val;

  if (node->kind == ND_ADDR)
    return find_base_var(node->lhs,off);
  if (node->kind == ND_ADD
  &&  node->ty->kind == TY_PTR
  &&  is_integer_constant(node->rhs,&val)) {
    *off += val;
    return find_base_addr(node->lhs,off);
  }
  if (node->kind == ND_SUB
  &&  node->ty->kind == TY_PTR
  &&  is_integer_constant(node->rhs,&val)) {
    *off -= val;
    return find_base_addr(node->lhs,off);
  }
  if (node->ty
  &&  node->ty->kind == TY_ARRAY)
    return find_base_var(node,off);
  return NULL;
}

// Return a constant expression usable after '#', or NULL.
// The value must be an address, not the contents of one.
// (!= ty_int (ND_CAST TY_PTR(10):u (ND_VAR TY_ARRAY(12) arr global)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
// (!= ty_int (ND_CAST TY_PTR(10):u (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) arr global) 0)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
// (!= ty_int (ND_CAST TY_PTR(10):u (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) arr global) 100)) (ND_CAST TY_PTR(10):u (ND_VAR TY_PTR(10) _L_5 global)))
char *is_addr_constant(Node *node)
{
  int64_t off = 0;
  Obj *var = NULL;

  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_PTR) {
    node = node->lhs;
  }
  if (node->kind == ND_VAR
  &&  node->ty->kind == TY_FUNC) {
    char *p = calloc(1,strlen(node->var->name)+2);
    sprintf(p,"_%s",node->var->name);
    return p;
  }
  if (node->kind == ND_ADDR
  &&  node->lhs->kind == ND_VAR
  &&  node->lhs->ty->kind == TY_FUNC) {
    char *p = calloc(1,strlen(node->lhs->var->name)+2);
    sprintf(p,"_%s",node->lhs->var->name);
    return p;
  }
  var = find_base_addr(node,&off);
  if (var) {
    char *p = calloc(1,strlen(var->name)+32);
    if (off==0) {
      sprintf(p,"_%s",var->name);
    }else{
      sprintf(p,"_%s%+ld",var->name,off);
    }
    return p;
  }
  return NULL;
}

bool gen_expr_x_sub(Node *node,bool test)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;
  int64_t val;
  char *addr;

  if (node->kind == ND_MEMBER
  &&  node->member->is_bitfield) {
    return false;
  }
  if (!is_int16_or_ptr(node->ty)) {
    if (test) return false;
    assert(0);
  }
  if ((addr=is_addr_constant(node))) {
    if (test) return true;
    ldx_IMM_STR(addr);
    return false;
  }
  switch(node->kind) {
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
      return false;
    case TY_LONG:
      assert(0);
      if (test) return true;
      println("\tldx @long+2");
      IX_invalidate();
      return false;
    case TY_BOOL:
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
    case TY_PTR:
    case TY_ENUM:
      if (test) return true;
      println("\tldx #%d",(unsigned int)((node->val & 0x0ffff)));
      IX_invalidate();
      return false;
    }
    return false;
  } // ND_NUM
  case ND_NEG:
    return false;
  case ND_VAR:
    if (is_int16_or_ptr(node->ty)) {
      if (is_global_var(node)) {
        if (test) return true;
        ldx_EXT(node);
        return false;
      }
      if (is_local_var(node)) {
        off = node->var->offset;
        if (test) return (0<=off && off<256);
        ldx_bp_nX(off);
        return false;
      }
    }
    if (test_addr_x(node)) {
      if (test) return true;
      ldx_nX(gen_addr_x(node));
      return false;
    }
    return false;
  case ND_MEMBER: {
    if ((addr=is_var_addr_constant(node))) {
      if (test) return true;
      println("\tldx %s",addr);
      IX_invalidate();
      return false;
    }
    if (test_addr_x(node)) {
      if (test) return true;
      ldx_nX(gen_addr_x(node));
      return false;
    }
    return false;
  }; // ND_MEMBER:
  case ND_DEREF: {
  //(ND_DEREF TY_PTR(10) (+ TY_PTR(10) (ND_VAR TY_PTR(10) p +0 ) 4))
    if (node->lhs->kind == ND_ADD       // XXX
    &&  node->lhs->ty->kind == TY_PTR
    &&  node->lhs->lhs->kind == ND_VAR
    &&  node->lhs->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->rhs,&val)) {
      if (is_local_var(node->lhs->lhs)
      &&  (0 <= node->lhs->lhs->var->offset && node->lhs->lhs->var->offset<256)
      &&  (0 <= val && val<256)) {
        if (test) return true;
        ldx_bp();
        ldx_nX(node->lhs->lhs->var->offset);
        ldx_nX(val);
        return false;
      }
      if (is_global_var(node->lhs->lhs)
      &&  (0 <= val && val<256)) {
        if (test) return true;
        ldx_EXT(node->lhs->lhs);
        ldx_nX(val);
        return false;
      }
    }
    if ((addr=is_addr_constant(lhs))) {
      if (test) return true;
      println("\tldx %s",addr);
      IX_invalidate();
      return false;
    }
    if (test_decayed_x(lhs)) {
      if (test) return true;
      ldx_nX(gen_decayed_x(lhs));
      return false;
    }
    if (test_expr_x(lhs)) {
      if (test) return true;
      gen_expr_x(lhs);
      ldx_nX(0);
      return false;
    }
    return false;
  } // ND_DEREF
  case ND_POST_INCDEC:
    if (is_int16_or_ptr(node->ty)
    &&  lhs->kind==ND_VAR) {
      if (is_global_var(lhs)) {
        int64_t val;
        if (is_integer_constant(rhs,&val)) {
          switch(val) {
          case 1:
          case 2:
          case -1:
          case -2:
            if (test) return true;
            ldx_EXT(lhs);
            inx_dex(val);
            stx_EXT(lhs);
            inx_dex(-val);
            return false;
          }
        }
      }
      if (is_local_var(lhs)) {
        if (test_addr_x(lhs)){
          int64_t val;
          if (is_integer_constant(rhs,&val)) {
            int off;
            char *label;
            switch(val) {
            case 1:
              if (test) return true;
              off = gen_addr_x(lhs);
              label = new_jump_label();
              println("\tinc %d+1,x",off);
              println("\tbne %s",label);
              println("\tinc %d,x",off);
              println("%s:",label);
              println("\tldx %d,x",off);
              println("\tdex");
              IX_invalidate();
              return false;
            case -1:
              if (test) return true;
              off = gen_addr_x(lhs);
              label = new_jump_label();
              println("\ttst %d+1,x",off);
              println("\tbne %s",label);
              println("\tdec %d,x",off);
              println("%s:",label);
              println("\tdec %d+1,x",off);
              println("\tldx %d,x",off);
              println("\tinx");
              IX_invalidate();
              return false;
            }
          }
        }
      }
    }
    return false;
  // (ND_PRE_INCDEC (ND_VAR TY_PTR(10) _L_44 global) 2)
  case ND_PRE_INCDEC:
    if (is_int16_or_ptr(node->ty)
    &&  lhs->kind==ND_VAR) {
      if (is_global_var(lhs)) {
        int64_t val;
        if (is_integer_constant(rhs,&val)) {
          switch(val) {
          case 1:
          case 2:
          case -1:
          case -2:
            if (test) return true;
            ldx_EXT(lhs);
            inx_dex(val);
            stx_EXT(lhs);
            return false;
          }
        }
      }
    }
    return false;
  // (ND_ADDR (ND_VAR TY_FLOAT(6) f +0 ))
  // (ND_ADDR (ND_DEREF ty_uchar (ND_CAST TY_PTR(10) 63060)))
  case ND_ADDR: {
    if (node->lhs->kind == ND_DEREF) {
      if (test_expr_x(node->lhs->lhs)) {
        if (test) return true;
        gen_expr_x(node->lhs->lhs);
      }
      return false;
    }
    if (is_local_var(lhs)) {
      if (test)
        return (abs(lhs->var->offset)<=2);
      ldx_bp();
      switch(lhs->var->offset) {
      case -2:
      case -1:
      case 1:
      case 2:
        inx_dex(lhs->var->offset);
        break;
      // case 0: // do nothing
      }
      return false;
    }
    return false;
  } // ND_ADDR;
  case ND_ASSIGN: {
      if (opt('g','3')) {
        println("; %s ND_ASSIGN %s %d",__func__,__FILE__,__LINE__);
      }
    }
    return false;
  case ND_SHLEQ:
    if (is_int16_or_ptr(node->ty)
    &&  test_addr_x(node->lhs)
    &&  is_integer_constant(node->rhs,&val)
    &&  val==1) {
      if (test) return true;
      int off = gen_addr_x(node->lhs);
      println("\tasl %d,x",off+1);
      println("\trol %d,x",off);
      ldx_nX(off);
      return false;
    }
    return false;
//  (>>= ty_ushort (ND_VAR ty_ushort t +4 ) (ND_NUM TY_CHAR(2) 1))
  case ND_SHREQ:
    if (is_int16_or_ptr(node->ty)
    &&  test_addr_x(node->lhs)
    &&  is_integer_constant(node->rhs,&val)
    &&  val==1) {
      if (test) return true;
      int off = gen_addr_x(node->lhs);
      if (node->lhs->ty->is_unsigned) {
        println("\tlsr %d,x",off);
        println("\tror %d,x",off+1);
      }else{
        println("\tasr %d,x",off);
        println("\tror %d,x",off+1);
      }
      ldx_nX(off);
      return false;
    }
    return false;
//case ND_STMT_EXPR:
//case ND_COMMA:
  case ND_CAST: {
    if (!is_int16_or_ptr(node->ty)) {
      return false;
    }
    // (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) p +0 ))
    if (node->kind == ND_CAST
    &&  node->ty->kind == TY_PTR
    &&  is_local_array(node->lhs)
    &&  node->lhs->var->offset<=6) {
      if (test) return true;
      ldx_bp();
      inx_dex(node->lhs->var->offset);
      return false;
    }
    if (!is_int16_or_ptr(node->lhs->ty)) {
      return false;
    }
    if (test_expr_x(lhs)) {
      if (test) return true;
      gen_expr_x(lhs);
      return false;
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
  //; (+ ty_int (ND_VAR ty_int y +0 ) 1)
  case ND_ADD:
  case ND_SUB:
    if (is_int16(node->ty)
    &&  is_int16(lhs->ty)
    &&  test_expr_x(lhs)
    &&  is_integer_constant(rhs,&val)) {
      if (node->kind == ND_SUB) {
        val = -val;
      }
      switch(val) {
      case 0:
        if (test) return true;
        gen_expr_x(lhs);
        return false;
      case 1:
      case 2:
      case -1:
      case -2:
        if (test) return true;
        gen_expr_x(lhs);
        inx_dex(val);
        return false;
      }
    }
    return false;
  default:
  }
  if (test) {
    return false;
  }
  error_tok(node->tok, "invalid expression at %s node->kind %d",__func__,node->kind);
}

void gen_expr_x(Node *node)
{
  gen_expr_x_sub(node,false);
}

// C11 6.3.2.1p3,p4: an array or a function is converted to a pointer, so the
// value is the address itself. The 6800 has no LEA, so the address is left at
// IX+off instead of being materialized in IX. Returns off, or -1 if not applicable.
int gen_decayed_x_sub(Node *node,bool test)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;
  int64_t val;
  char *addr;

  if (!is_decay_type(node->ty)
  &&  !(node->kind == ND_ADD && is_decay_type(lhs->ty))) {
    return -1;
  }
  switch (node->kind) {
  case ND_VAR:
  case ND_MEMBER:
    if (!test_addr_x(node)) {
      return -1;
    }
    if (test) return 0;
    return gen_addr_x(node);
  case ND_DEREF:
    if (is_decay_type(lhs->ty)) {
      return gen_decayed_x_sub(lhs,test);
    }
    if (!test_expr_x(lhs)) {
      return -1;
    }
    if (test) return 0;
    gen_expr_x(lhs);
    return 0;
  case ND_ADD:
    //(+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) ua +0 ) 6)
    if (lhs->ty->kind == TY_ARRAY
    &&  is_integer_constant(rhs,&val)) {
      off = addr_x_offset(lhs);
      if (0 <= off && 0 <= val && off + val <= 252) {
        if (test) return 0;
        return gen_addr_x(lhs) + val;
      }
    }
    break;
  }
  if ((addr=is_addr_constant(node))) {
    if (test) return 0;
    ldx_IMM_STR(addr);
    return 0;
  }
  return -1;
}

int gen_decayed_x(Node *node)
{
  return gen_decayed_x_sub(node,false);
}

bool test_decayed_x(Node *node)
{
  return 0 <= gen_decayed_x_sub(node,true);
}

bool test_expr_x(Node *node)
{
  return  gen_expr_x_sub(node,true);
}


// Offset from the base address that gen_addr_x() loads into IX, or -1 if unknown.
static int addr_x_offset(Node *node)
{
  int64_t val;
  int off;

  if (node->kind == ND_MEMBER
  &&  node->member->is_bitfield) {
    return -1;
  }
  if (is_global_var(node) || is_global_array(node)) {
    return 0;
  }
  if (is_local_var(node)) {
    return node->var->offset;
  }
  if (node->kind == ND_MEMBER) {
    off = addr_x_offset(node->lhs);
    return (off < 0) ? -1 : off + node->member->offset;
  }
  if (node->kind == ND_DEREF) {
    if (node->lhs->kind == ND_ADD
    &&  is_integer_constant(node->lhs->rhs,&val)
    &&  (0<=val && val<=252)) {
      if (node->lhs->ty->kind == TY_PTR
      &&  node->lhs->lhs->ty->kind == TY_PTR
      &&  is_local_var(node->lhs->lhs)
      &&  node->lhs->lhs->var->offset<=252) {
        return val;
      }
      if (is_decay_type(node->lhs->lhs->ty)) {
        if (is_global_array(node->lhs->lhs)) {
          return 0;
        }
        off = addr_x_offset(node->lhs->lhs);
        return (off < 0) ? -1 : off + val;
      }
    }
    // gen_expr_x() loads the pointer itself into IX and returns 0
    if (can_load_x(node->lhs->ty) && test_expr_x(node->lhs)) {
      return 0;
    }
  }
  return -1;
}

static Node *skip_empty_cast(Node *node);

// Compute the absolute address of a given node in IX.
// Returns the offset from that address, or -1 if the node does not reside in memory.
int gen_addr_x_sub(Node *node,bool test)
{
  Node *lhs = node->lhs;
//Node *rhs = node->rhs;
  Node *addr;
  int off;
  int64_t val;

  if (node->kind == ND_MEMBER
  &&  node->member->is_bitfield) {
    return -1;
  }
  switch (node->kind) {
  case ND_VAR:
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA){
      if (node->var->offset<=252) {
        if (test) return 0;
        println("; gen_addr_x():TY_LDA,%d ",node->var->offset);
        ldx_bp_nX(node->var->offset);
        return 0;
      }
      goto fallback;
    }
    // Local variable
    if (node->var->is_local) {
      if (node->var->offset <= 252){
        if (test) return 0;
        ldx_bp();
        return node->var->offset;
      }
      goto fallback;
    }
    // Function and Global variable
    if (test) return 0;
    ldx_IMM_VAR(node->var->name);
    return 0;
  case ND_DEREF:
    // (ND_DEREF ty_uint (ND_CAST TY_PTR(10):u (ND_ADDR (ND_VAR TY_FLOAT y +0 ))))
    addr = skip_empty_cast(node->lhs);
    if (addr->kind == ND_ADDR) {
      return gen_addr_x_sub(addr->lhs,test);
    }
    // (ND_DEREF ty_uint (+ TY_PTR(10):u (ND_CAST TY_PTR(10):u (ND_ADDR (ND_VAR TY_FLOAT y +0 ))) 2))
    if (node->lhs->kind == ND_ADD
    &&  node->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->rhs,&val)) {
      addr = skip_empty_cast(node->lhs->lhs);
      if (addr->kind == ND_ADDR) {
        off = addr_x_offset(addr->lhs);
        if ((0 <= off) && (off + val <= 252)) {
          return gen_addr_x_sub(addr->lhs,test) + val;
        }
        return -1;
      }
    }
    // (ND_DEREF ty_uchar (ND_POST_INCDEC (ND_VAR TY_PTR(10) src +8 ) 1))
    if (node->lhs->kind == ND_POST_INCDEC
    &&  (is_int8(node->ty) || is_int16_or_ptr(node->ty))
    &&  test_addr_x(node->lhs->lhs)
    &&  node->lhs->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->rhs,&val)
    &&  val==1 ){
      if (test) return 0;
      if (is_global_var(node->lhs->lhs)) {
        ldx_EXT(node->lhs->lhs);
        println("\tinx");
        stx_EXT(node->lhs->lhs);
      }else{
        int off = gen_addr_x(node->lhs->lhs);
        char *label = new_jump_label();
        println("\tinc %d,x",off+1);
        println("\tbne %s",label);
        println("\tinc %d,x",off);
        println("%s:",label);
        println("\tldx %d,x",off);
      }
      println("\tdex");
      IX_invalidate();
      return 0;
    }
    if (node->lhs->kind == ND_ADD) {
      int64_t val;
         
      if ((node->lhs->ty->kind == TY_PTR)
      &&  (node->lhs->lhs->kind == ND_VAR)
      &&  (node->lhs->lhs->ty->kind == TY_PTR)
      &&   is_local_var(node->lhs->lhs)
      &&  (node->lhs->lhs->var->offset<=252)
      &&  is_integer_constant(node->lhs->rhs,&val)
      &&  (0<=val && val<=252)) {
        if (test) return 0;
        ldx_bp_nX(node->lhs->lhs->var->offset);
        return val;
      }
      if ((node->lhs->ty->kind == TY_PTR)
      &&  (node->lhs->lhs->kind == ND_VAR)
      &&   is_global_var(node->lhs->lhs)
      &&  is_integer_constant(node->lhs->rhs,&val)
      &&  (0<=val && val<=252)) {
        if (test) return 0;
        ldx_EXT(node->lhs->lhs);
        return val;
      }
      // (ND_DEREF ty_int (+ TY_ARRAY(12) ...  n))
      if (is_decay_type(node->lhs->lhs->ty)
      &&  is_integer_constant(node->lhs->rhs,&val)
      &&  (0<=val && val<=252)
      &&  test_addr_x(node->lhs->lhs)) {
        if (test) {
          if (is_global_array(node->lhs->lhs)) {
            return 0;
          }
          off = addr_x_offset(node->lhs->lhs);
          return ((0 <= off) && (off + val <= 252)) ? 0 : -1;
        }
        if (is_global_array(node->lhs->lhs)) {
          println("\tldx #_%s+%ld",node->lhs->lhs->var->name,val);
          IX_invalidate();
          return 0;
        }
        off = gen_addr_x(node->lhs->lhs);
        if (off+val <= 252) {
          return  off + val;
        }
        assert(0); // gen_addr_x() must not be called when the offset is over 252
      }
    }
    if (test_decayed_x(node->lhs)) {
      if (test) return 0;
      return gen_decayed_x(node->lhs);
    }
    if (test_expr_x(node->lhs)) {
      if (test) return 0;
      gen_expr_x(node->lhs);
      return 0;
    }
    return -1;
  case ND_COMMA:
    return -1;
  case ND_MEMBER:
    if (!test_addr_x(lhs)) {
      return -1;
    }
    if (test) {
      int off;
      if (is_global_var(lhs)) {
        return 0;
      }
      off = addr_x_offset(lhs);
      return ((0 <= off) && (off + node->member->offset <= 252)) ? 0 : -1;
    }
    if (is_global_var(node->lhs)
    &&  node->member->offset > 252) {
      println("\tldx #_%s+%d",node->lhs->var->name,node->member->offset);
      IX_invalidate();
      return 0;
    }
    off = gen_addr_x(node->lhs) + node->member->offset;
    if (off<=252) {
      return off;
    }
    assert(0); // gen_addr_x() must not be called when the offset is over 252
  case ND_FUNCALL:
    return -1;
  case ND_ASSIGN:
  case ND_COND:
    return -1;
  case ND_VLA_PTR:
    return -1;
  }
  if (test) return -1;
  error_tok(node->tok, "not an lvalue at gen_addr_x, node->kind %d",node->kind);
  // fallback to gen_addr()
fallback:
  if (test) return -1;
  assert(0); // test_addr_x() must have returned false
}

int gen_addr_x(Node *node)
{
  int off = gen_addr_x_sub(node,false);
  return off;
}

bool test_addr_x(Node *node)
{
  return 0 <= gen_addr_x_sub(node,true);
}

int gen_addr_array_sub(Node *node,bool test)
{
  int64_t val;

// (ND_DEREF ty_uchar (+ TY_ARRAY(12) (ND_VAR TY_ARRAY(12) ua +16 ) (ND_VAR ty_int i +0 )))
  if (node->kind == ND_DEREF
  &&  node->lhs->kind == ND_ADD
  &&  is_local_array(node->lhs->lhs)
  &&  node->lhs->lhs->var->offset <=252
  &&  is_local_var(node->lhs->rhs)
  &&  node->lhs->rhs->var->offset <=254) {
    if (is_int16(node->lhs->rhs->ty)) {
      if (test) return true;
      ldx_bp();
      println("\tldab %d,x",node->lhs->rhs->var->offset+1);
      println("\tldaa %d,x",node->lhs->rhs->var->offset);
      println("\taddb @bp+1");
      println("\tadca @bp");
      tfr_dx();
      return node->lhs->lhs->var->offset;
    }
  }
  // (ND_DEREF ty_int (+ TY_ARRAY(12) (* ty_int (ND_VAR ty_int l +16 ) 2) (ND_VAR TY_ARRAY(12) m +2 )))
  if (node->kind == ND_DEREF
  &&  node->lhs->kind == ND_ADD
  &&  is_local_array(node->lhs->rhs)
  &&  node->lhs->rhs->var->offset <=252
  &&  node->lhs->lhs->kind == ND_MUL
  &&  node->lhs->lhs->ty == ty_int
  &&  is_integer_constant(node->lhs->lhs->rhs,&val)
  &&  val==2
  &&  is_local_var(node->lhs->lhs->lhs)
  &&  node->lhs->lhs->lhs->var->offset <=254) {
    if (is_int16(node->lhs->lhs->lhs->ty)) {
      if (test) return true;
      ldx_bp();
      println("\tldab %d,x",node->lhs->lhs->lhs->var->offset+1);
      println("\tldaa %d,x",node->lhs->lhs->lhs->var->offset);
      println("\taslb");
      println("\trola");
      println("\taddb @bp+1");
      println("\tadca @bp");
      tfr_dx();
      return node->lhs->rhs->var->offset;
    }
  }

  return false;
}

bool test_addr_array(Node *node)
{
  return gen_addr_array_sub(node,true);
}

int gen_addr_array(Node *node)
{
  return gen_addr_array_sub(node,false);
}

static void word32i(uint32_t val)
{
  uint16_t hi = (uint16_t)((val>>16)&0x0ffff);
  uint16_t lo = (uint16_t)(val&0x0ffff);

  println("\t.word %u",hi);
  println("\t.word %u",lo);
}

static void load32i(uint32_t val)
{
  uint16_t hi = (uint16_t)((val>>16)&0x0ffff);
  uint16_t lo = (uint16_t)(val&0x0ffff);

  if (opt('O','s') && hi!=lo) {
    println("\tjsr __load32i");
    word32i(val);
    IX_invalidate();
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
  IX_invalidate();
}

// Load a value from where AccA:B is pointing to.
//
//   __load8 sets AccB only. Treat AccA as undefined.
//   __load16 sets AccA:B.
//   __load32 sets @long.
//   All of them leave the address in IX, but this compiler does not use it.
void load(Type *ty) {
  switch (ty->kind) {
  case TY_ARRAY:
  case TY_STRUCT:
  case TY_UNION:
  case TY_FUNC:
  case TY_VLA:
    // These cannot fit in a register, so the address becomes the value.
    // This is where "an array becomes a pointer to its first element" happens.
    return;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    println("\tjsr __load32");		// @long = (AccAB)");
    IX_invalidate();
    return;
  }

  if (ty->size == 1){
    println("\tjsr __load8");
  }else if (ty->size == 2){
    if (opt('O','2')) {
      tfr_dx();
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
  IX_invalidate();
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
  case TY_FLOAT:
    return true;
  }
  return is_integer(ty);
}

void load_x(Type *ty,int off) {
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
  case TY_DOUBLE:
  case TY_LDOUBLE:
    load32x(off);
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
    load32x(off);
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
      if (opt('O','2')) {
        println("\tldx _%s+2",node->var->name);
        println("\tstx @long+2");
        println("\tldx _%s",  node->var->name);
        println("\tstx @long");
        IX_invalidate();
      }else{
        ldx_IMM_VAR(node->var->name);
        load32x(0);
      }
      break;
    default:
      assert(0);
    }
    return;
  }
  if (test_addr_x(node)) {
    int off = gen_addr_x(node);
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
    println("\tldx #%d",ty->size);
    println("\tjsr  __copy_struct");	// remove tos
    depth -= 2;
    IX_invalidate();
    return;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    popx();
    store32x(0);                      // store @long to (0-3,x)");
    return;
  }

  popx();
  if (ty->size == 1){
    println("\tstab 0,x");
  }else if (ty->size == 2){
    println("\tstab 1,x");
    println("\tstaa 0,x");
  }else if (ty->size == 4){
    store32x(0);                      // store @long to (0-3,x)");
  }else
    assert(0);
}

static void clr_x(Type *ty,int off) {
  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    assert(0);
    return;
  }
  switch (ty->size) {
  case 1:
    println("\tclr %d,x",off);
    break;
  case 2:
    println("\tclr %d,x",off+1);
    println("\tclr %d,x",off);
    break;
  case 4:
    if (opt('O','s')) {
      println("\tclr %d,x",off+3);
      println("\tclr %d,x",off+2);
      println("\tclr %d,x",off+1);
      println("\tclr %d,x",off);
    }else{
      println("\tclrb");
      println("\tstab %d,x",off+3);
      println("\tstab %d,x",off+2);
      println("\tstab %d,x",off+1);
      println("\tstab %d,x",off);
    }
    break;
  default:
    println("  mov %%rax, (%%rdi)");
    assert(0);
  }
}

void store_x(Type *ty,int off) {
  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    println("; store struct/union from *TOS to IX, size %d",ty->size);
    push();
    if (off!=0) {
      ldd_i(off);
      adx();
    }
    ldd_i(ty->size);
    println("\tjsr  __copy_struct2 ; store_X");
    depth -= 2; // copy_struct2 remove *TOS
    IX_invalidate();
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
    store32x(off);
    break;
  default:
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
    return;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    println("\tjsr __f32iszero");
    return;
  }
  println("\taba");
  println("\tadca #0");
  return;
  //error("; cmp_zero ty: %d",ty->kind);
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
  case TY_ENUM:
    return ty->is_unsigned ? U16 : I16;
  case TY_LONG:
    return ty->is_unsigned ? U32 : I32;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    return F32;
  }
  return U16; // TY_PTR
}

// The table for type casts
// signed char to:
static char i8i16[]  = "clra\n\tasrb\n\trolb\n\tsbca #0";
static char i8u16[]  = "clra\n\tasrb\n\trolb\n\tsbca #0";
static char i8i32[]  = "jsr __s8to32";
static char i8u32[]  = "jsr __s8to32";
static char i8i64[]  = ";jsr __s8to64";
//static char i8u64[]  = ";jsr __s8to64";
static char i8f32[]  = "clra\n\tasrb\n\trolb\n\tsbca #0\n\tjsr __i16tof32";
static char i8f64[]  = ";jsr __u8tof64";
// unsigned char to:
static char u8i16[]  = "clra";
static char u8u16[]  = "clra";
static char u8i32[]  = "jsr __u8to32";
static char u8u32[]  = "jsr __u8to32";
static char u8i64[]  = ";jsr __u8to64";
//static char u8u64[]  = ";jsr __u8to64";
static char u8f32[]  = "clra\n\tjsr __u16tof32";
static char u8f64[]  = ";jsr __u8tof64";
// signed int to:
static char i16i32[] = "jsr __s16to32";
static char i16u32[] = "jsr __s16to32";
static char i16i64[] = ";jsr __i16i64 " __FILE__;
static char i16f32[] = "jsr __i16tof32";
static char i16f64[] = "; jsr __i16f64 " __FILE__;
// unsigned int to:
static char u16i32[] = "jsr __u16to32";
static char u16i64[] = ";jsr __u16i64 " __FILE__;
//static char u16u32[] = "jsr __u16to32";
static char u16f32[] = "jsr __u16tof32";
static char u16f64[] = ";jsr __u16tof32" __FILE__;
// signed long to:
static char i32i8[] = "ldab @long+3";
static char i32u8[] = "ldab @long+3";
static char i32i16[] = "ldab @long+3\n\tldaa @long+2";
static char i32u16[] = "ldab @long+3\n\tldaa @long+2";
static char i32f32[] = "jsr __i32tof32";
static char i32i64[] = "; movsxd %eax, %rax";
static char i32f64[] = "; cvtsi2sdl %eax, %xmm0";
// unsigned long to:
static char u32f32[] = "jsr __u32tof32";
static char u32i64[] = "; mov %eax, %eax";
static char u32f64[] = "; mov %eax, %eax; cvtsi2sdq %rax, %xmm0";
// float to:
static char f32i8[] = "jsr __f32toi8";
static char f32u8[] = "jsr __f32tou8";
static char f32i16[] = "jsr __f32toi16";
static char f32u16[] = "jsr __f32tou16";
static char f32i32[] = "jsr __f32toi32";
static char f32u32[] = "jsr __f32tou32";
static char f32i64[] = "; cvttss2siq %xmm0, %rax";
static char f32u64[] = "; cvttss2siq %xmm0, %rax";
static char f32f64[] = "; cvtss2sd %xmm0, %xmm0";
// long long not supported.
static char i64i32[] = "; i64i32 " __FILE__;
static char i64u32[] = "; i64u32 " __FILE__;
static char i64f32[] = "; cvtsi2ssq %rax, %xmm0";
static char i64f64[] = "; cvtsi2sdq %rax, %xmm0";
static char u64f32[] = "; cvtsi2ssq %rax, %xmm0";
static char u64f64[] =
  "; test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
  "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
  "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";
// double not supported
static char f64i8[] = "; cvttsd2sil %xmm0, %eax; movsbl %al, %eax";
static char f64u8[] = "; cvttsd2sil %xmm0, %eax; movzbl %al, %eax";
static char f64i16[] = "; cvttsd2sil %xmm0, %eax; movswl %ax, %eax";
static char f64u16[] = "; cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";
static char f64i32[] = "; cvttsd2sil %xmm0, %eax";
static char f64u32[] = "; cvttsd2siq %xmm0, %rax";
static char f64i64[] = "; cvttsd2siq %xmm0, %rax";
static char f64u64[] = "; cvttsd2siq %xmm0, %rax";
static char f64f32[] = "; cvtsd2ss %xmm0, %xmm0";

// ex. i32i16: i32->i16
static char *cast_table[][11] = {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  i8i16,   i8i32,  i16i64, NULL,  i8u16,   i8u32,  i8i64, i8f32, i8f64, NULL}, // i8
  {NULL,  NULL,   i16i32, i16i64, NULL,  NULL,   i16u32, i16i64, i16f32, i16f64, NULL}, // i16
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, NULL}, // i32
  {i32i8, i32i16, i64i32, NULL,   i32u8, i32u16, i64u32, NULL,   i64f32, i64f64, NULL}, // i64

  {NULL,  u8i16,   u8i32,  i16i64, NULL,  u8u16,   u8u32,  u8i64, u8f32, u8f64, NULL}, // u8
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
    if (from->kind == TY_BOOL)
      return;
    if (from->kind == TY_CHAR) {
      println("\tnegb");    // if AccB==0 then C=0 else C=1
      println("\trolb");    // C to AccB:b0
      println("\tandb #1"); // drop b7-b1
      return;
    }
    if (is_int16_or_ptr(from)) {
      cmp_zero(from);
      println("\tclrb");
      println("\tnega");
      println("\ttba");
      println("\trolb");
      return;
    }
    cmp_zero(from);         // if zero, Z=1 AccB=0 else Z=0 AccB!=0
    println("\tclra");
    println("\tnegb");      // if AccB==0 then C=0 else C=1
    println("\ttab");       // Zero AccB preserving carry flag
    println("\trolb");      // C to AccB
    return;
  }

  int t1 = getTypeId(from);
  int t2 = getTypeId(to);

  if (cast_table[t1][t2]){
    char *op = cast_table[t1][t2];
    println("\t%s", op);
    if(op[0]!=';' && strstr(op, "jsr"))
      if (!strstr(op, "__u8to32")
      &&  !strstr(op, "__s8to32")
      &&  !strstr(op, "__u16to32")
      &&  !strstr(op, "__s16to32")) {
        IX_invalidate();
      }
  }
}

static int is_empty_cast(Type *from, Type *to) {
  if (to->kind == TY_VOID)
    return 0;
  if (to->kind == TY_BOOL)
    return from->kind == TY_BOOL;
  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  return cast_table[t1][t2]==NULL;
}

static Node *skip_empty_cast(Node *node)
{
  while (node->kind == ND_CAST && is_empty_cast(node->lhs->ty, node->ty))
    node = node->lhs;
  return node;
}

static void push_struct(Type *ty) {
  if (opt('g','3')) {
    println("; %s %s %d",__func__,__FILE__,__LINE__);
    println("; stack depth = %d",depth);
  }
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
  if (opt('g','3')) {
    println("; stack depth = %d",depth);
  }
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
    ldab_i(val);
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
  if (opt('O','2')) {
    // push32/32x/32bx/32dx destroy IX, which may require reloading IX later.
    // Generating the value directly at -O2 can reduce this overhead.
    if (off==0) {
      println("\tldab 3,x");
      println("\tpshb");
      println("\tldab 2,x");
      println("\tpshb");
      println("\tldab 1,x");
      println("\tpshb");
      println("\tldab 0,x");
      println("\tpshb");
      depth+=4;
      return;
    }else if (1<=off && off<=252) {
      println("\tldab %d,x",off+3);
      println("\tpshb");
      println("\tldab %d,x",off+2);
      println("\tpshb");
      println("\tldab %d,x",off+1);
      println("\tpshb");
      println("\tldab %d,x",off);
      println("\tpshb");
      depth+=4;
      return;
    }
  }

  if (off==0) {
    println("\tjsr __push32x");
  }else if (1<=off && off<=255) {
    ldab_i(off);
    println("\tjsr __push32bx");
  }else{
    ldd_i(off);
    println("\tjsr __push32dx");
  }
  IX_invalidate();
  depth+=4;
}

static void push_args2(Node *args,Type *param_ty,bool is_variadic)
{
  if (!args)
    return;
  push_args2(args->next,param_ty? param_ty->next: NULL,is_variadic);
  if (opt('g','3')) {
    println("; push_args2");
    ast_node_dump(args);
  }
  switch (args->ty->kind) {
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
      ldab_i(val);
    }else{
      gen_expr(args);
    }
    if (args->pass_by_stack){
      if (is_variadic && !param_ty) {
        cast(args->ty, ty_int);
        push();
      }else{
        push1();
      }
    }
    break;
  } // TY_CHAR,TY_BOOL
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    if (args->pass_by_stack && args->kind==ND_NUM) {
      union { float f32; uint32_t u32; } u = { args->fval };
      println("; push float %e, 0x%08x",u.f32,u.u32);
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
    int64_t val;
    if (is_long_constant(args,&val)) {
      gen_direct_pushl(val);
    }else if (test_addr_x(args)){
      int off = gen_addr_x(args);
      pushlx(off);
    }else{
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
  // Determine register or stack passing and calculate the stack size
  int reg_passable = !node->ret_buffer;
  int stack = 0;
  Type *param_ty = node->lhs->ty->params;
  for (Node *arg = node->args; arg; arg = arg->next) {
    bool is_va_arg = !param_ty && node->lhs->ty->is_variadic;
    if (param_ty)
      param_ty = param_ty->next;
    // check first parameter
    switch (arg->ty->kind) {
    case TY_VOID:
      assert(0);
    case TY_STRUCT:
    case TY_UNION:
      reg_passable = 0;
      arg->pass_by_stack = 1;
      stack += arg->ty->size;
      break;
    default: // TY_BOOL,CHAR,SHORT,INT,LONG,FLOAT,ENUM,PTR,FUNC,TY_ARRAY,TY_VLA
      if (reg_passable) {
        reg_passable = 0;
        arg->pass_by_stack = 0;
      }else if (is_va_arg) {
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
  push_args2(node->args,node->lhs->ty->params,node->lhs->ty->is_variadic);

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
//       copy_struct2:
//         AccAB:size
//         IX:   dest
//         TOS:  src
  push();
  if (current_fn->stack_size<255) {
    ldx_bp();
    ldx_nX(current_fn->stack_size);
  }else{
    ldd_i(current_fn->stack_size);
    ldx_bp();
    adx();
    ldx_nX(0);
  }
  ldd_i(ty->size);
  println("\tjsr __copy_struct2 ; copy_struct_mem");
  depth -= 2; // copy_struct2 remove *TOS
  //ins(2); // copy_struct2 already pops argument.
  IX_invalidate();
  // reload dest addr
  if (current_fn->stack_size<255) {
    ldx_bp();
    println("\tldab %d+1,x",current_fn->stack_size);
    println("\tldaa %d,x",  current_fn->stack_size);
  }else{
    ldd_i(current_fn->stack_size);
    ldx_bp();
    adx();
    println("\tldab 1,x");
    println("\tldaa 0,x");
  }
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
    println("\tstx @tmp1	; save __alloca_bottom address");
  }
  println("\tsts @tmp2	; save old SP");	// sp -= alloca size
  println("\tldab @tmp2+1");
  println("\tldaa @tmp2");
  println("\tsubb @tmp4+1");	// alloca size
  println("\tsbca @tmp4");
  println("\tstab @tmp3+1");
  println("\tstaa @tmp3");
  println("\tlds @tmp3	; get new SP");
  char *loop = new_jump_label();
  char *skip = new_jump_label();
  // The program moved the stack pointer to implement alloca,
  //     so the original stack data must be copied.
  // Push the data from __alloca_bottom to @tmp2 onto the new SP
  println("\tldx 0,x");		// get old alloca bottom
  println("%s:",loop);
  println("\tdex");
  println("\tcpx @tmp2");
  println("\tbeq %s",skip);
  println("\tldab 0,x");
  println("\tpshb");
  println("\tbra %s",loop);
  println("%s:",skip);

  println("; Move alloca_bottom pointer.");
  println("\tldx @tmp1");
  IX_invalidate();
  println("\tldab 1,x	; make new __alloca_bottom__");
  println("\tldaa 0,x");
  println("\tsubb @tmp4+1	; alloca size");
  println("\tsbca @tmp4");
  println("\tstab 1,x");  // update alloca_bottom
  println("\tstaa 0,x");
  println(";");	          // return alloca(size);
}

//
// If node is a simple expression, it is computed directly without pushing it onto the stack.
//
static bool gen_direct_sub(Node *node,char *opb, char *opa, bool test, bool is_char)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));
  char *addr;

  if (!is_store && (addr=is_addr_constant(node))) {
    switch (node->ty->kind) {
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
    case TY_ARRAY:
      if (test) return true;
      println("\t%s #<%s", opb, addr);
      println("\t%s #>%s",   opa, addr);
      return 1;
    }
  }
  switch(node->kind){
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:		// TODO: Avoid unnecessary type promotion
      if (test) return 1;
      println("\t%s #<%u", opb, (uint16_t)node->val);
      return 1;
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
      } else if (strcmp(opb,"ldab")==0) {
        if (opa) ldd_i ((uint16_t)node->val);
        else     ldab_i((uint16_t)node->val);
      } else if (strcmp(opb,"andb")==0) {
        if (opa) and_i ((uint16_t)node->val);
        else     andb_i((uint16_t)node->val);
      } else if (strcmp(opb,"orab")==0) {
        if (opa) ora_i ((uint16_t)node->val);
        else     orab_i((uint16_t)node->val);
      } else if (strcmp(opb,"eorb")==0) {
        if (opa) eor_i ((uint16_t)node->val);
        else     eorb_i((uint16_t)node->val);
      } else {
        println("\t%s #<%u", opb, (uint16_t)node->val);
        if (opa) println("\t%s #>%u", opa, (uint16_t)node->val);
      }
      return 1;
    default:
      return 0;
    }
  } // ND_NUM
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return 0;
    }
    if(node->var->is_local){
      if (node->ty->kind==TY_ARRAY) {
        if (test) return true;
        println("\t%s @bp+1",opb);
        if (opa)
          println("\t%s @bp",opa);
        if (strcmp(opb,"addb")==0 && node->var->offset==0) {
          return 1;
        }
        println("\t%s #<%d",opb,node->var->offset);
        if (opa)
          println("\t%s #>%d",opa,node->var->offset);
        return 1;
      }
      if (!test_addr_x(node)) return 0;
      if (is_int8(node->ty)) {
        if (test) {
          return is_char || node->ty->is_unsigned;
        }
        int off = gen_addr_x(node);
        println("\t%s %d,x",opb,off);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
      }else{
        if (test) return 1;
        int off = gen_addr_x(node);
        println("\t%s %d,x",opb,off+1);
        if (opa)
          println("\t%s %d,x",opa,off);
      }
      return 1;
    }else{
      // global
      if (node->ty->kind==TY_FUNC)
        return 0;
//    if (node->ty->kind==TY_CHAR && !node->ty->is_unsigned && !opa)
//        return 0;
      if (test) return 1;
      if (is_int8(node->ty)) {
   	    if (is_store) {
          println("\t%s _%s",opb,node->var->name);
          invalidate_EXT(node);
          return 1;
        }
        println("\t%s _%s",opb,node->var->name);
        if (opa)
          println("\t%s #0",opa);
	      return 1;
	    }
      if (node->ty->kind==TY_ARRAY) {
        println("\t%s #<_%s",opb,node->var->name);
        if (opa)
          println("\t%s #>_%s",opa,node->var->name);
	      return 1;
      }
      println("\t%s _%s+1",opb,node->var->name);
      if (opa)
        println("\t%s _%s",opa,node->var->name);
      if (is_store) {
        invalidate_EXT(node);
      }
      return 1;
    }
    return 0;
  } // ND_VAR
  case ND_DEREF:
    switch(node->lhs->kind){
    // (ND_DEREF ty_char (ND_NUM TY_PTR e000))
    case ND_NUM:
      assert (node->lhs->ty->kind == TY_PTR);
      if (test) return 1;
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\t%s %ld",opb,node->lhs->val);
        return 1;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        println("\t%s %ld+1",opb,node->lhs->val);
        println("\t%s %ld",opa,node->lhs->val);
        return 1;
      } // ND_DEREF → ND_NUM
      break;
    // (ND_DEREF ty_int (ND_VAR TY_ARRAY(12) _L_1 global)
    case ND_VAR: {
      if (!is_integer(node->ty) || node->ty->kind==TY_LONG)
        return 0;
      if (is_global_array(node->lhs)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s _%s",opb,node->lhs->var->name);
          invalidate_EXT(node->lhs);
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s _%s+1",opb,node->lhs->var->name);
          println("\t%s _%s",  opa,node->lhs->var->name);
          invalidate_EXT(node->lhs);
          return 1;
        }
      }
    } // ND_DEREF → ND_VAR
      break;
    case ND_CAST: {
      if (!is_integer_or_ptr(node->ty) || node->ty->kind==TY_LONG)
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
          println("\t%s %ld",  opa,node->lhs->lhs->val);
          return 1;
        }
      }
    } // ND_DEREF → ND_CAST
      break;
    case ND_ADD: { // ND_DEREF → ND_ADD
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
            if (opa)
              println("\t%s _%s",opa,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->lhs->var->name,val);
            if (opa)
              println("\t%s _%s+%ld",opa,lhs->lhs->var->name,val);
          }
          return 1;
        }
      }
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
            if (opa)
              println("\t%s _%s",  opa,lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->var->name,val);
            if (opa)
              println("\t%s _%s+%ld",  opa,lhs->var->name,val);
          }
          return 1;
        }
      }
    } // ND_DEREF → ND_ADD
    break;
    } // ND_DEREF
    return 0;
  case ND_CAST:
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_sub(node->lhs, opb, opa, test,0))
      return 1;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_sub(node->lhs, opb, opa, test,0)) {
      return 1;
    }
    if (node->ty->kind      == TY_PTR
    &&  gen_direct_sub(node->lhs, opb, opa, test, 0))
      return 1;
    // (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) m +0 )
    if (node->ty->kind == TY_PTR
    &&  node->lhs->kind == ND_VAR
    &&  is_local_array(node->lhs)) {
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
    char *addr;
    if ((addr=is_addr_constant(node))!=NULL) {
      if (test) return 1;
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\t%s #%s",opb,addr);
        return 1;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        println("\t%s #<%s", opb,addr);
        println("\t%s #>%s", opa,addr);
        return 1;
      }
      return 0;
    }
    return 0;
  default:
    if (test_addr_x(node)) {
      if (node->kind == ND_MEMBER
      &&  !node->member->is_bitfield
      &&  is_global_var(node->lhs)) {
        char *name = node->lhs->var->name;
        int moff = node->member->offset;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) {
            return is_char || node->ty->is_unsigned;
          }
          println("\t%s _%s+%d",opb,name,moff);
          if (is_store) {
            invalidate_EXT(node->lhs);
          }else if (opa) {
            println("\t%s #0",opa);
          }
          return 1;
        case TY_INT:
        case TY_SHORT:
        case TY_ENUM:
          if (test) return 1;
          println("\t%s _%s+%d",opb,name,moff+1);
          if (opa)
            println("\t%s _%s+%d",opa,name,moff);
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return 1;
        }
      }
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        if (test) {
          return is_char || node->ty->is_unsigned;
        }
        int off = gen_addr_x(node);
        println("\t%s %d,x",opb,off);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
        return 1;
      case TY_INT:
      case TY_SHORT:
      case TY_ENUM:
        if (test) return 1;
        off = gen_addr_x(node);
        println("\t%s %d,x",opb,off+1);
        if (opa)
          println("\t%s %d,x",opa,off);
        return 1;
      default:
        return 0;
      }
    }
    return 0;
  }
  return 0;
}

bool can_direct(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,1,0);	// test mode

  return r;
}

bool can_direct_char(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,1,1);	// test mode

  return r;
}

bool gen_direct(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0,0);
}

bool gen_direct_char(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0,1);
}

//
// Commutative 16-bit arithmetic processing
//
int gen_direct_lr(Node *node, char *opb, char *opa)
{
    int can_direct_lhs = can_direct(node->lhs);
    int can_direct_rhs = can_direct(node->rhs);

#if 0
    if (can_direct_lhs && can_direct_rhs){
      if (node->rhs->kind == ND_CAST
      &&  is_int16(node->rhs->ty)
      &&  is_int8(node->rhs->lhs->ty)
      &&  node->rhs->lhs->ty->is_unsigned) {
        gen_expr(node->rhs);
        if (gen_direct(node->lhs,opb,opa))
          return 1;
        assert(0);
      }
    }
#endif

    if (can_direct_rhs){
      gen_expr(node->lhs);
      if (gen_direct(node->rhs,opb,opa))
        return 1;
      assert(0);
    }
    if (can_direct_lhs){
      gen_expr(node->rhs);
      if (gen_direct(node->lhs,opb,opa))
        return 1;
      assert(0);
    }
    return 0;
}

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
    // A VLA variable holds a pointer at var->offset, not the data.
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

//
// @long op= val
//
//  - val of 1 or -1 becomes inc32 or dec32. add and sub swap them.
//  - At -O2 and above it makes the code for each byte. Bytes are tied
//    by the carry, so go LSB to MSB. Low bytes of 0 are skipped and
//    the first byte emitted uses AccB.
//  - IX is not touched. jsr __op32i breaks it.
//
static void gen_direct_long_addsub_imm(Node *node, int64_t val)
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

//
// @long op= val
//
//  - -O2 does it one byte at a time. Bytes are free of each other, so
//    clr and com may touch the C flag.
//  - CLR and COM have no direct mode, so they are 3 bytes here.
//  - IX is not touched. jsr __op32i breaks it.
//
static void gen_direct_long_bitop_imm(Node *node, int64_t val)
{
  char *opb;
  char *fmt;      // does the whole byte
  uint8_t keep;   // leave this byte alone
  uint8_t whole;  // use fmt for this byte

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

//
// off,x op= val, one byte at a time.
// add and sub: bytes are tied by the carry, so go LSB to MSB.
// Low bytes of 0 are skipped. The first byte emitted uses AccB.
//
static void gen_opeq32_addsub(Node *node, int off, int64_t val)
{
  char *opb;
  char *opa;
  bool emitted = false;

  switch (node->kind) {
  case ND_ADDEQ: opb="addb"; opa="adca"; break;
  case ND_SUBEQ: opb="subb"; opa="sbca"; break;
  default: assert(0);
  }

  for (int nth = 3; nth >= 0; nth--) {
    uint8_t imm = (val >> ((3-nth)*8)) & 0xFF;

    if (!emitted && imm == 0) {
      continue;
    }
    if (!emitted) {
      println("\tldab %d,x", off+nth);
      println("\t%s #%u", opb, imm);
      println("\tstab %d,x", off+nth);
      emitted = true;
    } else {
      println("\tldaa %d,x", off+nth);
      println("\t%s #%u", opa, imm);
      println("\tstaa %d,x", off+nth);
    }
  }
}

// off,x op= val, one byte at a time.
// Bytes are free of each other, so clr and com may touch the C flag.
static void gen_opeq32_bitop(NodeKind kind, int off, int64_t val)
{
  char *opb;
  char *fmt;      // the instructions that do the whole byte
  uint8_t keep;   // this byte stays as it is
  uint8_t whole;  // this byte is done by fmt

  switch (kind) {
  case ND_ANDEQ: opb="andb"; keep=0xFF; whole=0x00; fmt="\tclr %d,x"; break;
  case ND_OREQ:  opb="orab"; keep=0x00; whole=0xFF; fmt="\tldab #$FF\n\tstab %d,x"; break;
  case ND_XOREQ: opb="eorb"; keep=0x00; whole=0xFF; fmt="\tcom %d,x"; break;
  default: assert(0);
  }

  for (int nth = 3; nth >= 0; nth--) {
    uint8_t imm = (val >> ((3-nth)*8)) & 0xFF;

    if (imm == keep)
      continue;
    if (imm == whole) {
      println(fmt, off+nth);
      continue;
    }
    println("\tldab %d,x", off+nth);
    println("\t%s #%u", opb, imm);
    println("\tstab %d,x", off+nth);
  }
}

//
// @long op= (off,IX)
//
// op is the stem. for example,
// when op == "add", op32x makes jsr __add32x, __add32bx, __add32dx
//
static void op32x(char *op, int off)
{
  if (off == 0) {
    println("\tjsr __%s32x", op);      // __op32x does not touch IX
    return;
  }
  if (off <= 255) {
    ldab_i(off);
    println("\tjsr __%s32bx", op);
  } else {
    ldd_i(off);
    println("\tjsr __%s32dx", op);
  }
  IX_invalidate();
}

//
// @long = (lhs op= rhs)
//
//  - sub uses rsub, so that @long = lhs - @long.
//
static void gen_opeq32(Node *node)
{
  char *op;

  switch (node->kind) {
  case ND_ADDEQ: op="add";  break;
  case ND_SUBEQ: op="rsub"; break;
  case ND_ANDEQ: op="and";  break;
  case ND_OREQ:  op="or";   break;
  case ND_XOREQ: op="xor";  break;
  default: assert(0);
  }

  if (test_addr_x(node->lhs)) {
    gen_expr(node->rhs);                       // @long = rhs
    op32x(op, gen_addr_x(node->lhs));    // @long op= lhs
    store32x(0);                               // lhs = @long
    return;
  }

  gen_addr(node->lhs);   // AB = &lhs
  push();                // save &lhs
  gen_expr(node->rhs);   // @long = rhs
  popx();                // IX = &lhs
  op32x(op, 0);          // @long op= [IX]
  store32x(0);           // [IX] = @long
}

//
// @long op= rhs, expanded byte by byte.
//
static void gen_direct_long(Node *node)
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

  // ldab -> op -> stab
  println("\tldab @long+3");
  if (R == 3) {
    println("\t%s %s+3", opb, raddr);
  } else {
    println("\t%s %d,x",  opb, roff+3);
  }
  println("\tstab @long+3");

  // ldaa -> op -> staa
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

static bool can_direct_long(Node *node)
{
  return long_location_type(skip_empty_cast(node->rhs)) != 0;
}

//
// @long = lhs op rhs
//
//  - 32-bit work is a stack machine whose top is @long. push and pop
//    of four bytes costs more than the op itself, so here we read
//    both sides where they are and only the result goes into @long.
//  - opb is for the lowest byte, opa for the upper three. add and sub
//    have to carry.
//
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

  // lhs,rhs: local or other var
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

// The MC6800 has only one IX, so 2 and 4 cannot be used at the same time.
// Two 2's are fine because they share @bp.
bool can_direct_long2(Node *node)
{
  Node *lhs = skip_empty_cast(node->lhs);
  Node *rhs = skip_empty_cast(node->rhs);
  int L = long_location_type(lhs);
  int R = long_location_type(rhs);

  if (!L || !R)             return false;
  if (L==1 || L==3)         return true;   // lhs does not need IX
  if (R==1 || R==3)         return true;   // rhs does not need IX
  if (L==2 && R==2)         return true;   // share @bp

  return false;
}

//
// @long op= rhs, through the stack
//
static void gen_long_tos(Node *node)
{
  char *op;

  switch (node->kind) {
  case ND_ADD:    op="add"; break;
  case ND_SUB:    op="sub"; break;
  case ND_BITAND: op="and"; break;
  case ND_BITOR:  op="or";  break;
  case ND_BITXOR: op="xor"; break;
  default: assert(0);
  }

  gen_expr(node->lhs);
  pushl();
  gen_expr(node->rhs);
  println("\tjsr __%s32tos", op);
  IX_invalidate();
  depth -= 4;
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
  if (opt_fbuiltin_memcpy && builtin_memcpy(node)) {
    return;
  }
  if (opt_fbuiltin_memset && builtin_memset(node)) {
    return;
  }
  if (opt_fbuiltin_strcpy && builtin_strcpy(node)) {
    return;
  }
  if (opt_fbuiltin_signbit && builtin_signbit(node)) {
    return;
  }
  if (opt_fbuiltin_isnan && builtin_isnan(node)) {
    return;
  }
  if (opt_fbuiltin_isinf && builtin_isinf(node)) {
    return;
  }
  if (opt_fbuiltin_isfinite && builtin_isfinite(node)) {
    return;
  }
  if (opt_fbuiltin_fabsf && builtin_fabsf(node)) {
    return;
  }
  if (opt_fbuiltin_copysignf && builtin_copysignf(node)) {
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
    // The arg must be the last named parameter.
    Obj *last = current_fn->params;
    while (last && last->next)
      last = last->next;
    if (node->args->kind != ND_VAR
    ||  !last
    ||  node->args->var != last) {
      error_tok(node->tok, "_builtin_va_start_addr: not the last named parameter");
    }
    // var is the first parameter and pass-by-register ?
    int passed_by_reg = 0;
    if (!strcmp(current_fn->params->name,node->args->var->name)) {
      passed_by_reg = 1;
      switch (current_fn->params->ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
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
  }else if (test_decayed_x(node->lhs)) {
    println("\tjsr %d,x",gen_decayed_x(node->lhs));
  }else if (test_expr_x(node->lhs)) {
    gen_expr_x(node->lhs);
    println("\tjsr 0,x");
  }else{
    if (node->args && !node->args->pass_by_stack) {
      switch (node->args->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        push1();
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_PTR:
      case TY_ENUM:
        push();
        break;
      case TY_LONG:
      case TY_FLOAT:
      case TY_DOUBLE:
      case TY_LDOUBLE:
        pushl();
        break;
      default:
        assert(0);
      }
    }else if (node->ret_buffer) {
      push();
    }
    gen_expr(node->lhs);
    println("\tstab @tmp1+1");
    println("\tstaa @tmp1");
    if (node->args && !node->args->pass_by_stack) {
      switch (node->args->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        pop1();
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_PTR:
      case TY_ENUM:
        pop();
        break;
      case TY_LONG:
      case TY_FLOAT:
      case TY_DOUBLE:
      case TY_LDOUBLE:
        popl();
        break;
      default:
        assert(0);
      }
    }else if (node->ret_buffer) {
      pop();
    }
    println("\tldx @tmp1");
    println("\tjsr 0,x");
  }
  IX_invalidate();
  
  // Removes pushed arguments before calling a function for speed
  remove_args(stack_args);

  return;
}

static void opeq_setup_operands(Node *node)
{
  gen_addr(node->lhs);
  push();
  gen_expr(node->rhs);
  int rhs_size = node->rhs->ty->size;
  switch (rhs_size) {
  case 1: push1(); break;
  case 2: push();  break;
  case 4: pushl(); break;
  default: assert(0);
  }
  println("\ttsx");
  println("\tldx %d,x",rhs_size);
  IX_invalidate();
  switch (node->ty->kind) {
  case TY_BOOL:
  case TY_CHAR:
    println("\tldab 0,x");
    break;
  case TY_SHORT:
  case TY_INT:
  case TY_ENUM:
    println("\tldab 1,x");
    println("\tldaa 0,x");
    break;
  case TY_LONG:
  case TY_FLOAT:
  case TY_DOUBLE:
    load32x(0);
    break;
  case TY_PTR:
  default: assert(0);
  }
}

static void opeq_cleanup_operands(Node *node)
{
  IX_invalidate();
  switch (node->rhs->ty->size) {
  case 1:
    ins(1);
    break;
  case 2:
    ins(2);
    break;
  case 4:
    depth -= 4;
    break;
  default: assert(0);
  }
  switch (node->kind) {
  case ND_SHLEQ:
  case ND_SHREQ:
    // the right side is the shift count, not the width of the operation
    break;
  default:
    cast(node->rhs->ty,node->ty);
  }
  IX_invalidate();
  store(node->ty);
}

static void opeq_float(Node *node)
{
  char *op;

  switch (node->kind) {
  case ND_ADDEQ: op="__addf32tos"; break;
  case ND_SUBEQ: op="__subf32tos"; break;
  case ND_MULEQ: op="__mulf32tos"; break;
  case ND_DIVEQ: op="__divf32tos"; break;
  default: assert(0);
  }

  if (node->kind==ND_ADDEQ || node->kind==ND_MULEQ) {
    gen_addr(node->lhs);
    push();
    println("\ttsx");
    println("\tldx 0,x");
    println("\tjsr __push32x");
    IX_invalidate();
    depth += 4;
    gen_expr(node->rhs);
  }else{
    // sub and div need the left side in @long and the right side on the stack
    opeq_setup_operands(node);
  }
  println("\tjsr %s",op);
  opeq_cleanup_operands(node);
}

static void opeq(Node *node)
{
  int64_t val;
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;

  if (is_flonum(node->ty)) {
    opeq_float(node);
    return;
  }

  switch(node->kind){
  case ND_ADDEQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      if (test_addr_x(lhs)) {
        int64_t v;

        if (node->retval_unused
        &&  is_long_constant(rhs,&v)) {
          if (v==1 || v==-1) {
            op32x(v==1 ? "inc" : "dec", gen_addr_x(lhs));
            return;
          }
          if (!opt('O','s')) {
            gen_opeq32_addsub(node, gen_addr_x(lhs), v);
            return;
          }
        }
      }
      gen_opeq32(node);
      return;
    // Handle non-char/int RHS case? XXX
    case TY_BOOL:
      switch (rhs->ty->kind) {
      case TY_LONG: {
        // walk b + rhs one byte at a time; leave as soon as a byte is not zero
        char *label = new_label("L_%d");
        gen_addr(lhs);
        push();
        gen_expr(rhs);
        popx();
        println("\tldab 0,x");
        println("\taddb @long+3");
        println("\tbne %s", label);
        println("\tadcb @long+2");
        println("\tbne %s", label);
        println("\tadcb @long+1");
        println("\tbne %s", label);
        println("\tadcb @long");
        println("%s:", label);
        cast(ty_char,ty_bool);
        println("\tstab 0,x");
        return;
      }
      case TY_BOOL:
      case TY_CHAR:
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        gen_addr(lhs);
        push();
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        popx();
        println("\taddb 0,x");
        println("\tadca #0");
        cast(ty_int,ty_bool);
        println("\tstab 0,x");
        return;
      }
      error_tok(node->tok, "opeq: bad rhs type for _Bool +=");
    case TY_CHAR:
      if (is_global_var(lhs)) {
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        println("\taddb _%s",lhs->var->name);
        println("\tstab _%s",lhs->var->name);
        invalidate_EXT(lhs);
        return;
      }
      if (test_addr_x(lhs)) {
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        int off = gen_addr_x(lhs);
        println("\taddb %d,x",off);
        println("\tstab %d,x",off);
        return;
      }
      gen_addr(lhs);
      push();
      gen_expr(rhs);
      cast(rhs->ty,ty_int);
      popx();
      println("\taddb 0,x");
      println("\tstab 0,x");
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (is_global_var(lhs)) {
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        println("\taddb _%s+1",lhs->var->name);
        println("\tadca _%s",lhs->var->name);
        println("\tstab _%s+1",lhs->var->name);
        println("\tstaa _%s",lhs->var->name);
        invalidate_EXT(lhs);
        return;
      }
      if (test_addr_x(lhs)) {
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        int off = gen_addr_x(lhs);
        println("\taddb %d,x",off+1);
        println("\tadca %d,x",off);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        return;
      }
      if (is_integer_constant(rhs,&val)) {
        gen_addr(lhs);
        tfr_dx();
        println("\tldab 1,x");
        println("\tldaa 0,x");
        println("\taddb #<%ld",val);
        println("\tadca #>%ld",val);
        println("\tstab 1,x");
        println("\tstaa 0,x");
        return;
      }
      gen_addr(lhs);
      push();
      gen_expr(rhs);
      cast(rhs->ty,ty_int);
      popx();
      println("\taddb 1,x");
      println("\tadca 0,x");
      println("\tstab 1,x");
      println("\tstaa 0,x");
      return;
    default:
      assert(0);
    }
    assert(0);
  }
  case ND_SUBEQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      if (test_addr_x(lhs)) {
        int64_t v;

        if (node->retval_unused
        && is_long_constant(rhs,&v)) {
          if (v==1 || v==-1) {
            op32x(v==1 ? "dec" : "inc", gen_addr_x(lhs));
            return;
          }
          if (!opt('O','s')) {
            gen_opeq32_addsub(node, gen_addr_x(lhs), v);
            return;
          }
        }
      }
      gen_opeq32(node);
      return;
    case TY_BOOL:
      switch (rhs->ty->kind) {
      case TY_LONG:
        // bool(b - rhs) == bool(rhs - b); a borrow leaves D != 0,
        // so the upper 16 bits can be ORed in as they are
        gen_addr(lhs);
        push();
        gen_expr(rhs);
        println("\tldab @long+3");
        println("\tldaa @long+2");
        popx();
        println("\tsubb 0,x");
        println("\tsbca #0");
        println("\torab @long+1");
        println("\toraa @long");
        cast(ty_int,ty_bool);
        println("\tstab 0,x");
        return;
      case TY_BOOL:
      case TY_CHAR:
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        gen_addr(lhs);
        push();
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        negd();
        popx();
        println("\taddb 0,x");
        println("\tadca #0");
        cast(ty_int,ty_bool);
        println("\tstab 0,x");
        return;
      }
      error_tok(node->tok, "opeq: bad rhs type for _Bool -=");
    case TY_CHAR: {
        if (test_addr_x(lhs)) {
          if (is_integer_constant(rhs,&val)) {
            int off = gen_addr_x(lhs);
            if (node->retval_unused) {
              switch(val) {
              case 1:   // -= 1;
                println("\tdec %d,x",off);
                return;
              case -1:  // -= -1;
                println("\tinc %d,x",off);
                return;
              case 2:   // -= 2;
                if (opt('O','s')) {
                  println("\tdec %d,x",off);
                  println("\tdec %d,x",off);
                  return;
                }
              }
            }
            println("\tldab %d,x",off);
            println("\tsubb #%ld",val);
            println("\tstab %d,x",off);
            return;
          }else if (is_global_var(rhs) && rhs->ty->kind==TY_CHAR) {
            int off = gen_addr_x(lhs);
            println("\tldab %d,x",off);
            println("\tsubb _%s",rhs->var->name);
            println("\tstab %d,x",off);
            return;
          }
          gen_expr(rhs);
          cast(rhs->ty,ty_int);
          println("\tnegb");
          int off = gen_addr_x(lhs);
          println("\taddb %d,x",off);
          println("\tstab %d,x",off);
          return;
        }
        gen_addr(lhs);
        push();
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        println("\tnegb");
        popx();
        println("\taddb 0,x");
        println("\tstab 0,x");
        return;
      } // TY_CHAR
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test_addr_x(lhs)) {
        if (node->ty->kind == TY_PTR
        &&  rhs->kind == ND_CAST
        &&  rhs->ty->kind == TY_PTR) {
          rhs = rhs->lhs;
        }
        if (is_integer_constant(rhs,&val)) {
          int off = gen_addr_x(lhs);
          println("\tldab %d,x",off+1);
          println("\tldaa %d,x",off);
          println("\tsubb #<%ld",val);
          println("\tsbca #>%ld",val);
          println("\tstab %d,x",off+1);
          println("\tstaa %d,x",off);
          IX_invalidate();
          return;
        }
        gen_expr(rhs);
        cast(rhs->ty,ty_int);
        negd();
        int off = gen_addr_x(lhs);
        println("\taddb %d,x",off+1);
        println("\tadca %d,x",off);
        println("\tstab %d,x",off+1);
        println("\tstaa %d,x",off);
        IX_invalidate();
        return;
      }
      if (is_integer_constant(rhs,&val)) {
        gen_addr(lhs);
        tfr_dx();
        println("\tldab 1,x");
        println("\tldaa 0,x");
        println("\tsubb #<%ld",val);
        println("\tsbca #>%ld",val);
        println("\tstab 1,x");
        println("\tstaa 0,x");
        return;
      }
      gen_addr(lhs);
      push();
      gen_expr(rhs);
      cast(rhs->ty,ty_int);
      negd();
      popx();
      println("\taddb 1,x");
      println("\tadca 0,x");
      println("\tstab 1,x");
      println("\tstaa 0,x");
      return;
    default:
      assert(0);
    }
    assert(0);
  }
  case ND_MULEQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      gen_addr(lhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tjsr __push32x");
      IX_invalidate();
      depth+=4;
      gen_expr(node->rhs);
      println("\tjsr __mul32tos");
      IX_invalidate();
      depth-=4;
      store(node->ty);
      return;
    case TY_BOOL:
    case TY_CHAR: 
      gen_addr(lhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 0,x");
      cast(lhs->ty,ty_int);
      IX_invalidate();
      push();
      gen_expr(rhs);
      cast(rhs->ty,ty_int);
      println("\tjsr __mul16x16");
      ins(2);
      IX_invalidate();
      if (node->ty->kind==TY_BOOL) {
        cast(ty_int,ty_bool);
      }
      store(node->lhs->ty);
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      gen_addr(lhs);
      push();
      println("\ttsx");
      println("\tldx 0,x");
      println("\tldab 1,x");
      println("\tldaa 0,x");
      IX_invalidate();
      push();
      gen_expr(node->rhs);
      cast(node->rhs->ty,ty_int);
      println("\tjsr __mul16x16");
      IX_invalidate();
      ins(2);
      store(node->lhs->ty);
      return;
    default:
      assert(0);
    }
    assert(0);
  }
  case ND_DIVEQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      opeq_setup_operands(node);
      if (rhs->ty->is_unsigned) {
        println("\tjsr __div32x32u");
      }else{
        println("\tjsr __div32x32s");
      }
      opeq_cleanup_operands(node);
      break;
    case TY_BOOL:
    case TY_CHAR: 
      switch (rhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        opeq_setup_operands(node);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __div8x8u");
        }else{
          println("\tjsr __div8x8s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        opeq_setup_operands(node);
        cast(node->ty,ty_int);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __div16x16u");
        }else{
          println("\tjsr __div16x16s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_LONG:
        opeq_setup_operands(node);
        cast(node->ty,ty_long);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __div32x32u");
        }else{
          println("\tjsr __div32x32s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_PTR:
      default:assert(0);
      }
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      if (node->lhs->ty->is_unsigned) {
        if (is_integer_constant(node->rhs, &val)){
          switch(val) {
          case 8:
          case 4:
          case 2:
            if (is_global_var(node->lhs)) {
              int n = exact_log2(val);
              for (int i=0; i<n; i++) {
                println("\tlsr _%s",  node->lhs->var->name);
                println("\tror _%s+1",node->lhs->var->name);
              }
              invalidate_EXT(node->lhs);
            }else{
              int off = gen_addr_x(node->lhs);
              int n = exact_log2(val);
              for (int i=0; i<n; i++) {
                println("\tlsr %d,x",off);
                println("\tror %d,x",off+1);
              }
              IX_invalidate();
            }
            return;
          }
        }
      }else if (is_integer_constant(node->rhs, &val)){
        switch(val){
        case 2:
          if (is_global_var(node->lhs)) {
            println("\tldab _%s+1", node->lhs->var->name);
            println("\tldaa _%s",   node->lhs->var->name);
            println("\tasra");
            println("\trola");
            println("\tadcb #0");
            println("\tadca #0");
            println("\tasra");
            println("\trorb");
            println("\tstab _%s+1",node->lhs->var->name);
            println("\tstaa _%s",  node->lhs->var->name);
            invalidate_EXT(node->lhs);
            return;
          }else if (test_addr_x(node->lhs)) {
            int off = gen_addr_x(node->lhs);
            println("\tldab %d,x",off+1);
            println("\tldaa %d,x",off);
            println("\tasra");
            println("\trola");
            println("\tadcb #0");
            println("\tadca #0");
            println("\tasra");
            println("\trorb");
            println("\tstab %d,x",off+1);
            println("\tstaa %d,x",off);
            IX_invalidate();
            return;
          }
        }
      }
      switch (rhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        opeq_setup_operands(node);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __div16x16u");
        }else{
          println("\tjsr __div16x16s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_LONG:
        opeq_setup_operands(node);
        cast(node->ty,ty_long);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __div32x32u");
        }else{
          println("\tjsr __div32x32s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_PTR:
      default:assert(0);
      }
      break;
    case TY_PTR:
    default:
      assert(0);
    }
    return;
  }
  case ND_MODEQ: {
    switch(node->ty->kind) {
    case TY_LONG:
      opeq_setup_operands(node);
      if (rhs->ty->is_unsigned) {
        println("\tjsr __rem32x32u");
      }else{
        println("\tjsr __rem32x32s");
      }
      opeq_cleanup_operands(node);
      break;
    case TY_BOOL:
    case TY_CHAR: 
      switch (rhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        opeq_setup_operands(node);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __mod8x8u");
        }else{
          println("\tjsr __mod8x8s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        opeq_setup_operands(node);
        cast(node->ty,ty_int);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __rem16x16u");
        }else{
          println("\tjsr __rem16x16s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_LONG:
        opeq_setup_operands(node);
        cast(node->ty,ty_long);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __rem32x32u");
        }else{
          println("\tjsr __rem32x32s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_PTR:
      default:assert(0);
      }
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      switch (rhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
        opeq_setup_operands(node);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __rem16x16u");
        }else{
          println("\tjsr __rem16x16s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_LONG:
        opeq_setup_operands(node);
        cast(node->ty,ty_long);
        if (rhs->ty->is_unsigned) {
          println("\tjsr __rem32x32u");
        }else{
          println("\tjsr __rem32x32s");
        }
        opeq_cleanup_operands(node);
        break;
      case TY_PTR:
      default:assert(0);
      }
      break;
    case TY_PTR:
    default:
      assert(0);
    }
    return;
  }
  case ND_ANDEQ:
  case ND_OREQ:
  case ND_XOREQ:
    switch(node->ty->kind) {
    case TY_LONG: // long op= rhs
      if (test_addr_x(lhs)) {
        int64_t v;
        if (!opt('O','s')
        &&  node->retval_unused
        &&  is_long_constant(rhs,&v)) {
          gen_opeq32_bitop(node->kind, gen_addr_x(lhs), v);
          return;
        }
      }
      gen_opeq32(node); // @long = lhs opeq rhs
      return;
    case TY_BOOL:
      switch (node->kind) {
      case ND_ANDEQ:  // bool &= rhs
        if (test_addr_x(node->lhs)) {
          gen_expr(node->rhs);
          cast(node->rhs->ty,ty_uchar);
          int off = gen_addr_x(node->lhs);
          println("\tandb %d,x",off);
          println("\tstab %d,x",off);
          return;
        }
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        cast(node->rhs->ty,ty_int);
        popx();
        println("\tandb 0,x");
        println("\tstab 0,x");
        return;
      case ND_OREQ: // bool |= rhs
        if (test_addr_x(node->lhs)) {
          gen_expr(node->rhs);
          cast(node->rhs->ty,ty_bool);
          int off = gen_addr_x(node->lhs);
          println("\torab %d,x",off);
          println("\tstab %d,x",off);
          return;
        }
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        cast(node->rhs->ty,ty_bool);
        popx();
        println("\torab 0,x");
        println("\tstab 0,x");
        return;
      case ND_XOREQ:  // bool ^= rhs
        if (test_addr_x(node->lhs)) {
          gen_expr(node->rhs);
          int off = gen_addr_x(node->lhs);
          switch(node->rhs->ty->kind) {
          case TY_BOOL:
          case TY_CHAR:
          case TY_SHORT:
          case TY_INT:
          case TY_ENUM:
            println("\teorb %d,x",off);
            break;
          case TY_LONG:
            println("\tldab @long+3");
            println("\teorb %d,x",off);
            println("\tstab @long+3");
            break;
          default:
            assert(0);
          }
          cast(node->rhs->ty,ty_bool);
          println("\tstab %d,x",off);
          return;
        }
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        popx();
        switch(node->rhs->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
          println("\teorb 0,x");
          break;
        case TY_LONG:
          println("\tldab @long+3");
          println("\teorb 0,x");
          println("\tstab @long+3");
          break;
        default:
          assert(0);
        }
        cast(node->rhs->ty,ty_bool);
        println("\tstab 0,x");
        return;
      } // bool ^= rhs
    case TY_CHAR: 
      // is_simple_var() sends a global var lhs to x = x op y, so it never gets here
      if (node->lhs->ty->is_unsigned && test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        cast(node->rhs->ty,ty_int);
        int off = gen_addr_x(node->lhs);
        switch(node->kind) {
        case ND_ANDEQ:
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
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      cast(node->rhs->ty,ty_int);
      println("\ttsx");
      println("\tldx 0,x");
      IX_invalidate();
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
      println("\tstab 0,x");
      ins(2);
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      if (test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        cast(node->rhs->ty,ty_int);
        int off = gen_addr_x(node->lhs);
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
        IX_invalidate();
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      cast(node->rhs->ty,ty_int);
      println("\ttsx");
      println("\tldx 0,x");
      IX_invalidate();
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
      println("\tstab 1,x");
      println("\tstaa 0,x");
      ins(2);
      return;
    default:
      error_tok(node->tok,"invalid operand &,|,^");
      assert(0);
    }
    assert(0);
  case ND_SHREQ:
  case ND_SHLEQ: {
    int64_t val;

    switch(node->lhs->ty->kind) {
    case TY_LONG:
      if (is_global_var(node->lhs)) {
        gen_expr(node->rhs);
        push1();
        ldx_IMM_VAR(node->lhs->var->name);
        load32x(0);
        pop1();
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl32");
        }else if (node->lhs->ty->is_unsigned) {
          println("\tjsr __shr32u");
        }else{
          println("\tjsr __shr32s");
        }
        ldx_IMM_VAR(node->lhs->var->name);
        store_x(node->ty,0);
        return;
      }
      opeq_setup_operands(node);
      pop1();
      if (node->kind == ND_SHLEQ) {
        println("\tjsr __shl32");
      }else if (node->lhs->ty->is_unsigned) {
        println("\tjsr __shr32u");
      }else{
        println("\tjsr __shr32s");
      }
      store(node->ty);
      return;
    case TY_BOOL:
    case TY_CHAR: 
      if (is_global_var(lhs)) {
        invalidate_EXT(lhs);
        if (is_integer_constant(rhs, &val)){
          if (node->retval_unused && val<=2) {
            if (node->kind == ND_SHLEQ) {
              for (int i=0; i<val; i++) {
                println("\tasl _%s",lhs->var->name);
              }
              return;
            }else if (lhs->ty->is_unsigned) {  // ND_SHREQ && unsigned
              for (int i=0; i<val; i++) {
                println("\tlsr _%s",lhs->var->name);
              }
              return;
            }else{                                   // ND_SHREQ && signed
              for (int i=0; i<val; i++) {
                println("\tasr _%s",lhs->var->name);
              }
              return;
            }
          }
          println("\tldab _%s",lhs->var->name);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            gen_shl(lhs->ty,val);
          }else{
            gen_shr(lhs->ty,val);
          }
          println("\tstab _%s",lhs->var->name);
          return;
        }
      }else if (test_addr_x(lhs)) {
        if (is_integer_constant(rhs, &val)){
          int off = gen_addr_x(lhs);
          if (node->retval_unused && val<=2) {
            if (node->kind == ND_SHLEQ) {
              for (int i=0; i<val; i++) {
                println("\tasl %d,x",off);
              }
              return;
            }else if (lhs->ty->is_unsigned) {  // ND_SHREQ && unsigned
              for (int i=0; i<val; i++) {
                println("\tlsr %d,x",off);
              }
              return;
            }else{                                   // ND_SHREQ && signed
              for (int i=0; i<val; i++) {
                println("\tasr %d,x",off);
              }
              return;
            }
          }
          println("\tldab %d,x",off);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            gen_shl(lhs->ty,val);
          }else{
            gen_shr(lhs->ty,val);
          }
          println("\tstab %d,x",off);
          return;
        }
        gen_expr(rhs);
        push1();
        int off = gen_addr_x(lhs);
        println("\tldab %d,x",off);
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl8");
        }else if (lhs->ty->is_unsigned) {
          println("\tjsr __shr8u");
        }else{
          println("\tjsr __shr8s");
        }
        println("\tstab %d,x",off);
        IX_invalidate();
        ins(1);
        return;
      } // TY_BOOL, TY_CHAR
      opeq_setup_operands(node);
      if (node->kind == ND_SHLEQ) {
        println("\tclra");
        println("\tjsr __shl16");
      }else{
        cast(lhs->ty,ty_int);
        if (lhs->ty->is_unsigned) {
          println("\tjsr __shr16u");
        }else{
          println("\tjsr __shr16s");
        }
      }
      println("\tstab 0,x");
      IX_invalidate();
      ins(3);
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      if (is_global_var(lhs)) {
        if (is_integer_constant(rhs, &val)) {
          invalidate_EXT(lhs);
          println("\tldab _%s+1",lhs->var->name);
          println("\tldaa _%s",lhs->var->name);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            gen_shl(lhs->ty,val);
          }else{
            gen_shr(lhs->ty,val);
          }
          println("\tstab _%s+1",lhs->var->name);
          println("\tstaa _%s",lhs->var->name);
          return;
        }
        gen_expr(rhs);
        push1();
        invalidate_EXT(lhs);
        println("\tldab _%s+1",lhs->var->name);
        println("\tldaa _%s",lhs->var->name);
        if (node->kind == ND_SHLEQ) {
          println("\tjsr __shl16");
        }else if (lhs->ty->is_unsigned) {
          println("\tjsr __shr16u");
        }else{
          println("\tjsr __shr16s");
        }
        println("\tstab _%s+1",lhs->var->name);
        println("\tstaa _%s",lhs->var->name);
        ins(1);
        return;
      }
      if (test_addr_x(node->lhs)) {
        if (is_integer_constant(node->rhs, &val)){
          int off = gen_addr_x(node->lhs);
          println("\tldab %d,x",off+1);
          println("\tldaa %d,x",off);
          if (val==0) {
            return;
          }
          if (node->kind == ND_SHLEQ) {
            gen_shl(node->lhs->ty,val);
          }else{
            gen_shr(node->lhs->ty,val);
          }
          println("\tstab %d,x",off+1);
          println("\tstaa %d,x",off);
          return;
        }
        gen_expr(node->rhs);
        push1();
        int off = gen_addr_x(node->lhs);
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
        IX_invalidate();
        ins(1);
        return;
      }
      opeq_setup_operands(node);
      if (node->kind == ND_SHLEQ) {
        println("\tjsr __shl16");
      }else if (node->lhs->ty->is_unsigned) {
        println("\tjsr __shr16u");
      }else{
        println("\tjsr __shr16s");
      }
      opeq_cleanup_operands(node);
      return;
    default:
      assert(0);
    }
    assert(0);
    return;
  }
  default:
    assert(0);
  }
}

// Generate code for a given node.
void gen_expr(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  int off;
  int64_t val;
  char *addr;

  switch (node->kind) {
  case ND_ADD: case ND_SUB: case ND_MUL: case ND_DIV:
  case ND_EQ:  case ND_NE:  case ND_LT:  case ND_LE:
  case ND_GT:  case ND_GE:  case ND_NEG:
    if (is_flonum(node->lhs->ty)) {
      gen_expr_float(node);
      return;
    }
    break;
  }

  switch (node->kind) {
  case ND_NULL_EXPR:
    return;
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE: {
      union { float f32; uint32_t u32; } u = { node->fval };
      println("; load float %e, %08x",u.f32,u.u32);
      load32i(u.u32);
      return;
    }
    case TY_BOOL:
    case TY_CHAR:
      if(node->val==0){
        println("\tclrb");
      }else{
        ldab_i((uint16_t)node->val);
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
    case TY_VOID:
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

    if (is_global_var(node->lhs)) {
      char *var = node->lhs->var->name;
      switch (node->lhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        switch(val){
        case 1:
          if (!node->retval_unused)
            println("\tldab _%s",var);
          println("\tinc _%s",var);
          break;
        case -1:
          if (!node->retval_unused)
            println("\tldab _%s",var);
          println("\tdec _%s",var);
          break;
        default:
          println("\tldab _%s",var);
          println("\taddb #%d",val);
          println("\tstab _%s",var);
          if (!node->retval_unused)
            println("\tsubb #%d",val);
          break;
        }
        invalidate_EXT(node->lhs);
        break;
        // TY_BOOL, TY_CHAR
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        if (node->retval_unused && val==1) {
          if (opt('O','s')) {
            ldx_EXT(node->lhs);
            println("\tinx");
            stx_EXT(node->lhs);
          }else{
            char *label = new_jump_label();
            println("\tinc _%s+1",var);
            println("\tbne %s",label);
            println("\tinc _%s",var);
            println("%s:",label);
            invalidate_EXT(node->lhs);
          }
        }else if (node->retval_unused && val==2) {
          ldx_EXT(node->lhs);
          println("\tinx");
          println("\tinx");
          stx_EXT(node->lhs);
        }else if (val>0) {
          println("\tldab _%s+1",var);
          println("\tldaa _%s",var);
          println("\taddb #<%d",val);
          println("\tadca #>%d",val);
          println("\tstab _%s+1",var);
          println("\tstaa _%s",var);
          if (!node->retval_unused) {
            println("\tsubb #<%d",val);
            println("\tsbca #>%d",val);
          }
          invalidate_EXT(node->lhs);
        } else if (node->retval_unused && val==-1) {
          ldx_EXT(node->lhs);
          println("\tdex");
          stx_EXT(node->lhs);
        } else if (node->retval_unused && val==-2) {
          ldx_EXT(node->lhs);
          println("\tdex");
          println("\tdex");
          stx_EXT(node->lhs);
        } else { // val<0
          val = abs(val);
          println("\tldab _%s+1",var);
          println("\tldaa _%s",var);
          println("\tsubb #<%d",val);
          println("\tsbca #>%d",val);
          println("\tstab _%s+1",var);
          println("\tstaa _%s",var);
          if (!node->retval_unused) {
            println("\taddb #<%d",val);
            println("\tadca #>%d",val);
          }
          invalidate_EXT(node->lhs);
        }
        break;
      case TY_LONG:
        ldx_IMM_VAR(var);
        if (node->retval_unused) {
          invalidate_EXT(node->lhs);
          if (val==1) {
            println("\tjsr __inc32x");
            return;
          }else if (val==-1) {
            println("\tjsr __dec32x");
            return;
          }
        }
        load32x(0);
        if (val==1) {
          println("\tjsr __inc32");
        }else if (val==-1) {
          println("\tjsr __dec32");
        }else{
          println("\tjsr __add32i");
          word32i(val);
          IX_invalidate();
        }
        ldx_IMM_VAR(var);
        store32x(0);
        invalidate_EXT(node->lhs);
        if (!node->retval_unused) {
          println("\tjsr __sub32i");
          word32i(val);
          IX_invalidate();
        }
        break;
      default:
        assert(0);
      }
      return;
    } // ND_POST_INCDEC → is_global_var
    if (node->lhs->ty->kind == TY_LONG) {
      gen_addr(node->lhs);
      push();
      tfr_dx();
      load32x(0);
      println("\tjsr __add32i");
      word32i(val);
      popx();
      store32x(0);
      if (!node->retval_unused) {
        println("\tjsr __sub32i");
        word32i(val);
      }
      return;
    }
    if (test_addr_x(node->lhs)){
      off = gen_addr_x(node->lhs);
    }else{
      off = 0;
      gen_addr(node->lhs);
      tfr_dx();
    }
    switch (node->lhs->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
      switch(val){
        case 1:
          if (!node->retval_unused)
            println("\tldab %d,x",off);
          println("\tinc %d,x",off);
          break;
        case -1:
          if (!node->retval_unused)
            println("\tldab %d,x",off);
          println("\tdec %d,x",off);
          break;
        default:
          println("\tldab %d,x",off);
          println("\taddb #%d",val);
          println("\tstab %d,x",off);
          if (!node->retval_unused)
            println("\tsubb #%d",val);
          break;
      }
      break;
      // TY_BOOL, TY_CHAR
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (node->retval_unused && val==1) {
        char *label = new_jump_label();
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
      }else if (node->retval_unused && val==-1) {
        char *label = new_jump_label();
        println("\ttst %d,x",off+1);  // 7 2
        println("\tbne %s",label);    // 4 2
        println("\tdec %d,x",off);    // 7 2
        println("%s:",label);
        println("\tdec %d,x",off+1);  // 7 2    // total: 25 8
      } else { // val<0
        val = abs(val);
        println("\tldab %d,x",off+1); // 5 2
        println("\tldaa %d,x",off);   // 5 2
        println("\tsubb #<%d",val);   // 2 2
        println("\tsbca #>%d",val);   // 2 2
        println("\tstab %d,x",off+1); // 6 2
        println("\tstaa %d,x",off);   // 6 2    // total 26 12
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

    if (is_global_var(node->lhs)) {
      char *var = node->lhs->var->name;
      switch (node->lhs->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        switch(val){
        case 1:
          println("\tinc _%s",var);
          if (!node->retval_unused)
            println("\tldab _%s",var);
          break;
        case -1:
          println("\tdec _%s",var);
          if (!node->retval_unused)
            println("\tldab _%s",var);
          break;
        default:
          println("\tldab _%s",var);
          println("\taddb #%d",val);
          println("\tstab _%s",var);
          break;
        }
        invalidate_EXT(node->lhs);
        break;
        // TY_BOOL, TY_CHAR
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        if (node->retval_unused && val==1) {
          ldx_EXT(node->lhs);
          println("\tinx");
          stx_EXT(node->lhs);
        }else if (node->retval_unused && val==2) {
          ldx_EXT(node->lhs);
          println("\tinx");
          println("\tinx");
          stx_EXT(node->lhs);
        }else if (val>0) {
          println("\tldab _%s+1",var);
          println("\tldaa _%s",var);
          println("\taddb #<%d",val);
          println("\tadca #>%d",val);
          println("\tstab _%s+1",var);
          println("\tstaa _%s",var);
          invalidate_EXT(node->lhs);
        } else if (node->retval_unused && val==-1) {
          ldx_EXT(node->lhs);
          println("\tdex");
          stx_EXT(node->lhs);
        } else if (node->retval_unused && val==-2) {
          ldx_EXT(node->lhs);
          println("\tdex");
          println("\tdex");
          stx_EXT(node->lhs);
        } else { // val<0
          val = abs(val);
          println("\tldab _%s+1",var);
          println("\tldaa _%s",var);
          println("\tsubb #<%d",val);
          println("\tsbca #>%d",val);
          println("\tstab _%s+1",var);
          println("\tstaa _%s",var);
          invalidate_EXT(node->lhs);
        }
        break;
       case TY_LONG:
         ldx_IMM_VAR(var);
         load32x(0);
         println("\tjsr __add32i");
         word32i(val);
         IX_invalidate();
         ldx_IMM_VAR(var);
         store32x(0);
         invalidate_EXT(node->lhs);
         break;
      default:
        assert(0);
      }
      return;
    }
    if (node->lhs->ty->kind == TY_LONG) {
      gen_addr(node->lhs);
      push();
      tfr_dx();
      load32x(0);
      println("\tjsr __add32i");
      word32i(val);
      popx();
      store32x(0);
      return;
    }
    if (test_addr_x(node->lhs)){
      off = gen_addr_x(node->lhs);
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
          println("\tinc %d,x",off);
          println("\tldab %d,x",off);
          break;
        case -1:
          println("\tdec %d,x",off);
          println("\tldab %d,x",off);
          break;
        default:
          println("\tldab %d,x",off);
          println("\taddb #%d",val);
          println("\tstab %d,x",off);
          break;
        }
      }
      break;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (node->retval_unused && val==1) {
        char *label = new_jump_label();
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
    if (is_int16(node->ty) && can_direct(node->lhs)) {
      println("\tclrb");
      println("\tclra");
      gen_direct(node->lhs,"subb","sbca");
      return;
    }
    gen_expr(node->lhs);

    switch (node->ty->kind) {
    case TY_LONG:
      println("\tjsr __neg32");
      IX_invalidate();
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
    Member *mem = node->member;
    if (mem->is_bitfield) {
      load_bitfield(node);
      return;
    }
    if (is_global_var(node->lhs)
    &&  is_integer_or_ptr(node->ty)) {
      char *name = node->lhs->var->name;
      int moff = mem->offset;
      if (node->ty->size == 1) {
        println("\tldab _%s+%d",name,moff);
        return;
      }
      if (node->ty->size == 2) {
        println("\tldab _%s+%d",name,moff+1);
        println("\tldaa _%s+%d",name,moff);
        return;
      }
    }
    if (opt('O','2')
    &&  node->ty->size == 4
    &&  (addr = is_var_addr_constant(node))) {
      println("\tldx %s+2",addr);
      println("\tstx @long+2");
      println("\tldx %s",  addr);
      println("\tstx @long");
      IX_invalidate();
      return;
    }
    if (node->ty->size == 4
    &&  (addr = is_var_addr_constant(node))) {
      ldx_IMM_STR(addr);
      println("\tjsr __load32x");
      return;
    }
    if (can_load_x(node->ty) && test_addr_x(node)) {
      off = gen_addr_x(node);
      load_x(node->ty,off);
    }else{
      gen_addr(node);
      load(node->ty);
    }
    return;
  }
  case ND_DEREF: {
    Node *lhs = node->lhs;
    if (opt('O','3')) {
      if (node->ty->kind == TY_CHAR
      &&  lhs->kind == ND_ADD
      &&  is_global_array(lhs->rhs)
      &&  lhs->rhs->ty->array_len<=256
      &&  !is_integer_constant(lhs->lhs,&val)) {
        char *offset = new_label("L_%d");
        gen_expr(optimize_expr(new_cast(skip_integral_promotion(lhs->lhs),ty_uchar)));
        ldx_IMM_VAR(lhs->rhs->var->name);
        println("\tstab %s+1    ; XXX !",offset);
        println("%s:",offset);
        println("\tldab 0,x");
        return;
      }
      if (node->ty->kind == TY_CHAR
      &&  lhs->kind == ND_ADD
      &&  is_global_array(lhs->lhs)
      &&  lhs->lhs->ty->array_len<=256
      &&  !is_integer_constant(lhs->rhs,&val)) {
        char *offset = new_label("L_%d");
        gen_expr(optimize_expr(new_cast(skip_integral_promotion(lhs->rhs),ty_uchar)));
        ldx_IMM_VAR(lhs->lhs->var->name);
        println("\tstab %s+1    ; XXX !",offset);
        println("%s:",offset);
        println("\tldab 0,x");
        return;
      }
    }
    // (ND_DEREF ty_uchar (ND_POST_INCDEC (ND_VAR TY_PTR(10) src +8 ) 1)))
    if (node->lhs->kind == ND_POST_INCDEC
    &&  (is_int8(node->ty) || is_int16_or_ptr(node->ty))
    &&  test_addr_x(node->lhs->lhs)
    &&  node->lhs->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->rhs,&val)) {
      if (is_global_var(node->lhs->lhs) && abs(val)<=2) {
        ldx_EXT(node->lhs->lhs);
        if (is_int8(node->ty)) {
          println("\tldab 0,x");
        }else if (is_int16_or_ptr(node->ty)) {
          println("\tldab 1,x");
          println("\tldaa 0,x");
        }else{
          assert(0);  // what?
        }
        switch(val){
        case -2: println("\tdex");
        case -1: println("\tdex");
                 break;
        case  2: println("\tinx");
        case  1: println("\tinx");
                 break;
        default: assert(0);
        }
        stx_EXT(node->lhs->lhs);
        return;
      }else if (val==1){
        int off = gen_addr_x(node->lhs->lhs);
        char *label = new_jump_label();
        println("\tinc %d,x",off+1);
        println("\tbne %s",label);
        println("\tinc %d,x",off);
        println("%s:",label);
        println("\tldx %d,x",off);
        println("\tdex");
        if (is_int8(node->ty)) {
          println("\tldab 0,x");
        }else if (is_int16_or_ptr(node->ty)) {
          println("\tldab 1,x");
          println("\tldaa 0,x");
        }else{
          assert(0);  // what?
        }
        IX_invalidate();
        return;
      }
    }
    // (ND_DEREF ty_uchar (ND_PRE_INCDEC (ND_VAR TY_PTR(10) src +8 ) 1)))
    if (node->lhs->kind == ND_PRE_INCDEC
    &&  (is_int8(node->ty) || is_int16_or_ptr(node->ty))
    &&  test_addr_x(node->lhs->lhs)
    &&  node->lhs->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->rhs,&val)) {
      if (is_global_var(node->lhs->lhs) && abs(val)<=2) {
        ldx_EXT(node->lhs->lhs);
        switch(val){
        case -2: println("\tdex");
        case -1: println("\tdex");
                 break;
        case  2: println("\tinx");
        case  1: println("\tinx");
                 break;
        default: assert(0);
        }
        stx_EXT(node->lhs->lhs);
        if (is_int8(node->ty)) {
          println("\tldab 0,x");
        }else if (is_int16_or_ptr(node->ty)) {
          println("\tldab 1,x");
          println("\tldaa 0,x");
        }else{
          assert(0);  // what?
        }
        return;
      }
    }
    if (can_direct(node)) {
      gen_direct(node,"ldab","ldaa");
      return;
    }
    if (opt('O','2')
    &&  node->ty->size == 4
    &&  (addr = is_var_addr_constant(node))) {
      println("\tldx %s+2",addr);
      println("\tstx @long+2");
      println("\tldx %s",  addr);
      println("\tstx @long");
      IX_invalidate();
      return;
    }
    if (node->ty->size == 4
    &&  (addr = is_var_addr_constant(node))) {
      ldx_IMM_STR(addr);
      println("\tjsr __load32x");
      return;
    }
    if (can_load_x(node->ty) && test_decayed_x(lhs)){
      load_x(node->ty,gen_decayed_x(lhs));
      return;
    }
    if (can_load_x(node->ty) && test_expr_x(lhs)){
      gen_expr_x(lhs);
      load_x(node->ty,0);
      return;
    }
    if (can_load_x(node->ty) && test_addr_x(node)) {
      load_x(node->ty,gen_addr_x(node));
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

    Node *lhs = node->lhs;
    Node *rhs = node->rhs;

    if (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION) {
      if (test_addr_x(node->lhs)) {
        gen_addr(node->rhs);
        if (!node->retval_unused) {
          push();
        }
        int off = gen_addr_x(node->lhs);
        store_x(node->ty, off);
        if (!node->retval_unused) {
          pop();
        }
        return;
      }
      if (node->retval_unused) {
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        store(node->ty);
        return;
      }
      gen_expr(node->rhs);
      push();
      gen_addr(node->lhs);
      push();
      println("\ttsx");
      println("\tldab 3,x");
      println("\tldaa 2,x");
      store(node->ty);
      pop();
      return;

    }
    // bit-field
    if (node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield) {
      assign_to_bitfield(node);
      return;
    } // ND_MEMBER, bit-field

    if (node->ty->kind == TY_LONG
    ||  node->ty->kind == TY_FLOAT
    ||  node->ty->kind == TY_DOUBLE
    ||  node->ty->kind == TY_LDOUBLE) {
      if (test_addr_x(node->lhs)) {
        gen_expr(node->rhs);
        off = gen_addr_x(node->lhs);
        store32x(off);
        if (is_global_var(node->lhs)) {
          invalidate_EXT(node->lhs);
        }
        return;
      }
      gen_addr(node->lhs);
      push();
      gen_expr(node->rhs);
      store(node->ty);
      return;
    }

    if (node->retval_unused
    && is_int16_or_ptr(lhs->ty)
    && is_int16_or_ptr(rhs->ty)) {
      if (is_global_var(node->lhs)
      &&  test_expr_x(node->rhs)) {
        gen_expr_x(node->rhs);
        stx_EXT(node->lhs);
        return;
      }
      if (node->lhs->kind == ND_DEREF
      &&  test_expr_x(node->rhs)
      &&  (addr=is_addr_constant(node->lhs->lhs))!=NULL) {
        gen_expr_x(node->rhs);
        println("\tstx %s",addr);
        return;
      }
    }
    if (lhs->ty->size == 1) {
      if (is_integer_constant(rhs, &val)) {
        if (val==0) {
          if (node->retval_unused) {
            if (can_direct(lhs)) {
              gen_direct(lhs,"clr","clr");
              return;
            }else if (test_addr_x(lhs)) {
              int off = gen_addr_x(node->lhs);
              println("\tclr %d,x",off);
              return;
            }
          }
        }else{ // val!=0
          if (lhs->ty->kind == TY_BOOL) {
            val = !!val;
          }
          if (can_direct(lhs)) {
            ldab_i(val);
            gen_direct(node->lhs,"stab","staa");
            return;
          }else if (test_addr_x(lhs)) {
            ldab_i(val);
            int off = gen_addr_x(node->lhs);
            println("\tstab %d,x",off);
            return;
          }
        }
      }
    }
    if (can_direct(lhs) && lhs->ty->size == 2) {
      if (node->retval_unused && is_integer_constant(rhs, &val)) {
        if (val==0) {
          gen_direct(lhs,"clr","clr");
          return;
        }
      }
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
    &&  node->lhs->ty->size <= 2
    &&  can_direct(node->lhs)) {
      gen_expr(node->rhs);
      gen_direct(node->lhs,"stab","staa");
      invalidate_EXT(node->lhs);
      return;
    }
    if ((ty = is_integer_constant(node->rhs,&val))
    &&  ty->size <= 2
    &&  is_integer_or_ptr(node->ty)
    &&  node->lhs->ty->size <= 2) {
      if (can_direct(node->lhs)) {
        gen_direct(node->rhs,"ldab","ldaa");
        gen_direct(node->lhs,"stab","staa");
      }else if (test_addr_x(node->lhs)){
        int off = gen_addr_x(node->lhs);
        if (node->retval_unused && val==0) {
          clr_x(node->ty,off);
        }else{
          gen_expr(node->rhs);
          store_x(node->ty,off);
        }
      }else if (test_addr_array(node->lhs)) {
        int off = gen_addr_array(node->lhs);
        if (node->retval_unused && val==0) {
          clr_x(node->ty,off);
        }else{
          gen_expr(node->rhs);
          store_x(node->ty,off);
        }
      }else{
        gen_addr(node->lhs);
        tfr_dx();
        if (node->retval_unused && val==0) {
          clr_x(node->ty,0);
        }else{
          gen_direct(node->rhs,"ldab","ldaa");
          store_x(node->ty,0);
        }
      }
      return;
    }
    if (can_direct(node->lhs)) {
      gen_expr(node->rhs);
      gen_direct(node->lhs,"stab","staa");
      return;
    }
    if (test_addr_x(node->lhs)){
      gen_expr(node->rhs);
      int off = gen_addr_x(node->lhs);
      store_x(node->ty,off);
      return;
    }
    if (is_global_var(node->rhs)
    &&  can_direct(node->rhs)) {
      gen_addr(node->lhs);
      tfr_dx();
      gen_direct(node->rhs,"ldab","ldaa");
      store_x(node->ty,0);
      return;
    }
    if (node->lhs->ty->size == 1) {
      gen_expr(node->rhs);
      push1();
      gen_addr(node->lhs);
      tfr_dx();
      pop1();
      println("\tstab 0,x");
      return;
    }else if (node->lhs->ty->size == 2) {
      gen_expr(node->rhs);
      push();
      gen_addr(node->lhs);
      tfr_dx();
      pop();
      println("\tstab 1,x");
      println("\tstaa 0,x");
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
    node->rhs->retval_unused = node->retval_unused;
    gen_expr(node->rhs);
    return;
  case ND_CAST:
    if ((addr=is_addr_constant(node))) {
      println("\tldab #<%s",addr);
      println("\tldaa #>%s",addr);
      return;
    }
    if (is_long_constant(node,&val)) {
      load32i(val);
      return;
    }
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
    return;
  case ND_MEMZERO:
    // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
    if (node->var->name) {
      println("; ND_MEMZERO %s size=%d, offset=%d,  %s %s %d",
                 node->var->name, node->var->ty->size, node->var->offset,
                 __func__, __FILE__, __LINE__);
    }else{
      println("; ND_MEMZERO %s (noname) size=%d, offset=%d,  %s %s %d",
                 node->var->name, node->var->ty->size, node->var->offset,
                 __func__, __FILE__, __LINE__);
    }
    if (!node->var->is_local) {
      ldx_IMM_VAR(node->var->name);
      println("\tclrb");
      int c = count();
      println("L_memzero_%d:", c);
      println("\tstab 0,x");
      println("\tinx");
      println("\tcpx #_%s+%d",node->var->name,node->var->ty->size);
      println("\tbne L_memzero_%d", c);
      IX_invalidate();
    }else if (node->var->ty->size <= 6
    && node->var->ty->size + node->var->offset < 256) {
      ldx_bp();
      println("\tclrb");
      for (int i=0; i<node->var->ty->size; i++){
        println("\tstab %d,x",node->var->offset+i);
      }
    } else if (node->var->ty->size/2 > 255) {
      // decb cannot count more than 255 rounds
      ldd_i(node->var->ty->size);
      push();
      ldd_i(0);
      push();
      println("\tldab @bp+1");
      println("\tldaa @bp");
      if (node->var->offset) {
        println("\taddb #<%d",node->var->offset);
        println("\tadca #>%d",node->var->offset);
      }
      println("\tjsr _memset");
      IX_invalidate();
      remove_args(4);
    } else if (node->var->offset < 255) {
      ldx_bp();
      println("\tclra");
      if (node->var->ty->size>=2) {
        ldab_i(node->var->ty->size/2);
        int c = count();
        println("L_memzero_%d:", c);
        println("\tstaa %d,x",node->var->offset);
        println("\tstaa %d,x",node->var->offset+1);
        println("\tinx");
        println("\tinx");
        println("\tdecb");
        println("\tbne L_memzero_%d", c);
      }
      if (node->var->ty->size % 2) {
        println("\tstaa %d,x",node->var->offset);
      }
      IX_invalidate();
    } else {
      println("\tldab @bp+1");
      println("\tldaa @bp");
      if (node->var->offset) {
        println("\taddb #<%d",node->var->offset);
        println("\tadca #>%d",node->var->offset);
      }
      tfr_dx();
      println("\tclra");
      if (node->var->ty->size>=2) {
        ldab_i(node->var->ty->size/2);
        int c = count();
        println("L_memzero_%d:", c);
        println("\tstaa 0,x");
        println("\tstaa 1,x");
        println("\tinx");
        println("\tinx");
        println("\tdecb");
        println("\tbne L_memzero_%d", c);
      }
      if (node->var->ty->size % 2) {
        println("\tstaa 0,x");
      }
      IX_invalidate();
    }
    return;
  case ND_COND: {
    int c = count();
    char L_else[30];
    char L_end[30];
    sprintf(L_else,"L_else_%d",c);
    sprintf(L_end, "L_end_%d",c);

    if (!gen_jump_if_false(node->cond,L_else)){
      assert(0);
    }
    gen_expr(node->then);
    println("\tjmp %s",L_end);
    println("%s:", L_else);
    IX_invalidate();
    gen_expr(node->els);
    println("%s:", L_end);
    IX_invalidate();
    return;
  }
  case ND_NOT: {
    if (is_int8(node->lhs->ty)) {
      gen_expr(node->lhs);
      println("\tnegb");
      println("\tldab #1");
      println("\tsbcb #0");
      if (!is_int8(node->ty)) {
        println("\tclra");
      }
      return;
    }
    if (is_int16_or_ptr(node->lhs->ty)) {
      gen_expr(node->lhs);
      println("\taba");
      println("\tadca #0");
      println("\tnega");
      println("\tldaa #0");
      println("\tldab #1");
      println("\tsbcb #0");
      return;
    }
    int c = count();
    char *L_not = new_label_count("L_not_%d",c);
    char *L_end = new_label_count("L_end_%d",c);

    if (!gen_jump_if_false(node->lhs,L_not)){
      assert(0);
    }
    ldab_i(0);
    println("\tbra %s",L_end);
    println("%s:", L_not);
    ldab_i(1);
    println("%s:", L_end);
    if (!is_int8(node->ty)) {
      println("\tclra");
    }
    return;
  }
  case ND_BITNOT:
    assert(is_integer(node->ty));
    gen_expr(node->lhs);
    switch(node->ty->size){
    case 1:
      println("\tcomb");
      return;
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
    char L_false[32];
    char L_end[32];
    sprintf(L_false,"L_and_%d",c);
    sprintf(L_end,  "L_end_%d",c);

    if (!gen_jump_if_false(node->lhs,L_false)){
      assert(0);
    }
    if (!gen_jump_if_false(node->rhs,L_false)){
      assert(0);
    }
    ldab_i(1);
    println("\tbra %s",L_end);
    println("%s:",L_false);
    ldab_i(0);
    println("L_end_%d:", c);
    if (!is_int8(node->ty)) {
      println("\tclra");
    }
    IX_invalidate();
    return;
  }
  case ND_LOGOR: {
    int c = count();
    char L_true[32], L_end[32];
    sprintf(L_true,"L_or_%d",c);
    sprintf(L_end, "L_end_%d" ,c);

    if (!gen_jump_if_true(node->lhs,L_true)) {
      assert(0);
    }
    if (!gen_jump_if_true(node->rhs,L_true)) {
      assert(0);
    }
    ldab_i(0);
    println("\tbra %s",L_end);
    println("%s:",L_true);
    ldab_i(1);
    println("%s:", L_end);
    if (!is_int8(node->ty)) {
      println("\tclra");
    }
    IX_invalidate();
    return;
  }
  case ND_FUNCALL:
    gen_funcall(node);
    return;
  case ND_LABEL_VAL:
    println("\tldab #<%s",node->unique_label);
    println("\tldaa #>%s",node->unique_label);
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
    IX_invalidate();
    remove_args(4);
    return;
  }
  // Above is a unary operator
  //
  // Below is a binary operator
  switch (node->lhs->ty->kind) {
  case TY_LONG: {
    switch (node->kind) {
    case ND_ADD:
      if (is_long_constant(rhs,&val)) {
        gen_expr(lhs);                         // @long = lhs
        gen_direct_long_addsub_imm(node,val);  // @long += val
        return;
      }
      if (can_direct_long2(node)){
        gen_direct_long2(node);     // @long = lhs + rhs
        return;
      }
      if (!opt('O','s')) {
        if (can_direct_long(node)){
          gen_expr(lhs);
          gen_direct_long(node);   // @long += rhs
          return;
        }
      }
      gen_long_tos(node);           // push lhs; @long += TOS (and remove TOS)
      return;
    case ND_SUB:
      if (is_long_constant(rhs,&val)) {
        gen_expr(lhs);              // @long = lhs
        gen_direct_long_addsub_imm(node,val);  // @long -= val
        return;
      }
      if (can_direct_long2(node)){
        gen_direct_long2(node);     // @long = lhs - rhs
        return;
      }
      if (!opt('O','s')) {
        if (can_direct_long(node)){
          gen_expr(lhs);
          gen_direct_long(node);    // @long -= rhs
          return;
        }
      }
      gen_long_tos(node);           // push lhs; @long -= TOS (and remove TOS)
      return;
    case ND_MUL:
      if (node->lhs->kind     == ND_CAST
      &&  node->lhs->ty->kind == TY_LONG
      &&  is_int16(node->lhs->lhs->ty)
      &&  node->rhs->kind     == ND_CAST
      &&  node->rhs->ty->kind == TY_LONG
      &&  is_int16(node->rhs->lhs->ty)
      &&  node->lhs->lhs->ty->is_unsigned == node->rhs->lhs->ty->is_unsigned){
        gen_expr(lhs->lhs);
        push();
        gen_expr(rhs->lhs);
        if (node->lhs->lhs->ty->is_unsigned) {
          println("\tjsr __mul16x16u_32");
        }else{
          println("\tjsr __mul16x16s_32");
        }
        ins(2);
        IX_invalidate();
        return;
      }
      if (node->lhs->kind == ND_NUM) {
        gen_direct_pushl(node->lhs->val);
      }else if (test_addr_x(node->lhs)){
        int off = gen_addr_x(node->lhs);
        pushlx(off);
      }else{
        gen_expr(node->lhs);
        pushl();
      }
      gen_expr(node->rhs);
      println("\tjsr __mul32tos");	// @long *= TOS, pull TOS");
      IX_invalidate();
      depth -= 4;
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
      IX_invalidate();
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
      IX_invalidate();
      return;
    case ND_BITAND:
    case ND_BITOR:
    case ND_BITXOR: {
      if (is_long_constant(rhs,&val)) {
        gen_expr(lhs);                        // @long = lhs
        gen_direct_long_bitop_imm(node,val);  // @long op= val
        return;
      }
      if (can_direct_long2(node)) {
        gen_direct_long2(node);       // @long = lhs op rhs
        return;
      }
      if (!opt('O','s')) {
        if (can_direct_long(node)){
          gen_expr(lhs);
          gen_direct_long(node);      // @long op= rhs
          return;
        }
      }
      gen_long_tos(node);           // push lhs; @long op= TOS (and remove TOS)
      return;
    } // ND_BITAND, ND_BITOR, ND_BITXOR
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_GT:
    case ND_GE: // long relop long
      if (node->rhs->kind == ND_NUM && node->rhs->ty->kind==TY_LONG) {
        gen_direct_pushl(node->rhs->val);
      }else if (test_addr_x(node->rhs)){
        int off = gen_addr_x(node->rhs);
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
      IX_invalidate();
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
        ldab_i(val);
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
      return;
    default:
      error_tok(node->tok, "TY_LONG: invalid expression");
    }
  } // TY_LONG:
  }
  // The following is a binary operator, length less than or equal to an int
  switch (node->kind) {
  case ND_ADD: {
    if (is_int8(node->ty)) {
      if (can_direct_char(node->rhs)){
        gen_expr(node->lhs);
        if(gen_direct_char(node->rhs,"addb",NULL))
          return;
        assert(0);
      }
      gen_expr(node->lhs);
      push1();
      gen_expr(node->rhs);
      popa();
      println("\taba");
      println("\ttab");
      return;
    }
    if (gen_direct_lr(node,"addb","adca"))
      return;
    if (node->lhs->kind == ND_CAST
    &&  is_int16(node->lhs->ty)
    &&  is_int8(node->lhs->lhs->ty)
    &&  node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  is_int16(node->rhs->ty)
    &&  is_int8(node->rhs->lhs->ty)
    &&  node->rhs->lhs->ty->is_unsigned ) {
      gen_expr(node->lhs->lhs);
      push1();
      gen_expr(node->rhs->lhs);
      popa();
      println("\taba");
      println("\ttab");
      println("\tldaa #0");
      println("\trola");
      return;
    }
    if (node->lhs->kind == ND_CAST
    &&  is_int16(node->lhs->ty)
    &&  is_int8(node->lhs->lhs->ty)
    &&  !node->lhs->lhs->ty->is_unsigned
    &&  node->rhs->kind == ND_CAST
    &&  is_int16(node->rhs->ty)
    &&  is_int8(node->rhs->lhs->ty)
    &&  !node->rhs->lhs->ty->is_unsigned ) {
      if ((test_addr_x(node->lhs->lhs) || is_var_addr_constant(node->lhs->lhs))
      &&  (test_addr_x(node->rhs->lhs) || is_var_addr_constant(node->rhs->lhs))) {
        println("\tclra");
        if ((addr = is_var_addr_constant(node->lhs->lhs))) {
          println("\tldab %s",addr);
        }else{
          off = gen_addr_x(node->lhs->lhs);
          println("\tldab %d,x",off);
        }
        if ((addr = is_var_addr_constant(node->rhs->lhs))) {
          println("\taddb %s",addr);
        }else{
          off = gen_addr_x(node->rhs->lhs);
          println("\taddb %d,x",off);
        }
        char *label = new_jump_label();
        println("\tbge %s",label);
        println("\tdeca");
        println("%s:",label);
        IX_invalidate();
        return;
      }
      char *label = new_jump_label();
      gen_expr(node->lhs->lhs);
      push1();
      gen_expr(node->rhs->lhs);
      println("\tclra");
      println("\ttsx");
      println("\taddb 0,x");
      println("\tbge %s",label);
      println("\tdeca");
      println("%s:",label);
      IX_invalidate();
      ins(1);
      return;
    }
    if (node->rhs->kind     == ND_CAST
    &&  is_int16(node->rhs->ty)
    &&  !node->rhs->ty->is_unsigned
    &&  is_int8(node->rhs->lhs->ty)
    &&  !node->rhs->lhs->ty->is_unsigned
    &&  test_addr_x(node->rhs->lhs)) {
      gen_expr(node->lhs);
      off = gen_addr_x(node->rhs->lhs);
      char *label = new_jump_label();
      println("\ttst %d,x",off);
      println("\tbpl %s",label);
      println("\tdeca");
      println("%s:",label);
      println("\taddb %d,x",off);
      println("\tadca #0");
      return;
    }
    if (node->lhs->kind == ND_MUL
    &&  node->lhs->ty == ty_int
    &&  is_integer_constant(node->lhs->rhs, &val)
    &&  val==2
    &&  is_int16(node->lhs->lhs->ty)
    &&  test_addr_x(node->lhs->lhs)) {
      gen_expr(node->rhs);
      off = gen_addr_x(node->lhs->lhs);
      println("\taddb %d,x",off+1);
      println("\tadca %d,x",off);
      println("\taddb %d,x",off+1);
      println("\tadca %d,x",off);
      return;
    }
    if (node->lhs->kind == ND_MUL
    &&  is_int16(node->lhs->ty)
    &&  node->lhs->lhs->kind == ND_CAST
    &&  is_int16(node->lhs->lhs->ty)
    &&  is_integer_constant(node->lhs->rhs, &val)
    &&  val==2
    &&  is_int8(node->lhs->lhs->lhs->ty)
    &&  node->lhs->lhs->lhs->ty->is_unsigned
    &&  test_addr_x(node->lhs->lhs->lhs)) {
      gen_expr(node->rhs);
      off = gen_addr_x(node->lhs->lhs->lhs);
      println("\taddb %d,x",off);
      println("\tadca #0");
      println("\taddb %d,x",off);
      println("\tadca #0");
      return;
    }
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_invalidate();
    println("\taddb 1,x");
    println("\tadca 0,x");
    IX_invalidate();
    ins(2);
    return;
  } // ND_ADD
  case ND_SUB:
    // (- TY_CHAR(2) (ND_VAR ty_char x +1 ) (ND_VAR ty_char x +1 ))
    if (is_int8(node->ty)) {
      if (can_direct_char(node->rhs)){
        gen_expr(node->lhs);
        if(gen_direct_char(node->rhs,"subb",NULL))
          return;
        assert(0);
      }
      gen_expr(node->lhs);
      push1();
      gen_expr(node->rhs);
      popa();
      println("\tsba");
      println("\ttab");
      return;
    }
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
    // A decayed VLA keeps its pointer value in a slot, so the value is read
    // from the slot and not from the array the slot points to.
    if (node->rhs->kind == ND_VAR
    &&  node->rhs->var->ty->kind == TY_VLA
    &&  node->rhs->var->offset <= 254){	// the slot read is two bytes
      gen_expr(node->lhs);
      ldx_bp();
      println("\tsubb %d+1,x",node->rhs->var->offset);
      println("\tsbca %d,x",node->rhs->var->offset);
      return;
    }
    if (node->lhs->kind == ND_VAR
    &&  node->lhs->var->ty->kind == TY_VLA
    &&  node->lhs->var->offset <= 254){
      gen_expr(node->rhs);
      negd();
      ldx_bp();
      println("\taddb %d+1,x",node->lhs->var->offset);
      println("\tadca %d,x",node->lhs->var->offset);
      return;
    }
    if (test_addr_x(node->rhs)){
      gen_expr(node->lhs);
      int off = gen_addr_x(node->rhs);
      println("\tsubb %d+1,x",off);
      println("\tsbca %d,x",off);
      return;
    }
    if (test_addr_x(node->lhs)){
      gen_expr(node->rhs);
      negd();
      int off = gen_addr_x(node->lhs);
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
      if ((test_addr_x(node->lhs->lhs) || is_var_addr_constant(node->lhs->lhs))
      &&  (test_addr_x(node->rhs->lhs) || is_var_addr_constant(node->rhs->lhs))) {
        println("\tclra");
        if ((addr = is_var_addr_constant(node->lhs->lhs))) {
          println("\tldab %s",addr);
        }else{
          off = gen_addr_x(node->lhs->lhs);
          println("\tldab %d,x",off);
        }
        if ((addr = is_var_addr_constant(node->rhs->lhs))) {
          println("\tsubb %s",addr);
        }else{
          off = gen_addr_x(node->rhs->lhs);
          println("\tsubb %d,x",off);
        }
        char *label = new_jump_label();
        println("\tbge %s",label);
        println("\tdeca");
        println("%s:",label);
        IX_invalidate();
        return;
      }
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
    IX_invalidate();
    ins(2);
    return;
  case ND_MUL:
    if (gen_mul8u(node)) {
      return;
    }
    if (gen_mul8s(node)) {
      return;
    }
    if (gen_mul16(node)) {
      return;
    }
    assert(0);
  case ND_DIV:
    if (node->lhs->ty ==  node->ty){
      switch(node->rhs->kind){
      case ND_NUM:
        switch (node->rhs->ty->kind) {
        case TY_INT:
        case TY_SHORT:
          if (node->lhs->ty->is_unsigned){
            switch(node->rhs->val){
            case 2:
            case 4:
            case 8:
            case 16:
              gen_expr(node->lhs);
              int n = exact_log2(node->rhs->val);
              for (int i=0; i<n; i++) {
                println("\tlsra");
                println("\trorb");
              }
              return;
            }
          }else{
            switch(node->rhs->val){
            case 2:
              gen_expr(node->lhs);
              println("\tasra");
              println("\trola");
              println("\tadcb #0");
              println("\tadca #0");
              println("\tasra");
              println("\trorb");
              return;
            }
          }
        }
      }
    }
    if (node->ty->kind == TY_CHAR && node->ty->is_unsigned
    &&  node->lhs->ty->kind == TY_CHAR && node->lhs->ty->is_unsigned
    &&  node->rhs->ty->kind == TY_CHAR && node->rhs->ty->is_unsigned) {
      gen_expr(node->rhs);
      push1();
      gen_expr(node->lhs);
      println("\tjsr __div8x8u");
      IX_invalidate();
      ins(1);
      return;
    }
    // 16bit / 16bit
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    if (node->ty->is_unsigned) {
      println("\tjsr __div16x16u");
    }else{
      println("\tjsr __div16x16s");
    }
    IX_invalidate();
    ins(2);
    return;
  case ND_MOD:
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    if (node->ty->is_unsigned) {
      println("\tjsr __rem16x16u");
    }else{
      println("\tjsr __rem16x16s");
    }
    IX_invalidate();
    ins(2);
    return;
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR: {
    char *opb, *opa;
    switch (node->kind) {
    case ND_BITAND: opb = "andb"; opa = "anda"; break;
    case ND_BITOR:  opb = "orab"; opa = "oraa"; break;
    case ND_BITXOR: opb = "eorb"; opa = "eora"; break;
    default: assert(0);
    }
    if (is_int8(node->ty)) {
      if (gen_direct_lr(node, opb, NULL))
        return;
      gen_expr(node->lhs);
      push1();
      gen_expr(node->rhs);
      println("\ttsx");
      println("\t%s 0,x", opb);
      IX_invalidate();
      ins(1);
      return;
    }
    if (gen_direct_lr(node, opb, opa))
      return;
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    println("\t%s 1,x", opb);
    println("\t%s 0,x", opa);
    IX_invalidate();
    ins(2);
    return;
  } // ND_BITAND, ND_BITOR, ND_BITOXR
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE: // 8 ot 16bit relop
    // 8bit relop 8bit ?
    if (is_int8(node->lhs->ty) && is_int8(node->rhs->ty)) {
      assert(node->lhs->ty->is_unsigned == node->rhs->ty->is_unsigned);
      if (is_integer_constant(node->rhs, &val) &&  val==0){
        gen_expr(node->lhs);
        if (node->lhs->ty->is_unsigned) {
          switch(node->kind) {
          case ND_EQ: println("\tjsr __eq8_zero"); return;
          case ND_NE: println("\tjsr __ne8_zero"); return;
          case ND_LT: println("\tclrb");
                      println("\tclra");
                      return;
          case ND_LE: println("\tjsr __eq8_zero"); return;
          case ND_GT: println("\tjsr __ne8_zero"); return;
          case ND_GE: println("\tclra");
                      println("\tldab #1");
                      return;
          default:    assert(0);
          }
        }else{  // signed
          switch(node->kind) {
          case ND_EQ: println("\tjsr __eq8_zero");    return;
          case ND_NE: println("\tjsr __ne8_zero");    return;
          case ND_LT: println("\tjsr __lt8s_zero");   return;
          case ND_GE: println("\tjsr __ge8s_zero");   return;
          case ND_LE: println("\tjsr __le8s_zero");   return;
          case ND_GT: println("\tjsr __gt8s_zero");   return;
          default:    assert(0);
          }
        }
      }
      if (can_direct_char(node->rhs)){
        gen_expr(node->lhs);
        if(!gen_direct_char(node->rhs,"subb",NULL)) {
          assert(0);
        }
      }else{
        gen_expr(node->lhs);
        push1();
        gen_expr(node->rhs);
        popa();
        println("\tsba");
      }
      switch(node->kind){
      case ND_EQ:
        println("\tjsr __eq8");
        break;
      case ND_NE:
        println("\tjsr __ne8");
        break;
      case ND_LT:
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __lt8u");
        else
          println("\tjsr __lt8s");
        break;
      case ND_LE:
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __le8u");
        else
          println("\tjsr __le8s");
        break;
      case ND_GT:
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __gt8u");
        else
          println("\tjsr __gt8s");
        break;
      case ND_GE:
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __ge8u");
        else
          println("\tjsr __ge8s");
        break;
      default:
        assert(0);
      }
      return;
    }
    // must be 16bit op 16bit
    // must be 16bit op 16bit
    if (!is_int16_or_ptr_or_array(node->lhs->ty)
    ||  !is_int16_or_ptr_or_array(node->rhs->ty))
      error_tok(node->tok, "internal error: not 16bit op 16bit");
    // expr op 0
    if (is_integer_constant(node->rhs, &val) && val==0){
      gen_expr(node->lhs);
      if (node->lhs->ty->is_unsigned) {
        switch(node->kind) {
        case ND_EQ: println("\tjsr __eq16_zero");   return;
        case ND_NE: println("\tjsr __ne16_zero");   return;
        case ND_LT: println("\tclrb");
                    println("\tclra");
                    return;
        case ND_LE: println("\tjsr __eq16_zero");   return;
        case ND_GT: println("\tjsr __ne16_zero");   return;
        case ND_GE: println("\tclra");
                    println("\tldab #1");
                    return;
        default:    assert(0);
        }
      }else{
        switch(node->kind) { // signed
        case ND_EQ: println("\tjsr __eq16_zero");   return;
        case ND_NE: println("\tjsr __ne16_zero");   return;
        case ND_LT: println("\tjsr __lt16s_zero");  return;
        case ND_LE: println("\tjsr __le16s_zero");  return;
        case ND_GT: println("\tjsr __gt16s_zero");  return;
        case ND_GE: println("\tjsr __ge16s_zero");  return;
        default:    assert(0);
        }
      }
    }
    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if(!gen_direct(node->rhs,"subb","sbca")) {
        assert(0);
      }
    }else{
      gen_expr(node->rhs);
      push();
      gen_expr(node->lhs);
      println("\ttsx");
      IX_invalidate();
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
      if (node->ty->size < node->lhs->ty->size) {
        gen_shl(node->ty,val);
      }else{
        gen_shl(node->lhs->ty,val);
      }
      return;
    }
    switch(node->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
      char *skip = new_jump_label();
      char *loop = new_jump_label();
      if (can_direct_char(node->rhs)) {
        gen_expr(node->lhs);
        if(!gen_direct_char(node->rhs,"ldaa",NULL)) {
          assert(0);
        }
      }else{
        gen_expr(node->lhs);
        push1();
        gen_expr(node->rhs);
        println("\ttba");
        pop1();
      }
      if (opt('O','3')) {
        char *jump  = new_jump_label();
        //println("\tbeq %s",skip);
        println("\tsuba #8");
        println("\tbcc %s",loop);
        println("\tnega");
        println("\tstaa %s+1",jump);
        println("\tldx #%s",loop);
        println("%s:",jump);
        println("\tjmp 0,x");
        println("%s:",loop);
        for (int i=0; i<8; i++) {
          println("\taslb");
        }
        println("%s:",skip);
        IX_invalidate();
        return;
      }
      println("\tbeq %s",skip);
      println("%s:",loop);
      println("\taslb");
      println("\tdeca");
      println("\tbne %s",loop);
      println("%s:",skip);
      return;
    }
    gen_expr(node->rhs);
    push1();
    gen_expr(node->lhs);
//  shl16: AccAB << TOS(8bit)
    if (opt('O','2')) {
      char *skip = new_jump_label();
      char *loop = new_jump_label();
      println("\ttsx");
      println("\ttst 0,x");
      println("\tbeq %s",skip);
      println("%s:",loop);
      println("\taslb");
      println("\trola");
      println("\tdec 0,x");
      println("\tbne %s",loop);
      println("%s:",skip);
      IX_invalidate();
      ins(1);
      return;
    }
    println("\tjsr __shl16");
    IX_invalidate();
    ins(1);
    return;
  } // ND_SHL
  case ND_SHR: {
    int64_t val;

    if (is_integer_constant(node->rhs, &val)){
      gen_expr(node->lhs);
      gen_shr(node->lhs->ty,val);
      return;
    }
    switch (node->lhs->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
      char *skip = new_jump_label();
      char *loop = new_jump_label();
      if (can_direct_char(node->rhs)) {
        gen_expr(node->lhs);
        if(!gen_direct_char(node->rhs,"ldaa",NULL)) {
          assert(0);
        }
      }else{
        gen_expr(node->lhs);
        push1();
        gen_expr(node->rhs);
        println("\ttba");
        pop1();
      }
      if (opt('O','3')) {
        char *jump  = new_jump_label();
        //println("\tbeq %s",skip);
        println("\tsuba #8");
        println("\tbcc %s",loop);
        println("\tnega");
        println("\tstaa %s+1",jump);
        println("\tldx #%s",loop);
        println("%s:",jump);
        println("\tjmp 0,x");
        println("%s:",loop);
        for (int i=0; i<8; i++) {
          if (node->lhs->ty->is_unsigned){
            println("\tlsrb");
          }else{
            println("\tasrb");
          }
        }
        println("%s:",skip);
        IX_invalidate();
        return;
      }
      println("\tbeq %s",skip);
      println("%s:",loop);
      if (node->lhs->ty->is_unsigned){
        println("\tlsrb");
      }else{
        println("\tasrb");
      }
      println("\tdeca");
      println("\tbne %s",loop);
      println("%s:",skip);
      return;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
      gen_expr(node->rhs);
      push1();
      gen_expr(node->lhs);
      //  shr16: AccAB >> TOS(8bit)
      if (opt('O','2')) {
        char *skip = new_jump_label();
        char *loop = new_jump_label();
        println("\ttsx");
        println("\ttst 0,x");
        println("\tbeq %s",skip);
        println("%s:",loop);
        if (node->lhs->ty->is_unsigned){
          println("\tlsra");
          println("\trorb");
        }else{
          println("\tasra");
          println("\trorb");
        }
        println("\tdec 0,x");
        println("\tbne %s",loop);
        println("%s:",skip);
      }else if (node->lhs->ty->is_unsigned){
        println("\tjsr __shr16u");
      }else{
        println("\tjsr __shr16s");
      }
      IX_invalidate();
      ins(1);
      return;
    default:
      assert(0);
    }
  } // ND_SHR
  default: ;
  }
  error_tok(node->tok, "invalid expression");
}

void stmt_dump(char *p)
{
  char s[1024];
  char *q = s;
  static char *pp = NULL;

  if (!opt('g','2') && !opt('g','3')) {
    return;
  }
  if (!p) {
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
  if (opt('g','3')) {
    ast_node_dump(node);
  }

  switch (node->kind) {
  case ND_IF: {
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
#if 0
    fprintf(stderr, "IF %p cond=%p kind=%d lhs=%p line=%d\n",
        (void*)node, (void*)node->cond, node->cond->kind,
        (void*)node->cond->lhs, node->cond->tok->line_no);
#endif
    node->cond = optimize_condition(node->cond);
    if (!gen_jump_if_false(node->cond,L_else)){
      assert(0);
    }
    gen_stmt(node->then);
    if (node->els){
      println("\tjmp %s", L_end);
      println("%s:", L_else);
      IX_invalidate();
      gen_stmt(node->els);
    }
    println("L_end_%d:", c);
    IX_invalidate();
    return;
  }
  case ND_FOR: {
    int64_t val;
    char if_false[30];
    int c = count();
    sprintf(if_false,"%s",node->brk_label);
    if (node->init) {
      if (opt('g','3')) {
        stmt_dump(node->init->loc);
        ast_node_dump(node->init);
      }
      gen_stmt(node->init);
    }
    println("L_begin_%d:", c);
    IX_invalidate();
    if (node->cond) {
      node->cond = optimize_condition(node->cond);
      if (opt('g','3')) {
        stmt_dump(node->cond->loc);
        ast_node_dump(node->cond);
      }
      if (is_integer_constant(node->cond,&val)) {
        if (val==0) {
//        println("\tjmp %s", if_false);
          return; // conditon always false, no code generated
        }
      }else{
        if (!gen_jump_if_false(node->cond,if_false)){
          assert(0);
//        gen_expr(node->cond);
//        if (!is_compare_or_not(node->cond))
//          cmp_zero(node->cond->ty);
//        println("\tjeq %s", if_false);
        }
      }
    }
    gen_stmt(node->then);
    if (check_used_label(node->cont_label)) {
      println("%s:", node->cont_label);
      IX_invalidate();
    }
    if (node->inc) {
      node->inc = optimize_expr(node->inc);
      if (opt('g','3')) {
        stmt_dump(node->inc->loc);
        ast_node_dump(node->inc);
      }
      node->inc->retval_unused = true;
      gen_expr(node->inc);
    }
    println("\tjmp L_begin_%d", c);
    println("%s:", if_false);
    IX_invalidate();
    return;
  }
  case ND_DO: {
    int c = count();
    int64_t val;
    char L_begin[30];
    sprintf(L_begin,"L_begin_%d", c);
    println("%s:",L_begin);
    IX_invalidate();
    gen_stmt(node->then);
    if (check_used_label(node->cont_label)) {
      println("%s:", node->cont_label);
      IX_invalidate();
    }
    stmt_dump(node->cond->loc);
    node->cond = optimize_condition(node->cond);
    if (is_integer_constant(node->cond,&val)) {
      if (val!=0) {
        println("\tjmp %s", L_begin);
        IX_invalidate();
      }
    }else{
      if (!gen_jump_if_true(node->cond,L_begin)){
        assert(0);
//      gen_expr(node->cond);
//      if (!is_compare_or_not(node->cond))
//        cmp_zero(node->cond->ty);
//      println("\tjne %s", L_begin);
      }
      IX_invalidate();
    }
    println("%s:", node->brk_label);
    IX_invalidate();
    return;
  } // ND_DO
  case ND_SWITCH: {
    bool has_case_ranges = false;
    bool need_integral_promotion = false;

    node->cond = optimize_expr(node->cond);
    if (node->cond->ty->size == 1) {
      for (Node *n = node->case_next; n; n = n->case_next) {
        if (node->cond->ty->is_unsigned) {
          if ((n->begin<0 || n->begin>255)
          ||  (n->end  <0 || n->end  >255)) {
            need_integral_promotion = true;
            break;
          }
        }else{
          if ((n->begin<-128 || n->begin>127)
          ||  (n->end  <-128 || n->end  >127)) {
            need_integral_promotion = true;
            break;
          }
        }
      }
    }
    if (need_integral_promotion) {
      node->cond = new_cast(node->cond,ty_int);
    }

    for (Node *n = node->case_next; n; n = n->case_next) {
      if (n->begin != n->end) {
        has_case_ranges = true;
        break;
      }
    }
    if (node->cond->ty->size == 2 && !has_case_ranges) {
      if (test_addr_x(node->cond)) {
        int off = gen_addr_x(node->cond);
        ldx_nX(off);
      }else{
        gen_expr(node->cond);
        tfr_dx();
      }
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
          println("\tjeq %s", n->label);
          break;
        case 2:
          if (has_case_ranges) {
            int c = count();
            println("\tcmpb #<%ld",n->begin);
            println("\tbne L_case_%d",c);
            println("\tcmpa #>%ld",n->begin);
            println("\tjeq %s", n->label);
            println("L_case_%d:",c);
            IX_invalidate();
          }else{
            println("\tcpx #%ld",n->begin);
            println("\tjeq %s", n->label);
          }
          break;
        case 4: {
          int c = count();
          println("\tldx #%ld	; %ld",n->begin & 0x0ffff,n->begin);
          println("\tcpx @long+2");
          println("\tbne L_case_%d",c);
          println("\tldx #%ld",(n->begin>>16)&0x0ffff);
          println("\tcpx @long");
          println("\tjeq %s", n->label);
          println("L_case_%d:",c);
          IX_invalidate();
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
        println("\tcmpa #%ld", n->end - n->begin + 1);
        println("\tjcs %s",   n->label);
        break;
      case 2:
        println("\tpshb");
        println("\tpsha");
        println("\tsubb #<%ld", n->begin);
        println("\tsbca #>%ld", n->begin);
        println("\tsubb #<%ld", n->end - n->begin + 1);
        println("\tsbca #>%ld", n->end - n->begin + 1);
        println("\tpula");
        println("\tpulb");
        println("\tjcs %s", n->label);
        break;
      case 4:
        // TODO: case range
        assert(0);
      }
    }

    if (node->default_case)
      println("\tjmp %s", node->default_case->label);

    println("\tjmp %s", node->brk_label);
    IX_invalidate();
    gen_stmt(node->then);
    println("%s:", node->brk_label);
    IX_invalidate();
    return;
  } // ND_SWITCH
  case ND_CASE:
    println("%s:", node->label);
    IX_invalidate();
    gen_stmt(node->lhs);
    return;
  case ND_BLOCK:
    for (Node *n = node->body; n; n = n->next)
      gen_stmt(n);
    return;
  case ND_GOTO:
    println("\tjmp %s", node->unique_label);
    mark_used_label(node->unique_label);
    return;
  case ND_GOTO_EXPR:
    node->lhs = optimize_expr(node->lhs);
    gen_expr(node->lhs);
    println("\tpshb	; jmp [AccD]");
    println("\tpsha");
    println("\trts");
    println(";");
    return;
  case ND_LABEL:
    println("%s:", node->unique_label);
    IX_invalidate();
    gen_stmt(node->lhs);
    return;
  case ND_RETURN:
    if (node->lhs) {
      node->lhs = optimize_expr(node->lhs);
      gen_expr(node->lhs);
      Type *ty = node->lhs->ty;
      switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
        copy_struct_mem();
        break;
      }
    }
    if (!current_fn->use_bp) {
      if (opt('g','2')) {
        println("; function has no params & locals");
      }
      IX_invalidate();
      println("\trts");
      return;
    }
    println("\tjmp L_return_%d", current_fn->function_no);
    current_fn->return_count++;
//    println("\tjmp L_return_%s", current_fn->name);
    return;
  case ND_EXPR_STMT:
    if (opt('g','4')) {
      ast_node_dump(node->lhs);
    }
    node->lhs = optimize_expr(node->lhs);
    if (opt('g','4')) {
      ast_node_dump(node->lhs);
    }
    node->lhs->retval_unused = true;
    gen_expr(node->lhs);
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

typedef struct LongLiteral LongLiteral;
struct LongLiteral {
  LongLiteral *next;
  uint32_t     bits;
};

static LongLiteral *long_literals;

// Return the label of the .data entry that holds a long constant.
char *long_literal_label(int64_t val)
{
  uint32_t bits = (uint32_t)val;
  LongLiteral *ll;

  for (ll = long_literals; ll; ll = ll->next) {
    if (ll->bits == bits) {
      return format("CL_%08x",bits);
    }
  }
  ll = calloc(1,sizeof(LongLiteral));
  ll->bits = bits;
  ll->next = long_literals;
  long_literals = ll;
  return format("CL_%08x",bits);
}

static void emit_long_literals(void)
{
  if (!long_literals) {
    return;
  }
  println("\t.data");
  for (LongLiteral *ll = long_literals; ll; ll = ll->next) {
    println(";");
    println("; long %ld",(long)(int32_t)ll->bits);
    println("CL_%08x:",ll->bits);
    println("\t.word %d",(ll->bits>>16) & 0xffff);
    println("\t.word %d",(ll->bits    ) & 0xffff);
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

    // Prologue
    if (opt('g','2')) {
      println("; function %s prologue emit_text",fn->name);
      println("; function %s use alloca/vla %d use functions %d",
                  fn->name,fn->use_alloca,fn->use_funcall);
      println("; fn->body->kind=%d",fn->body->kind);
    }
    println("\t.code");
//  println("\t.type %s, @function", fn->name);
    println("_%s:", fn->name);
    current_fn = fn;
    current_fn->function_no = count();
    current_fn->return_count = 0;
    if(fn->body->kind==ND_BLOCK && fn->body->body==NULL){	// empty function
      println("\trts	; empty function");
      println(";");
      IX_invalidate();
      continue;
    }
    fn->use_bp = false;
    if (!fn->params
    &&  !fn->stack_size
    &&  !fn->use_alloca
    &&  (fn->ty->return_ty->kind != TY_STRUCT)
    &&  (fn->ty->return_ty->kind != TY_UNION)) {
      println("; function has no params & locals, not use @bp");
      IX_invalidate();
      depth = 0;
      goto no_params_locals;
    }
    fn->use_bp = true;

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
    if (opt('O','s')) {
      println("\tjsr __prologue_%d",reg_param_size);
    } else {
      if (reg_param_size==2) {
        println("\tstaa @tmp1");
      }
      println("\tldaa @bp+1");			// push old @bp
      println("\tpsha");
      println("\tldaa @bp");
      println("\tpsha");
      if (reg_param_size==2) {
        println("\tldaa @tmp1");
      }

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
    }else if (opt('O','s')) {
      println("\tsts @bp");
      if (fn->stack_size-1<=255) {
        ldab_i(fn->stack_size-1);
        println("\tjsr __sub_bp_b");
      }else{
        ldd_i(fn->stack_size-1);
        println("\tjsr __sub_bp_d");
      } 
      println("\ttxs");
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
    }
    IX_Dest = IX_BP;
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
    if (strcmp(fn->name, "main") == 0) {
      switch(fn->ty->return_ty->kind) {
      case TY_VOID:
      case TY_LONG:
      case TY_FLOAT:
      case TY_DOUBLE:
      case TY_LDOUBLE:
        break;
      default:
        println("\tclrb");
        println("\tclra");
        break;
      }
    }

    // Epilogue
    if (fn->return_count){
      println("L_return_%d:", fn->function_no);
    }
    if (opt('g','2')) {
      println("; function %s epilogue emit_text",fn->name);
      println("; recover sp, fn->stack_size=%d reg_param_size=%d",
	   	    	fn->stack_size,reg_param_size);
      println("; fn->ty->return_ty->size = %d", fn->ty->return_ty->size);
      println("; function %s use alloca/vla %d",fn->name,fn->use_alloca);
    }
    if (!fn->use_bp) {
      if (opt('g','2')) {
        println("; function not use @bp");
      }
      IX_invalidate();
      goto no_params_locals2;
    }
    if (fn->stack_size + reg_param_size <= 10){
      if (opt('g','2')) {
        println("; fn->stack_size %d, reg_param_size %d",fn->stack_size,reg_param_size);
      }
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
      case TY_DOUBLE:
      case TY_LDOUBLE:
        break;
      default:
        println("\tpshb");
      }
      if (opt('O','s') && fn->stack_size+reg_param_size-1 < 256) {
        ldab_i(fn->stack_size+reg_param_size-1);
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
      case TY_DOUBLE:
      case TY_LDOUBLE:
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
    case TY_DOUBLE:
    case TY_LDOUBLE:
      if (opt('O','s')) {
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
epilogue_end: ;
  }
}

void codegen(Obj *prog, FILE *out) {
  output_file = out;

  println("\t.setcpu 6800");
  println(";\toption flag -O%c -g%c",opt_O,opt_g);
  File **files = get_input_files();
  for (int i = 0; files[i]; i++) {
    struct stat st;
    char buf[32];

    if (stat(files[i]->name, &st) == 0
    &&  strftime(buf, sizeof(buf), "%b %e %Y %H:%M:%S", localtime(&st.st_mtime))) {
      println(";\t.file %d \"%s\" %s", files[i]->file_no, files[i]->name, buf);
    }else{
      println(";\t.file %d \"%s\"", files[i]->file_no, files[i]->name);
    }
  }
  assign_lvar_offsets(prog);
  emit_text(prog);
  emit_data(prog);
  emit_float_literals();
  emit_long_literals();
}
