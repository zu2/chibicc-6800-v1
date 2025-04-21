#include "chibicc.h"

static FILE *output_file;
static int depth;
static Obj *current_fn;

static void gen_expr(Node *node);
static void gen_stmt(Node *node);

typedef	enum {
  IX_None,
  IX_BP,
  IX_VAR,
} IX_Type;

IX_Type	IX_Dest = IX_None;

__attribute__((format(printf, 1, 2)))
void println(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(output_file, fmt, ap);
  va_end(ap);
  fprintf(output_file, "\n");
}

static int count(void) {
  static int i = 1;
  return i++;
}

static void push1(void) {	// push char parameter
  println("\tpshb");
  depth+=1;
}

static void push(void) {
  println("\tpshb");
  println("\tpsha");
  depth+=2;
}

static void pop1(void) {
  println("\tpulb");
  depth-=1;
}
static void pop(void) {
  println("\tpula");
  println("\tpulb");
  depth-=2;
}

static void popx(void) {
  println("\ttsx	; popx()");
  println("\tldx 0,x");
  IX_Dest = IX_None;
  println("\tins");
  println("\tins");
  depth-=2;
}

static void pushl(void) {
//println("\tjsr __push32");
//IX_Dest = IX_None;
  println("\tldab @long+3");
  println("\tpshb");
  println("\tldab @long+2");
  println("\tpshb");
  println("\tldab @long+1");
  println("\tpshb");
  println("\tldab @long");
  println("\tpshb");
  depth+=4;
}

static void popl() {
  println("\tjsr __pop32");
  depth-=4;
  IX_Dest = IX_None;
}

static void pushf(void) {
  pushl();
}

static void popf(void) {
  popl();
}

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
int align_to(int n, int align) {
  return n;	// 6800 has no alignment restrictions.
//  return (n + align - 1) / align * align;
}


static void ldx_bp()
{
  if (IX_Dest != IX_BP){
    println("; stack depth = %d",depth);
    println("\tldx @bp");	// TSX cannot be used because of VLA/alloca
  }
  IX_Dest = IX_BP;
}

static void tfr_dx()
{
  println("\tstab @tmp1+1");
  println("\tstaa @tmp1");
  println("\tldx @tmp1");
  IX_Dest = IX_None;
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


// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static void gen_addr(Node *node){
  switch (node->kind) {
  case ND_VAR:
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA) {
	println("\tldab @bp+1");
	println("\tldaa @bp");
	println("\taddb #<%d",node->var->offset);
	println("\tadca #>%d",node->var->offset);
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
//      println("  lea %d(%%rbp), %%rax", node->var->offset);
      return;
    }

    if (opt_fpic) {
      // Thread-local variable
      if (node->var->is_tls) {
        println("  data16 lea %s@tlsgd(%%rip), %%rdi", node->var->name);
        println("  .value 0x6666");
        println("  rex64");
        println("  call __tls_get_addr@PLT");
        return;
      }

      // Function or global variable
//      println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name);
      println("\tldab #<%s",node->var->name);
      println("\tldaa #>%s",node->var->name);
      return;
    }

    // Thread-local variable
    if (node->var->is_tls) {
      println("  mov %%fs:0, %%rax");
      println("  add $%s@tpoff, %%rax", node->var->name);
      return;
    }

    // Here, we generate an absolute address of a function or a global
    // variable. Even though they exist at a certain address at runtime,
    // their addresses are not known at link-time for the following
    // two reasons.
    //
    //  - Address randomization: Executables are loaded to memory as a
    //    whole but it is not known what address they are loaded to.
    //    Therefore, at link-time, relative address in the same
    //    exectuable (i.e. the distance between two functions in the
    //    same executable) is known, but the absolute address is not
    //    known.
    //
    //  - Dynamic linking: Dynamic shared objects (DSOs) or .so files
    //    are loaded to memory alongside an executable at runtime and
    //    linked by the runtime loader in memory. We know nothing
    //    about addresses of global stuff that may be defined by DSOs
    //    until the runtime relocation is complete.
    //
    // In order to deal with the former case, we use RIP-relative
    // addressing, denoted by `(%rip)`. For the latter, we obtain an
    // address of a stuff that may be in a shared object file from the
    // Global Offset Table using `@GOTPCREL(%rip)` notation.

    // Function
    if (node->ty->kind == TY_FUNC) {
      if (node->var->is_definition){
//        println("  lea %s(%%rip), %%rax", node->var->name);
        println("\tldab #<_%s", node->var->name);
        println("\tldaa #>_%s", node->var->name);
      }else{
//        println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name);
        println("\tldab #<_%s", node->var->name);
        println("\tldaa #>_%s", node->var->name);
      }
      return;
    }

    // Global variable
//    println("  lea %s(%%rip), %%rax", node->var->name);
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
//    println("  add $%d, %%rax", node->member->offset);
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
//    println("  lea %d(%%rbp), %%rax", node->var->offset);
    return;
  }

  error_tok(node->tok, "not an lvalue");
}

// Compute the absolute address of a given node in IX.
// It's an error if a given node does not reside in memory.
static int gen_addr_x(Node *node,bool save_d)
{
  switch (node->kind) {
  case ND_VAR:
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA){
      if (node->var->offset<=254) {
	ldx_bp();
        println("\tldx %d,x	; gen_addr_x():TY_LDA ",node->var->offset);
	IX_Dest = IX_None;
        return 0;
      }
      assert(0); // TODO:
    }
    // Local variable
    if (node->var->is_local) {
      if (node->var->offset <= 254){
	ldx_bp();
	return node->var->offset;
      }
      assert(0); // TODO:
    }
    // Function
    if (node->ty->kind == TY_FUNC) {
      println("\tldx #_%s",node->var->name);
      IX_Dest = IX_None;
      return 0;
    }
    // maybe Global variable
    println("\tldx #_%s",node->var->name);
    IX_Dest = IX_None;
    return 0;
    break;
  }
  // fallback to gen_addr()
  if (save_d)
    push();
  gen_addr(node);
  tfr_dx();
  if (save_d)
    pop();
  return 0;
}

//
// Can the address of this node be calculated using only the IX register?
//
int test_addr_x(Node *node)
{
  switch (node->kind) {
  case ND_VAR: {
    // Variable-length array, which is always local.
    if (node->var->ty->kind == TY_VLA){
      return 0;	// It's buggy.
      //return (node->var->offset + node->var->ty->size <= 256);
    }
    // Local variable
    if (node->var->is_local) {
      return  (node->var->offset + node->var->ty->size <= 256);
    }
    // Function
    if (node->ty->kind == TY_FUNC) {
      return 1;
    }
    // maybe Global variable
    return 0;	// It's buggy.
    return 1;
  } // ND_VAR
  case ND_DEREF: {
    Node *lhs = node->lhs;
    switch (lhs->kind){
    case ND_ADD:
      return 0;
    case ND_VAR:
      return test_addr_x(lhs);
    }
    return 0;
  }
  default:
  } // switch
  return 0;
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
    println("\tjsr __load32	; @long = (AccAB)");
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
    if (ty->is_unsigned){
      println("\tjsr __load8u	; AccB = (AccAB)");
    }else{
      println("\tjsr __load8s	; AccB = (AccAB)");
    }
  }else if (ty->size == 2){
    println("\tjsr __load16	; AccAB = (AccAB)");
  }else if (ty->size == 4){
    println("\tjsr __load32	; @long = (AccAB)");
  }else{
    fprintf(stderr,"; load error, ty->size=%d\n",ty->size);
    assert(0);
  }
  IX_Dest = IX_None;
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
    println("\tldab %d,x",off+3);
    println("\tstab @long+3");
    println("\tldab %d,x",off+2);
    println("\tstab @long+2");
    println("\tldab %d,x",off+1);
    println("\tstab @long+1");
    println("\tldab %d,x",off);
    println("\tstab @long");
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
    println("\tclra");
    println("\tldab %d,x",off);
    if (!ty->is_unsigned){
      println("\tasrb");
      println("\trolb");
      println("\tsbca #0");
    }
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
// Store D to an address that the stack top is pointing to.
static void store(Type *ty) {
  println("\ttsx");
  println("\tldx 0,x");
  IX_Dest = IX_None;
  println("\tins");
  println("\tins");
  depth -= 2;

  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    println("\tstab @tmp2+1");
    println("\tstaa @tmp2");
    println("\tstx  @tmp3");
    for (int i = 0; i < ty->size; i++) {
	println("\tldx @tmp2");
	println("\tldab %d,x",i);
	println("\tldx @tmp3");
	println("\tstab %d,x",i);
        IX_Dest = IX_None;
    }
    return;
  case TY_FLOAT:
    println("\tjsr __store32x   ; store @long to (0-3,x)");
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
    return;
  }

  if (ty->size == 1){
    println("\tstab 0,x");
  }else if (ty->size == 2){
    println("\tstab 1,x");
    println("\tstaa 0,x");
  }else if (ty->size == 4){
    println("\tjsr __store32x   ; store @long to (0-3,x)");
  }else
    println("  mov %%rax, (%%rdi)");
}

static void store_x(Type *ty,int off) {
  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    println("\tstab @tmp2+1");
    println("\tstaa @tmp2");
    println("\tstx  @tmp3");
    for (int i = 0; i < ty->size; i++) {
	println("\tldx @tmp2");
	println("\tldab %d,x",i);
	println("\tldx @tmp3");
        IX_Dest = IX_None;
	println("\tstab %d,x",i+off);
    }
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
    return;
  }

  if (ty->size == 1){
    println("\tstab %d,x",off);
  }else if (ty->size == 2){
    println("\tstab %d,x",off+1);
    println("\tstaa %d,x",off);
  }else if (ty->size == 4){	// TY_LONG, TY_FLOAT
    println("\tldab @long+3");
    println("\tstab %d,x",off+3);
    println("\tldab @long+2");
    println("\tstab %d,x",off+2);
    println("\tldab @long+1");
    println("\tstab %d,x",off+1);
    println("\tldab @long");
    println("\tstab %d,x",off);
  }else
    println("  mov %%rax, (%%rdi)");
}

static void cmp_zero(Type *ty) {
  switch (ty->kind) {
  case TY_FLOAT:
    println("\tjsr __f32iszero");
//  println("  xorps %%xmm1, %%xmm1");
//  println("  ucomiss %%xmm1, %%xmm0");
    return;
  case TY_DOUBLE:
    assert(ty->kind!=TY_DOUBLE);
//  println("  xorpd %%xmm1, %%xmm1");
//  println("  ucomisd %%xmm1, %%xmm0");
    return;
  case TY_LDOUBLE:
    assert(ty->kind!=TY_LDOUBLE);
//  println("  fldz");
//  println("  fucomip");
//  println("  fstp %%st(0)");
    return;
  }

  if (is_integer(ty) && ty->size == 1){
    println("\ttstb");
  }else if (is_integer(ty) && ty->size <= 2){
    println("\taba");
    println("\tadca #0");
  }else{
    println("\tjsr __iszero32");
    IX_Dest = IX_None;
  }
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };

static int getTypeId(Type *ty) {
  switch (ty->kind) {
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
static char i8i32[]  = "jsr __s8to32";
static char i8u32[]  = "jsr __s8to32";
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

#define FROM_F80_1                                           \
  "fnstcw -10(%rsp); movzwl -10(%rsp), %eax; or $12, %ah; " \
  "mov %ax, -12(%rsp); fldcw -12(%rsp); "

#define FROM_F80_2 " -24(%rsp); fldcw -10(%rsp); "

// ex. i32i16: i32->i16
static char *cast_table[][11] = {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  NULL,   i8i32,  i16i64, NULL,  NULL,   i8u32,  i16i64, i16f32, i16f64, NULL}, // i8
  {NULL,  NULL,   i16i32, i16i64, NULL,  NULL,   i16u32, i16i64, i16f32, i16f64, NULL}, // i16
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, NULL}, // i32
  {i32i8, i32i16, i64i32, NULL,   i32u8, i32u16, i64u32, NULL,   i64f32, i64f64, NULL}, // i64

  {NULL,  NULL,   u8i32,  i16i64, NULL,  NULL,   u8u32,  i16i64, i16f32, i16f64, NULL}, // u8
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
    println("\tclrb");
    println("\tbra L_end_%d", c);
    println("L_false_%d:", c);
    println("\tldab #1");
    println("L_end_%d:", c);
//  println("  setne %%al");
//  println("  movzx %%al, %%eax");
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
  if (to->kind == TY_BOOL)
    return 0;
  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  return cast_table[t1][t2]==NULL;
}

#if 0
// Structs or unions equal or smaller than 16 bytes are passed
// using up to two registers.
//
// If the first 8 bytes contains only floating-point type members,
// they are passed in an XMM register. Otherwise, they are passed
// in a general-purpose register.
//
// If a struct/union is larger than 8 bytes, the same rule is
// applied to the the next 8 byte chunk.
//
// This function returns true if `ty` has only floating-point
// members in its byte range [lo, hi).
static bool has_flonum(Type *ty, int lo, int hi, int offset) {
  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
    for (Member *mem = ty->members; mem; mem = mem->next)
      if (!has_flonum(mem->ty, lo, hi, offset + mem->offset))
        return false;
    return true;
  }

  if (ty->kind == TY_ARRAY) {
    for (int i = 0; i < ty->array_len; i++)
      if (!has_flonum(ty->base, lo, hi, offset + ty->base->size * i))
        return false;
    return true;
  }

  return offset < lo || hi <= offset || ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE;
}

static bool has_flonum1(Type *ty) {
  return 0;
//return has_flonum(ty, 0, 8, 0);
}

static bool has_flonum2(Type *ty) {
  return 0;
//return has_flonum(ty, 8, 16, 0);
}
#endif

static void push_struct(Type *ty) {
  int sz = align_to(ty->size, 8);
//  int c = count();
  depth += sz;
  assert(sz <= 256);	// can't handle sz > 256
  tfr_dx();
  for (int i = ty->size-1; i >=0 ; i-- ){
    println("\tldab %d,x",i);
    println("\tpshb");
  }
//  println("  sub $%d, %%rsp", sz);
//  depth += sz / 8;

//  for (int i = 0; i < ty->size; i++) {
//    println("  mov %d(%%rax), %%r10b", i);
//    println("  mov %%r10b, %d(%%rsp)", i);
//  }
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

static void push_args2(Node *args, bool first_pass, Node *last_pushed_arg) {
  if (!args)
    return;
  push_args2(args->next, first_pass,last_pushed_arg);

  if ((first_pass && !args->pass_by_stack) || (!first_pass && args->pass_by_stack))
    return;

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
  case TY_CHAR: {
    gen_expr(args);
      if (args->pass_by_stack){
        push1();
        *last_pushed_arg = *args;
      }
    }
    break;
  case TY_FLOAT:
    if (args->pass_by_stack && args->kind==ND_NUM) {
      union { float f32; uint32_t u32; } u = { args->fval };
      println("; push float %e, %08x",u.f32,u.u32);
      gen_direct_pushl(u.u32);
      *last_pushed_arg = *args;
    }else{
      gen_expr(args);
      if (args->pass_by_stack){
        pushf();
        *last_pushed_arg = *args;
      }
    }
    break;
  case TY_LONG:
    if (!args->pass_by_stack){
        gen_expr(args);
	break;
    }
    int64_t val = args->val;
    switch(args->kind){
    case ND_CAST:
      if(!is_empty_cast(args->lhs->ty, args->ty)
      && args->lhs->kind != ND_NUM){
        gen_expr(args);
        pushl();
        *last_pushed_arg = *args;
	break;
      }
      val = args->lhs->val;
      // THRU
    case ND_NUM:
      gen_direct_pushl(val);
      *last_pushed_arg = *args;
      break;
    case ND_VAR:
      if (test_addr_x(args)){
        int off = gen_addr_x(args,false);
	println("\tldab %d,x",off+3);
	println("\tpshb");
	println("\tldab %d,x",off+2);
	println("\tpshb");
	println("\tldab %d,x",off+1);
	println("\tpshb");
	println("\tldab %d,x",off);
	println("\tpshb");
      }else{
        gen_expr(args);
        pushl();
      }
      *last_pushed_arg = *args;
      break;
    default:
      gen_expr(args);
      pushl();
      *last_pushed_arg = *args;
    }
    break;
  default: {
    gen_expr(args);
      if (args->pass_by_stack){
        push();
        *last_pushed_arg = *args;
      }
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
static int push_args(Node *node, Node *last_pushed_arg) {
  int stack = 0, gp = 0;

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (node->ret_buffer)  // && node->ty->size > 16)
    gp++;

  // Load as many arguments to the registers as possible.
  for (Node *arg = node->args; arg; arg = arg->next) {
    Type *ty = arg->ty;

    arg->pass_by_stack = false;
    switch (ty->kind) {
    case TY_STRUCT:
    case TY_UNION:
        arg->pass_by_stack = true; // Arguments bigger than 2 bytes are pushed onto the stack
        stack += ty->size;
	break;
    case TY_DOUBLE:
    case TY_LDOUBLE:
	error_tok(node->tok, "gen_expr: double not implemented yet");
	break;
    default: // TY_CHAR TY_INT TY_LONG TY_FLOAT TY_PTR
      if (gp++ >= 1) {
        arg->pass_by_stack = true;
        stack+=(ty->kind==TY_ARRAY)? 2: arg->ty->size;
      }else
	arg->pass_by_stack = false;
    }
  }
  push_args2(node->args, true,   last_pushed_arg);
  push_args2(node->args, false,  last_pushed_arg);

  // If the return type is a struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument (in Acc A,B).
  // MC6800: all struct/union passes the pointer
  println("; return type is struct/union, the caller passes a pointer");
  if (node->ret_buffer) { // && node->ty->size > 16)
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
  Obj *var = current_fn->params;

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
    IX_Dest = IX_None;
    println("\tstab %d,x",i);
  }
  println("\tldab @tmp3+1");
  println("\tldaa @tmp3");
}

static void builtin_alloca(void) {
  // Align size to 16 bytes.
  // MC6800 has no align, ignore it.
//  println("  add $15, %%rdi");
//  println("  and $0xfffffff0, %%edi");

  // Shift the temporary area by %rdi.
  // println("; %%di has alloca size");
  println("\tstab @rdi+1");		// The name of the work area will be decided later.
  println("\tstaa @rdi");
  // println(";	__alloca_bottom__ -> cx");
  // The area between alloca_bottom and SP is the stack currently in use. Move this.
  println("\tldab @bp+1	; IX =  (__alloca_bottom)");
  println("\tldaa @bp");
  println("\taddb #<%d",current_fn->alloca_bottom->offset);
  println("\tadca #>%d",current_fn->alloca_bottom->offset);
  tfr_dx();
  println("\tstx @tmp3 ; save address of __alloca_bottom__");
  println("\tsts @tmp2 ; save current SP");
  println("\tsts @tmp1 ; sp -= rdi");
  println("\tldab @tmp1+1");
  println("\tldaa @tmp1");
  println("\tsubb @rdi+1");
  println("\tsbca @rdi");
  println("\tstab @tmp1+1");
  println("\tstaa @tmp1");
  println("\tlds @tmp1 ; get new SP");
  println("\tldx 0,x");
  int c1 = count();
  int c2 = count();
  println("; copy stack working area to stack top");
  println("L_%d:",c1);
  println("\tcpx @tmp2");
  println("\tbeq L_%d",c2);
  println("\tldab 0,x");
  println("\tpshb");
  println("\tdex");
  println("\tbra L_%d",c1);
  println("L_%d:",c2);
  IX_Dest = IX_None;
  //println("  mov %d(%%rbp), %%rcx", current_fn->alloca_bottom->offset);
  //println("  sub %%rsp, %%rcx");
  //println("  mov %%rsp, %%rax");
  //println("  sub %%rdi, %%rsp");
  //println("  mov %%rsp, %%rdx");
  //println("1:");
  //println("  cmp $0, %%rcx");
  //println("  je 2f");
  //println("  mov (%%rax), %%r8b");
  //println("  mov %%r8b, (%%rdx)");
  //println("  inc %%rdx");
  //println("  inc %%rax");
  //println("  dec %%rcx");
  //println("  jmp 1b");
  //println("2:");

  // Move alloca_bottom pointer.
  println("\tldx @tmp3");
  IX_Dest = IX_None;
  println("\tldab 1,x	; make new __alloca_bottom__");
  println("\tldaa 0,x");
  println("\tsubb @rdi+1");
  println("\tsbca @rdi");
  println("\tstab 1,x");
  println("\tstaa 0,x");
  //println("  mov %d(%%rbp), %%rax", current_fn->alloca_bottom->offset);
  //println("  sub %%rdi, %%rax");
  //println("  mov %%rax, %d(%%rbp)", current_fn->alloca_bottom->offset);
}

static bool is_compare(Node *node)
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

//
// If rhs is a simple expression, it is computed directly without pushing it onto the stack.
//
static int gen_direct_sub(Node *rhs,char *opb, char *opa, int test)
{
  switch(rhs->kind){
  case ND_NUM: {
    switch (rhs->ty->kind) {
    case TY_CHAR:		// TODO: Avoid unnecessary type promotion
    case TY_SHORT:
    case TY_INT:
    case TY_PTR:
      if (test) return 1;
      if (rhs->val==0) {
        if (strcmp(opb,"addb")==0) {	// subb used for compare, so addb only
          return 1;
        }
      }
      println("\t%s #<%u", opb, (uint16_t)rhs->val);
      println("\t%s #>%u", opa, (uint16_t)rhs->val);
      return 1;
    default:
      return 0;
    }
  } // ND_NUM
  case ND_VAR: {
    if (rhs->var->ty->kind != TY_VLA ){
      if (!test_addr_x(rhs)) return 0;
      if(rhs->var->is_local){
	if (rhs->ty->kind==TY_ARRAY) {
	  return 0;
	}
        int off = gen_addr_x(rhs,true);
        if (rhs->ty->kind==TY_CHAR){
          if (test) {
            return rhs->ty->is_unsigned;
	  }
          println("\t%s %d,x",opb,off);
          println("\t%s #0",opa);
        }else{
          if (test) return 1;
          println("\t%s %d,x",opb,off+1);
          println("\t%s %d,x",opa,off);
        }
        return 1;
      }else{
        // global
        if (rhs->ty->kind==TY_CHAR){
	  return 0;
        }else{
          if (test) return 1;
	  int off = gen_addr_x(rhs,true);
          println("\t%s %d,x",opb,off+1);
          println("\t%s %d,x",opa,off);
//          println("\t%s #<_%s",opb,rhs->var->name);
//          println("\t%s #>_%s",opa,rhs->var->name);
        }
        return 1;
      }
    }
    return 0;
  } // ND_VAR
  case ND_CAST:
    if(is_empty_cast(rhs->lhs->ty, rhs->ty))
      return gen_direct_sub(rhs->lhs, opb, opa, test);
  default:
    return 0;
  }
  return 0;
}

static int can_direct(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,1);	// test mode

  return r;
}

static int gen_direct(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0);
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
int gen_direct_shl_long(Node *node)
{
  Node *rhs = node->rhs;

  if (node->kind != ND_SHL || rhs->kind != ND_NUM)
    return 0;
  if (node->rhs->ty->kind!=TY_LONG && node->rhs->ty->kind!=TY_INT)
    return 0;

  if ( rhs->val >= 32 ) {
    println("\tldx #0");
    println("\tstx @long+2");
    println("\tstx @long");
    IX_Dest = IX_None;
    return 1;
  }
  switch (rhs->val) {
  case 24:
  case 16:
  case 8:
  }
  switch (rhs->val) {
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
    println("\tldx @long");
    println("\tldab @long+3");
    println("\tstab @long+2");
    println("\tclr @long+3");
    IX_Dest = IX_None;
    return 1;
  }
  return 0;
}

int gen_direct_shr_long(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  if (node->kind != ND_SHR || rhs->kind != ND_NUM)
    return 0;
  if (node->rhs->ty->kind!=TY_LONG && node->rhs->ty->kind!=TY_INT)
    return 0;

  if ( rhs->val >= 32 ) {
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
  switch (rhs->val) {
  case 24:
  case 16:
  case 8:
    println("\tclra");
    if (!lhs->ty->is_unsigned) {
      int c = count();
      println("\ttst @long");
      println("\tbpl L_%d",c);
      println("\tsbca #0");
      println("L_%d:",c);
    }
  }
  switch (rhs->val) {
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
  switch(rhs->kind){
  case ND_NUM: {
    switch (rhs->ty->kind) {
    case TY_LONG:
      if (test) return 1;
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

  int L =  ((lhs->kind == ND_NUM) && (lhs->ty->kind == TY_LONG))
        || ((lhs->kind == ND_VAR) && test_addr_x(lhs) &&  lhs->var->is_local);
  int R =  ((rhs->kind == ND_NUM) && (rhs->ty->kind == TY_LONG))
        || ((rhs->kind == ND_VAR) && test_addr_x(rhs) &&  rhs->var->is_local);
  if (!L || !R){
    return 0;
  }
  return 1;
}

//
// Evaluate condition, if false then jumps to the if_false
//   if can't return 0
//
static int gen_jump_if_false(Node *node,char *if_false)
{
  if(!is_compare(node))
    return 0;
  if(node->lhs->ty->kind!=TY_CHAR && node->lhs->ty->kind!=TY_INT && node->lhs->ty->kind!=TY_SHORT)
    return 0;
  char if_thru[32];
  int c = count();
  sprintf(if_thru,"L_thru_%d",c);
  if (can_direct(node->rhs)){
    gen_expr(node->lhs);
    if(node->rhs->kind == ND_NUM && node->rhs->val == 0){
      if(node->kind != ND_EQ && node->kind != ND_NE)
        println("\ttsta");
    }else if(!gen_direct(node->rhs,"subb","sbca")){
      assert(0);
    }
  }else{
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
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
    println("\tjne %s",if_false);
    break;
  case ND_NE:
    println("\taba");
    println("\tadca #0");
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
    if (node->lhs->ty->is_unsigned){
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
// Evaluate condition, if false then jumps to the specified label.
//   if can't return 0
//
static int gen_jump_if_true(Node *node,char *if_true)
{
 return 0; // XXX : not yet implemented
}


// Generate code for a given node.
static void gen_expr(Node *node) {
  node = optimize_expr(node);

  switch (node->kind) {
  case ND_NULL_EXPR:
    return;
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT: {
      union { float f32; uint32_t u32; } u = { node->fval };
      println("\tjsr __load32i	; float %e",u.f32);
      println("\t.word $%04x",(uint16_t)(u.u32>>16));
      println("\t.word $%04x",(uint16_t)(u.u32&0x0ffff));
      IX_Dest = IX_None;
//    println("  mov $%u, %%eax  # float %Lf", u.u32, node->fval);
//    println("  movq %%rax, %%xmm0");
      return;
    }
    case TY_DOUBLE:
    case TY_LDOUBLE: {
      error_tok(node->tok, "gen_expr: double not implemented yet");
      return;
    }
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
      if((uint16_t)node->val==0){
        println("\tclrb");
        println("\tclra");
      }else if ((node->val&0x00ff)==0){
        println("\tclrb");
        println("\tldaa #>%u", (uint16_t)node->val);
      }else if ((node->val&0xff00)==0){
        println("\tldab #<%u", (uint16_t)node->val);
        println("\tclra");
      }else if (((node->val&0xff00)>>8)==(node->val&0x00ff)){
        println("\tldab #<%u", (uint16_t)node->val);
        println("\ttba");
      }else{
        println("\tldab #<%u", (uint16_t)node->val);
        println("\tldaa #>%u", (uint16_t)node->val);
      }
      return;
    case TY_LONG:
      println("\tldx #%u",(uint16_t)((node->val>>16)&0x0ffff));
      println("\tstx @long");
      println("\tldx #%u",(uint16_t)(node->val&0x0ffff));
      println("\tstx @long+2");
      IX_Dest = IX_None;
//    println("\tjsr __load32i		; load 32bit immediate #%lu",node->val);
//    println("\t.word %u",(uint16_t)((node->val>>16)&0x0ffff));
//    println("\t.word %u",(uint16_t)(node->val&0x0ffff));
      return;
    }
    error_tok(node->tok, "gen_expr: not implemented yet token");
    return;
  }
  case ND_POST_INCDEC: {
    if (node->rhs->kind != ND_NUM){
      fprintf(stderr,"ND_POST_INCDEC: assertion failed 1\n");
      assert(0);
    }
    int val = node->rhs->val;
    int off;
    if (test_addr_x(node->lhs)){
      off = gen_addr_x(node->lhs,false);
    }else{
      off = 0;
      gen_addr(node->lhs);
      println("\tpshb");
      println("\tpsha");
      println("\ttsx");
      println("\tldx 0,x");
      println("\tins");
      println("\tins");
    }
    switch (node->lhs->ty->kind) {
    case TY_CHAR:
      println("\tldab %d,x",off);
      println("\taddb #%d",val);
      println("\tstab %d,x",off);
      println("\tsubb #%d",val);
      println("\tclra");
      if (!node->lhs->ty->is_unsigned){
        println("\tasrb");
        println("\trolb");
        println("\tsbca #0");
      }
      break;
    case TY_SHORT:
    case TY_INT:
      println("\tldab %d,x",off+1);
      println("\tldaa %d,x",off);
      println("\taddb #<%d",val);
      println("\tadca #>%d",val);
      println("\tstab %d,x",off+1);
      println("\tstaa %d,x",off);
      println("\tsubb #<%d",val);
      println("\tsbca #>%d",val);
      break;
    default:
      fprintf(stderr,"ND_POST_INCDEC: assertion failed 1\n");
      assert(0);
    }
    return;
  }
  case ND_NEG:
    gen_expr(node->lhs);

    switch (node->ty->kind) {
    case TY_FLOAT:
      println("\tldab @long	; negate float");
      println("\teorb #$80");
      println("\tstab @long");
//      println("  mov $1, %%rax");
//      println("  shl $31, %%rax");
//      println("  movq %%rax, %%xmm1");
//      println("  xorps %%xmm1, %%xmm0");
      return;
    case TY_DOUBLE:
      error_tok(node->tok, "gen_expr: double not implemented yet");
//      println("  mov $1, %%rax");
//      println("  shl $63, %%rax");
//      println("  movq %%rax, %%xmm1");
//      println("  xorpd %%xmm1, %%xmm0");
      return;
    case TY_LDOUBLE:
      error_tok(node->tok, "gen_expr: double not implemented yet");
//      println("  fchs");
      return;
    case TY_LONG:
      println("\tjsr __neg32");
      IX_Dest = IX_None;
      return;
    case TY_CHAR:
      println("\tnega");
      return;
    }
    println("\tnega");
    println("\tnegb");
    println("\tsbca #0");
//  println("  neg %%rax");
    return;
  //   ND_NEG end
  case ND_VAR:
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
#if 1
    int off = gen_addr_x(node,false);
    load_x(node->ty,off);
#else
    gen_addr(node);
    load(node->ty);
#endif
    return;
  case ND_MEMBER: {
    gen_addr(node);
    load(node->ty);

    Member *mem = node->member;
    if (mem->is_bitfield) {
      println("; bitfield mem->bit_width=%d, mem->bit_offset=%d, %s %d",
		      mem->bit_width, mem->bit_offset, __FILE__, __LINE__);
      println(";  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);
      for (int i=0; i<mem->bit_width + mem->bit_offset; i++){
	println("\taslb");
	println("\trora");
      }
//    println("  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);
      for (int i=0; i<mem->bit_width; i++) {
        if (mem->ty->is_unsigned){
	  println("\tlsra");
	  println("\trolb");
        }else{
	  println("\tasra");
	  println("\trolb");
	} 
      }
//    if (mem->ty->is_unsigned)
//      println("  shr $%d, %%rax", 64 - mem->bit_width);
//    else
//      println("  sar $%d, %%rax", 64 - mem->bit_width);
    }
    return;
  }
  case ND_DEREF:
    if (test_addr_x(node->lhs)){
      int off = gen_addr_x(node->lhs,false);
      println("\tldx %d,x",off);
      IX_Dest = IX_None;
      load_x(node->ty,0);
      return;
    }
    gen_expr(node->lhs);
    load(node->ty);
    return;
  case ND_ADDR:
    gen_addr(node->lhs);
    return;
  case ND_ASSIGN:
    if (test_addr_x(node->lhs)){ 
      gen_expr(node->rhs);
      int off = gen_addr_x(node->lhs,true);
      println("; ND_ASSIGN: test_addr_x(lhs) OK, store_x off=%d",off);
      store_x(node->ty,off);
      return;
    }
    gen_addr(node->lhs);
#if 0
    if (node->rhs->kind == ND_NUM) {
      if (node->rhs->val == 0) {
	 // TODO: Multiple assignments may be strange
         println("; ND_ASSIGN (char or int) = 0");
         switch(node->rhs->ty->kind){
         case TY_CHAR:
         case TY_SHORT:
         case TY_INT:
         case TY_LONG:
           tfr_dx();
	   if (node->rhs->ty->kind!=TY_CHAR) {
	     if (node->rhs->ty->kind==TY_LONG) {
	       println("\tclr 3,x");
	       println("\tclr 2,x");
	     }
	     println("\tclr 1,x");		// TY_LONG, TY_INT
	   }
	   println("\tclr 0,x");
	   IX_Dest = IX_None;
	   return;
	 }
	 // Complex types will be dealt with later
       }else{
	 // TODO: Multiple assignments may be strange
         println("; ND_ASSIGN (char or int) = const");
         tfr_dx();
	 switch(node->rhs->ty->kind){
	 case TY_CHAR:
	   println("\tldab #<%d",(int)node->rhs->val);
	   println("\tstab 0,x");
	   IX_Dest = IX_None;
	   return;
	 case TY_INT:
	   println("\tldab #<%d",(int)node->rhs->val);
	   println("\tldaa #>%d",(int)node->rhs->val);
	   println("\tstab 1,x");
	   println("\tstaa 0,x");
	   IX_Dest = IX_None;
	   return;
	 case TY_FLOAT:
	 case TY_LONG:
	   println("\tldab #%d",(int)(node->rhs->val & 0x00ff0000)>>16);
	   println("\tldaa #%d",(int)(node->rhs->val & 0xff000000)>>24);
	   println("\tstab 1,x");
	   println("\tstaa 0,x");
	   println("\tldab #%d",(int)(node->rhs->val & 0x000000ff));
	   println("\tldaa #%d",(int)(node->rhs->val & 0x0000ff00)>>8);
	   println("\tstab 3,x");
	   println("\tstaa 2,x");
	   IX_Dest = IX_None;
	   return;
	 }
      }
      // Complex types will be dealt with later
    }
#endif
    push();
    gen_expr(node->rhs);

    if (node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield) {
      println("\tstab @tmp1+1");
      println("\tstab @tmp1");
//    println("  mov %%rax, %%r8");

      // If the lhs is a bitfield, we need to read the current value
      // from memory and merge it with a new value.
      Member *mem = node->lhs->member;
      println("; bitfieled mem->bit_width=%d, mem->bit_offset=%d, %s %d",
		      mem->bit_width, mem->bit_offset, __FILE__, __LINE__);
      println("\tandb #<%d", (int)(1L << mem->bit_width) - 1);
      println("\tanda #>%d", (int)(1L << mem->bit_width) - 1);
      for (int i=0; i<mem->bit_offset; i++){
	println("\taslb");
	println("\trola");
      }
      println("\tstab @rdi+1");
      println("\tstaa @rdi");
//    println("  mov %%rax, %%rdi");
//    println("  and $%ld, %%rdi", (1L << mem->bit_width) - 1);
//    println("  shl $%d, %%rdi", mem->bit_offset);
      println("\tpula");
      println("\tpulb");
      println("\tpshb");
      println("\tpsha");
//    depth -= 2;
//    println("  mov (%%rsp), %%rax");
      load(mem->ty);

      long mask = ((1L << mem->bit_width) - 1) << mem->bit_offset;
      println("\tandb #<%d",(int)~mask);
      println("\tanda #>%d",(int)~mask);
      println("\torab @rdi+1");
      println("\toraa @rdi+1");
//    println("  mov $%ld, %%r9", ~mask);
//    println("  and %%r9, %%rax");
//    println("  or %%rdi, %%rax");
      store(node->ty);
      println("\tldab @tmp1+1");
      println("\tldaa @tmp1");
//    println("  mov %%r8, %%rax");
      return;
    }

    store(node->ty);
    println("; ND_ASSIGN end %s %d",__FILE__,__LINE__);
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
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
    return;
  case ND_MEMZERO:
    // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
    println("; ND_MEMZERO %.*s size=%d, offset=%d,  %s %d",
		    node->var->ty->name->len, node->var->ty->name->loc,
		    node->var->ty->size, node->var->offset, __FILE__, __LINE__);
    ldx_bp();
    if(node->var->ty->size <= 4
    && node->var->ty->size + node->var->offset <= 256){
      for (int i=0; i<node->var->ty->size; i++){
        println("\tclr %d,x",node->var->offset+i);
      }
    }else{
      println("\tldab #%d",node->var->ty->size);
      println("\tclra");
      int c = count();
      println("L_memzero_%d:", c);
      println("\tstaa %d,x",node->var->offset);
      println("\tinx");
      println("\tdecb");
      println("\tbne L_memzero_%d", c);
      break;
    }
    IX_Dest = IX_None;
    return;
  case ND_COND: {
    int c = count();
    gen_expr(node->cond);
    if (!is_compare(node->cond))
      cmp_zero(node->cond->ty);
    println("\tjeq L_else_%d", c);
    gen_expr(node->then);
    println("\tjmp L_end_%d", c);
    println("L_else_%d:", c);
    IX_Dest = IX_None;
    gen_expr(node->els);
    println("L_end_%d:", c);
    IX_Dest = IX_None;
    return;
  }
  case ND_NOT: {
    gen_expr(node->lhs);
    cmp_zero(node->lhs->ty);
    int c = count();
    println("\tbeq L_false_%d", c);
    println("\tclrb");
    println("\tbra L_end_%d", c);
    println("L_false_%d:", c);
    println("\tldab #1");
    println("L_end_%d:", c);
    println("\tclra");
//  println("  sete %%al");
//  println("  movzx %%al, %%rax");
    return;
  }
  case ND_BITNOT:
    gen_expr(node->lhs);
    println("\tcomb");
    println("\tcoma");
//  println("  not %%rax");
    return;
  case ND_LOGAND: {
    int c = count();
    char L_false[32];
    char L_end[32];
    sprintf(L_false,"L_false_%d",c);
    sprintf(L_end,  "L_end_%d",c);
    if (!gen_jump_if_false(node->lhs,L_false)){
      gen_expr(node->lhs);
      if (!is_compare(node->lhs))
        cmp_zero(node->lhs->ty);
      println("\tjeq %s",L_false);
    }
    if (!gen_jump_if_false(node->rhs,L_false)){
      gen_expr(node->rhs);
      if (!is_compare(node->rhs))
        cmp_zero(node->rhs->ty);
      println("\tbeq %s",L_false);
    }
    println("\tldab #1");
    println("\tbra %s",L_end);
    println("%s:",L_false);
    println("\tclrb");
    println("L_end_%d:", c);
    println("\tclra");
    IX_Dest = IX_None;
    return;
  }
  case ND_LOGOR: {
    int c = count();
    gen_expr(node->lhs);
    cmp_zero(node->lhs->ty);
    println("\tjne L_true_%d", c);
//  println("  jne .L.true.%d", c);
    gen_expr(node->rhs);
    cmp_zero(node->rhs->ty);
    println("\tjne L_true_%d", c);
    println("\tclrb");
    println("\tbra L_end_%d", c);
    println("\tclrb");
    println("L_true_%d:", c);
    println("\tldab #1");
    println("L_end_%d:", c);
    println("\tclra");
    IX_Dest = IX_None;
//  println("  jne .L.true.%d", c);
//  println("  mov $0, %%rax");
//  println("  jmp .L.end.%d", c);
//  println(".L.true.%d:", c);
//  println("  mov $1, %%rax");
//  println(".L.end.%d:", c);
    return;
  }
  case ND_FUNCALL: {
    if (node->lhs->kind == ND_VAR && !strcmp(node->lhs->var->name, "alloca")) {
      gen_expr(node->args);
      println("; ND_FUNCALL %s %d",__FILE__,__LINE__);
      println("; call builtin_alloca()");
      //println("  mov %%rax, %%rdi");
      builtin_alloca();
      return;
    }

    Node *last_pushed_arg = calloc(1,sizeof(Node));
    int stack_args = push_args(node, last_pushed_arg);
    // if passed-by-register argument, last_pushed_arg!=NULL
    if (node->lhs->kind == ND_VAR && node->lhs->ty->kind == TY_FUNC){
      println("\tjsr _%s",node->lhs->var->name);
    }else{
      int off = gen_addr_x(node->lhs,false);
      // If the return type is a large struct/union, the caller passes
      // a pointer to a buffer as if it were the first argument.
      println("\tjsr %d,x",off);
    }
    IX_Dest = IX_None;
  
    // Removes pushed arguments before calling a function
    if (stack_args*4 > 34) {
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
    }else{
      while(stack_args--) {
        println("\tins");
        depth--;
      }
    }
    // If the return value is a type shorter than an int,
    // the upper bytes contain garbage, so we correct it.
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

    // If the return type is a small struct, a value is returned
    // using up to two registers.
    // MC6800: all struct/union not passed by register, so ignore it.
#if 0
    if (node->ret_buffer && node->ty->size <= 16) {
      copy_ret_buffer(node->ret_buffer);
      println("  lea %d(%%rbp), %%rax", node->ret_buffer->offset);
    }
#endif

    return;
  } // ND_FUNCALL
  case ND_LABEL_VAL:
    println("\tldab #<%s",node->unique_label);
    println("\tldaa #>%s",node->unique_label);
//  println("  lea %s(%%rip), %%rax", node->unique_label);
    return;
#if	0
  case ND_CAS:
  case ND_EXCH:
    assert(0);
#endif
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
      println("\tjsr __addf32tos	; @long += TOS");
//    println("  add%s %%xmm1, %%xmm0", sz);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_SUB:
      println("\tjsr __subf32tos	; @long -= TOS");
//    println("  sub%s %%xmm1, %%xmm0", sz);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MUL:
      println("\tjsr __mulf32tos	; @long *= TOS");
//    println("  mul%s %%xmm1, %%xmm0", sz);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_DIV:
      println("\tjsr __divf32tos	; @long /= TOS");
//    println("  div%s %%xmm1, %%xmm0", sz);
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
      println("\tjsr __cmpf32tos	; @long cmp  TOS");
      println("\tbcc %s",L_cmpf1);	// when carry=1, compare NaN
      if (node->kind == ND_NE) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      println("\tclra");
      println("\tbra %s",L_cmpf2);
      println("%s:",L_cmpf1);
      if (node->kind == ND_EQ) {
        println("; ND_EQ");
	println("\tclra");
        println("\tsubb #1");		// 00:carry, other NC
	println("\trolb");
      } else if (node->kind == ND_NE) {
	println("; float ND_NE");	// EQ:AccB==0, other FF or 01
      } else if (node->kind == ND_LT) {	// AccB:FF true, other false
        println("; ND_LT");
        println("\tclra");
        println("\taddb #1");		// FF:Carry, other:NC
	println("\trolb");
      } else if (node->kind == ND_GT) {	// AccB:01 true, other false
        println("; ND_GT");
        println("\tclra");
        println("\tdecb");
        println("\tsubb #1");		// 01:carry, other NC
	println("\trolb");
      } else if (node->kind == ND_LE) { // AccB:FF,00 true, other false
        println("; ND_LE");
	println("\tdecb");		// AccB:FE,FF tue, other false
	println("\taddb #2");		// AccB:FE,FF Carry, other NC
	println("\trolb");
      } else if (node->kind == ND_GE) { // AccB:00,01 true, other false
        println("; ND_LE");
	println("\tsubb #2");		// AccB:00,01 Carry, other NC
	println("\trolb");
      } else {
        error_tok(node->tok, "invalid expression");
      }
      println("\tclra");
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
      println("\tjsr __add32tos	; @long += TOS, pull TOS");
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
      println("\tjsr __sub32tos  ; @long = TOS - @long, pull TOS");
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MUL:
      gen_expr(node->lhs);
      pushl();
      gen_expr(node->rhs);
      println("\tjsr __mul32tos ; @long *= TOS, pull TOS");
//  println("  imul %s, %s", di, ax);
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_DIV:
      gen_expr(node->rhs);
      pushl();
      gen_expr(node->lhs);
      if (node->ty->is_unsigned) {
        println("\tjsr __div32x32u ; @long /= TOS, pull TOS");
      }else{
        println("\tjsr __div32x32s ; @long /= TOS, pull TOS");
      }
      depth -= 4;
      IX_Dest = IX_None;
      return;
    case ND_MOD:
      gen_expr(node->rhs);
      pushl();
      gen_expr(node->lhs);
      if (node->ty->is_unsigned) {
        println("\tjsr __rem32x32u ; @long %%= TOS, pull TOS");
      }else{
        println("\tjsr __rem32x32s ; @long %%= TOS, pull TOS");
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
	println("\tldab %d,x",off+3);
	println("\tpshb");
	println("\tldab %d,x",off+2);
	println("\tpshb");
	println("\tldab %d,x",off+1);
	println("\tpshb");
	println("\tldab %d,x",off);
	println("\tpshb");
	depth+=4;
      }else{
        gen_expr(node->rhs);
        pushl();
      }
      gen_expr(node->lhs);
      // Since push in the order is rhs -> lhs, the conditions are reversed.
      // Should I change the name?
      if (node->kind == ND_EQ) {
        println("\tjsr __eq32");
      } else if (node->kind == ND_NE) {
        println("\tjsr __ne32");
      } else if (node->kind == ND_LT) {
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __gt32u");
        else
          println("\tjsr __gt32s");
      } else if (node->kind == ND_LE) {
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __ge32u");
        else
          println("\tjsr __ge32s");
      } else if (node->kind == ND_GT) {
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __lt32u");
        else
          println("\tjsr __lt32s");
      } else if (node->kind == ND_GE) {
        if (node->lhs->ty->is_unsigned)
          println("\tjsr __le32u");
        else
          println("\tjsr __le32s");
      }
//    println("  movzb %%al, %%rax");
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
      if (node->rhs->kind == ND_NUM
      && (node->rhs->ty->kind==TY_LONG || node->rhs->ty->kind==TY_INT) ){
        gen_expr(node->lhs);
  	if (node->kind == ND_SHL && gen_direct_shl_long(node))
	   return;
	if (node->kind == ND_SHR && gen_direct_shr_long(node))
	   return;
	println("\tldab #%d",(uint32_t)(node->rhs->val & 0x000000FF));
      }else{
        gen_expr(node->rhs);
        cast(node->rhs->ty, ty_char);
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
  case ND_ADD:
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);
    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if (gen_direct(node->rhs,"addb","adca")){
        return;
      }
      assert(0);
    }
    if (can_direct(node->lhs)){
      gen_expr(node->rhs);
      if (gen_direct(node->lhs,"addb","adca")){
        return;
      }
      assert(0);
    }
    gen_expr(node->lhs);
    if (node->rhs->kind     == ND_CAST
    &&  node->rhs->ty->kind == TY_ARRAY
    &&  node->rhs->lhs->kind == ND_VAR
    &&  node->rhs->lhs->var->is_local   ) {
      println("\taddb @bp+1");
      println("\tadca @bp");
      if (node->rhs->lhs->var->offset) {
        println("\taddb #<%d",node->rhs->lhs->var->offset);
        println("\tadca #>%d",node->rhs->lhs->var->offset);
      }
      return;
    }
    push();
    gen_expr(node->rhs);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\taddb 1,x");
    println("\tadca 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
    IX_Dest = IX_None;
    return;
  case ND_SUB:
    if (can_direct(node->rhs)){
      gen_expr(node->lhs);
      if(gen_direct(node->rhs,"subb","sbca"))
        return;
      assert(0);
    }
    gen_expr(node->rhs);		// TODO: lhs to rhs
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
//    println("  sub %s, %s", di, ax);
    println("\ttsx");
    println("\tsubb 1,x");
    println("\tsbca 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
    IX_Dest = IX_None;
    return;
  case ND_MUL:
    if (node->lhs->ty ==  node->ty){
      switch(node->rhs->kind){
      case ND_NUM:
        switch (node->rhs->ty->kind) {
        case TY_INT:
        case TY_SHORT:
	  switch(node->rhs->val){
	  case 2:
            gen_expr(node->lhs);
            cast(node->lhs->ty, node->ty);
	    println("\taslb");
	    println("\trola");
	    return;
	  case 3:
            gen_expr(node->lhs);
            cast(node->lhs->ty, node->ty);
	    println("\tstab @tmp1+1");
	    println("\tstaa @tmp1");
	    println("\taslb");
	    println("\trola");
	    println("\taddb @tmp1+1");
	    println("\tadca @tmp1");
	    return;
	  case 4:
            gen_expr(node->lhs);
            cast(node->lhs->ty, node->ty);
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    return;
	  case 8:
            gen_expr(node->lhs);
            cast(node->lhs->ty, node->ty);
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    return;
	  case 16:
            gen_expr(node->lhs);
            cast(node->lhs->ty, node->ty);
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    println("\taslb");
	    println("\trola");
	    return;
          }
        }
	break;
      }
    }
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
    cast(node->lhs->ty, node->ty);
//  println("  imul %s, %s", di, ax);
    println("\tjsr __mul16x16");
    println("\tins");
    println("\tins");
    depth -= 2;
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
    cast(node->lhs->ty, node->ty);
    if (node->ty->is_unsigned) {
      println("\tjsr __div16x16u");
    }else{
      println("\tjsr __div16x16s");
    }
    println("\tins");
    println("\tins");
    depth -= 2;
    IX_Dest = IX_None;
    return;
  case ND_MOD:
    gen_expr(node->rhs);
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
    if (node->ty->is_unsigned) {
      println("\tjsr __rem16x16u");
    }else{
      println("\tjsr __rem16x16s");
    }
    println("\tins");
    println("\tins");
    depth -= 2;
    IX_Dest = IX_None;
    return;
  case ND_BITAND:
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
//    println("  and %s, %s", di, ax);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\tandb 1,x");
    println("\tanda 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
    return;
  case ND_BITOR:
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
//    println("  or %s, %s", di, ax);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\torab 1,x");
    println("\toraa 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
    return;
  case ND_BITXOR:
    gen_expr(node->lhs);
    push();
    gen_expr(node->rhs);
//    println("  xor %s, %s", di, ax);
    println("\ttsx");
    IX_Dest = IX_None;
    println("\teorb 1,x");
    println("\teora 0,x");
    println("\tins");
    println("\tins");
    depth -= 2;
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
      println("\tins");
      println("\tins");
      depth -= 2;
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
//    println("  movzb %%al, %%rax");
    return;
  case ND_SHL:
    gen_expr(node->rhs);
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
    println("\tjsr __shl16");
    println("\tins");
    println("\tins");
    IX_Dest = IX_None;
    depth -= 2;
//    println("  mov %%rdi, %%rcx");
//    println("  shl %%cl, %s", ax);
    return;
  case ND_SHR:
    gen_expr(node->rhs);
    cast(node->rhs->ty, node->ty);
    push();
    gen_expr(node->lhs);
    cast(node->lhs->ty, node->ty);
//    println("  mov %%rdi, %%rcx");
    if (node->lhs->ty->is_unsigned){
      println("\tjsr __shr16u");
      println("\tins");
      println("\tins");
//    println("  shr %%cl, %s", ax);
    }else{
      println("\tjsr __shr16s");
      println("\tins");
      println("\tins");
//      println("  sar %%cl, %s", ax);
    }
    depth -= 2;
    IX_Dest = IX_None;
    return;
  }

  error_tok(node->tok, "invalid expression");
}


static void gen_stmt(Node *node) {
  char s[1024];
  char *p = node->loc;
  char *q = s;
  while(*p && *p!='\r' && *p!='\n'){
    *q++ = *p++;
  }
  *q = '\0';
  if (strcmp(s,";") && s[0]){
    println("; gen_stmt: %s",s);
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
    node->cond = optimize_expr(node->cond);
    if (!gen_jump_if_false(node->cond,L_else)){
      gen_expr(node->cond);
      if (!is_compare(node->cond))
        cmp_zero(node->cond->ty);
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
    int c = count();
    if (node->init)
      gen_stmt(node->init);
    println("L_begin_%d:", c);
    IX_Dest = IX_None;
    node->cond = optimize_expr(node->cond);
    if (node->cond) {
      if (!gen_jump_if_false(node->cond,node->brk_label)){
        gen_expr(node->cond);
        if (!is_compare(node->cond))
          cmp_zero(node->cond->ty);
        println("\tjeq %s", node->brk_label);
      }
    }
    gen_stmt(node->then);
    println("%s:", node->cont_label);
    IX_Dest = IX_None;
    if (node->inc)
      gen_expr(node->inc);
    println("\tjmp L_begin_%d", c);
    println("%s:", node->brk_label);
    IX_Dest = IX_None;
    return;
  }
  case ND_DO: {
    int c = count();
    println("L_begin_%d:", c);
    IX_Dest = IX_None;
    gen_stmt(node->then);
    println("%s:", node->cont_label);
    node->cond = optimize_expr(node->cond);
    gen_expr(node->cond);
    if (!is_compare(node->cond))
      cmp_zero(node->cond->ty);
    println("\tjne L_begin_%d", c);
    println("%s:", node->brk_label);
    IX_Dest = IX_None;
    return;
  }
  case ND_SWITCH:
    gen_expr(node->cond);

    for (Node *n = node->case_next; n; n = n->case_next) {
      char *ax = (node->cond->ty->size == 8) ? "%rax" : "%eax";
      char *di = (node->cond->ty->size == 8) ? "%rdi" : "%edi";

      // TODO: 32bit case
      if (n->begin == n->end) {
        int c = count();
	println("\tcmpb #<%ld",n->begin);
	println("\tbne L_case_%d",c);
	println("\tcmpa #>%ld",n->begin);
	println("\tjeq %s", n->label);
	println("L_case_%d:",c);
        IX_Dest = IX_None;
//        println("  cmp $%ld, %s", n->begin, ax);
//        println("  je %s", n->label);
        continue;
      }

      // [GNU] Case ranges
      println("; TODO: Case ranges");
      println("  mov %s, %s", ax, di);
      println("  sub $%ld, %s", n->begin, di);
      println("  cmp $%ld, %s", n->end - n->begin, di);
      println("  jbe %s", n->label);
    }

    if (node->default_case)
      println("\tjmp %s", node->default_case->label);

    println("\tjmp %s", node->brk_label);
    gen_stmt(node->then);
    println("%s:", node->brk_label);
    return;
  case ND_CASE:
    println("%s:", node->label);
    gen_stmt(node->lhs);
    return;
  case ND_BLOCK:
    for (Node *n = node->body; n; n = n->next)
      gen_stmt(n);
    return;
  case ND_GOTO:
    println("\tjmp %s", node->unique_label);
    return;
  case ND_GOTO_EXPR:
    gen_expr(node->lhs);
    println("\tpshb	; jmp [AccD]");
    println("\tpsha");
    println("\trts");
    return;
  case ND_LABEL:
    println("%s:", node->unique_label);
    gen_stmt(node->lhs);
    return;
  case ND_RETURN:
    if (node->lhs) {
      gen_expr(node->lhs);
      Type *ty = node->lhs->ty;
      switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
        println("; copy_struct_mem begin");
        copy_struct_mem();
        println("; copy_struct_mem end");
        break;
      }
    }

    println("\tjmp L_return_%d", current_fn->function_no);
//    println("\tjmp L_return_%s", current_fn->name);
    return;
  case ND_EXPR_STMT:
    gen_expr(node->lhs);
    return;
  case ND_ASM:
    println("  %s", node->asm_str);
    return;
  }

  error_tok(node->tok, "invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) {
    if (!fn->is_function)
      continue;

    // If a function has many parameters, some parameters are
    // inevitably passed by stack rather than by register.
    // The first passed-by-stack parameter resides at RBP+16.
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

    // ローカル変数領域の大きさを計算
    // Assign offsets to locals
    int ret_skipped = 0;
    for (Obj *var = fn->locals; var; var = var->next) { //引数もここ
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
//	default:assert(var->ty->size < 8);	// XXX
//		break;
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
      if (var->is_tls)
        println("\t.section .tdata,\"awT\",@progbits");
      else if (var->is_literal)
        println("\t.literal");
      else
        println("\t.data");

      println("_%s:", var->name);

      Relocation *rel = var->rel;
      int pos = 0;
      while (pos < var->ty->size) {
        if (rel && rel->offset == pos) {
          println("\t.word %s%+ld", *rel->label, rel->addend);
          rel = rel->next;
          pos += 8;
        } else {
	  if (isalnum(var->init_data[pos])){
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
    if (var->is_tls)
      println("\t.section .tbss,\"awT\",@nobits");
    else
      println("\t.bss");

    println("_%s:", var->name);
    for (int i=0; i<var->ty->size; i++){
      println("\t.byte 0");
    }
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
      IX_Dest = IX_None;
      continue;
    }
    // Prologue
    println("; function %s prologue emit_text %s %d",fn->name,__FILE__,__LINE__);

    // only one argument pass via Acc A,B, @long
    // Save passed-by-register arguments to the stack
    int gp = 0;
    // 返り値がSTRUCT/UNIONの場合は、レジスタ引数に返り値のアドレスが入るので
    // 関数引数はレジスタに入らない
    switch (fn->ty->return_ty->kind){
    case TY_STRUCT:
    case TY_UNION:
      gp++;
    }
    for (Obj *var = fn->params; var; var = var->next) {
      if (var->offset > 0)
        continue;

      Type *ty = var->ty;

      switch (ty->kind) {
      case TY_STRUCT:
      case TY_UNION:
	break;
      case TY_DOUBLE:
      case TY_LDOUBLE:
	assert(ty->kind!=TY_DOUBLE && ty->kind!=TY_LDOUBLE);
	break;
      default:
	gp++;
      }
    }
    int save_reg_param = 0;
    // 返り値がSTRUCT/UNIONの場合は、レジスタ引数に返り値のアドレスが入る
    switch (fn->ty->return_ty->kind){
    case TY_STRUCT:
    case TY_UNION:
      save_reg_param = 1;
      break;
    default:
      for (Obj *var = fn->params; var; var = var->next) {
        if (var->reg_param) {
	  switch(var->ty->kind){
	  case TY_SHORT:
	  case TY_INT:
	  case TY_PTR:
	  case TY_ENUM:
  	    save_reg_param = 1;
  	    break;
	  }
 	}
      }
    }
    // make base pointer
    if (save_reg_param)
      println("\tstaa @tmp1");
    println("\tldaa @bp+1");			// push old @bp
    println("\tpsha");
    println("\tldaa @bp");
    println("\tpsha");
    if (save_reg_param)
      println("\tldaa @tmp1");
    int reg_param_size = 0;
    switch (fn->ty->return_ty->kind){
    case TY_STRUCT:
    case TY_UNION:
      println("\tpshb");
      println("\tpsha");
      reg_param_size = 2;
      break;
    default:
      for (Obj *var = fn->params; var; var = var->next) {
        if (var->reg_param){ // push argment pass by register
          reg_param_size = var->ty->size;
  	  switch(var->ty->kind){
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
          case TY_FLOAT:
          case TY_LONG:
    	    println("\tldab @long+3");
	    println("\tpshb");
    	    println("\tldab @long+2");
	    println("\tpshb");
    	    println("\tldab @long+1");
	    println("\tpshb");
    	    println("\tldab @long");
	    println("\tpshb");
	    break;
	  }
	}
      }
    }
    // make base pointer
    if (fn->stack_size<=6){
      for(int i=0; i<fn->stack_size; i++)
        println("\tdes");
      println("\ttsx");
      println("\tstx @bp");
      IX_Dest = IX_BP;
    }else{
      println("\tsts @bp");			// make new bp
      println("\tldab @bp+1");
      println("\tldaa @bp");
      println("\tsubb #<%u",fn->stack_size-1);
      println("\tsbca #>%u",fn->stack_size-1);
      println("\tstab @bp+1");
      println("\tstaa @bp");
      println("\tldx @bp");
      IX_Dest = IX_BP;
      println("\ttxs");
      depth = 0;
    }
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
//  println("  mov %%rsp, %d(%%rbp)", fn->alloca_bottom->offset);
    // Emit code
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
#if 0
    if (fn->stack_size + reg_param_size <= 255){
      println("\tldx @bp");					// 4 2 // 50 23
      println("\tldx %d,x",fn->stack_size + reg_param_size);	// 6 2
      println("\tpshb");					// 4 1
      println("\tldab @bp+1");					// 3 2
      println("\taddb #<%u",fn->stack_size+reg_param_size+1);	// 2 2
      println("\tstab @bp+1");					// 4 2
      println("\tldab @bp");					// 3 2
      println("\tadcb #>%u",fn->stack_size+reg_param_size+1);	// 2 2
      println("\tstab @bp");					// 4 2
      println("\tpulb");					// 4 1
      println("\tlds @bp");					// 4 2
      println("\tstx @bp");					// 5 2
      println("\trts");						// 5 1
    }else
#endif
    if (fn->stack_size + reg_param_size <= 10){
      println("\tlds @bp");
      for(int i=0; i<fn->stack_size + reg_param_size - 1; i++)
	println("\tins");
    }else{
      switch (fn->ty->return_ty->kind){
      case TY_VOID:
      case TY_LONG:
      case TY_FLOAT:
	break;
      default:
        println("\tpshb");
      }
      println("\tldab @bp+1");					// 3 2 // 18 12
      println("\taddb #<%u",fn->stack_size+reg_param_size-1);	// 2 2
      println("\tstab @bp+1");					// 4 2
      println("\tldab @bp");					// 3 2
      println("\tadcb #>%u",fn->stack_size+reg_param_size-1);	// 2 2
      println("\tstab @bp");					// 4 2
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
      println("\tpulb");	// 4 1	// 16 6 + 5 1 = 21 7
      println("\tstab @bp");	// 4 2
      println("\tpulb");	// 4 1
      println("\tstab @bp+1");	// 4 2
      break;
    default:
      println("\ttsx");		// 4 1	// 23 7 + 5 1 = 28 8
      println("\tldx 0,x");	// 6 2
      IX_Dest = IX_None;
      println("\tins");		// 4 1
      println("\tins");		// 4 1
      println("\tstx @bp");	// 5 2
    }
    println("\trts");		// 5 1
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
