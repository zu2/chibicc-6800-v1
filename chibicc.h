#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <glob.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#ifndef __GNUC__
# define __attribute__(x)
#endif

typedef struct Type Type;
typedef struct Node Node;
typedef struct Member Member;
typedef struct Relocation Relocation;
typedef struct Hideset Hideset;

//
// strings.c
//

typedef struct {
  char **data;
  int capacity;
  int len;
} StringArray;

void strarray_push(StringArray *arr, char *s);
char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));

//
// tokenize.c
//

// Token
typedef enum {
  TK_IDENT,   // Identifiers
  TK_PUNCT,   // Punctuators
  TK_KEYWORD, // Keywords
  TK_STR,     // String literals
  TK_NUM,     // Numeric literals
  TK_PP_NUM,  // Preprocessing numbers
  TK_EOF,     // End-of-file markers
} TokenKind;

typedef struct {
  char *name;
  int file_no;
  char *contents;

  // For #line directive
  char *display_name;
  int line_delta;
} File;

// Token type
typedef struct Token Token;
struct Token {
  TokenKind kind;   // Token kind
  Token *next;      // Next token
  int64_t val;      // If kind is TK_NUM, its value
  long double fval; // If kind is TK_NUM, its value
  char *loc;        // Token location
  int len;          // Token length
  Type *ty;         // Used if TK_NUM or TK_STR
  char *str;        // String literal contents including terminating '\0'

  File *file;       // Source location
  char *filename;   // Filename
  int line_no;      // Line number
  int line_delta;   // Line number
  bool at_bol;      // True if this token is at beginning of line
  bool has_space;   // True if this token follows a space character
  Hideset *hideset; // For macro expansion
  Token *origin;    // If this is expanded from a macro, the original token
};

noreturn void error(char *fmt, ...) __attribute__((format(printf, 1, 2)));
noreturn void error_at(char *loc, char *fmt, ...) __attribute__((format(printf, 2, 3)));
noreturn void error_tok(Token *tok, char *fmt, ...) __attribute__((format(printf, 2, 3)));
void warn_tok(Token *tok, char *fmt, ...) __attribute__((format(printf, 2, 3)));
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
void convert_pp_tokens(Token *tok);
File **get_input_files(void);
File *new_file(char *name, int file_no, char *contents);
Token *tokenize_string_literal(Token *tok, Type *basety);
Token *tokenize(File *file);
Token *tokenize_file(char *filename);

#define unreachable() \
  error("internal error at %s:%d", __FILE__, __LINE__)

//
// preprocess.c
//

char *search_include_paths(char *filename);
void init_macros(void);
void define_macro_cli(char *name, char *buf);
void define_macro(char *name, char *buf);
void undef_macro(char *name);
Token *preprocess(Token *tok);

//
// parse.c
//
int64_t eval2(Node *node, char ***label);
int64_t eval(Node *node);
Node *new_copy(Node *node);
bool is_const_expr(Node *node);

// Variable or function
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name;    // Variable name
  Type *ty;      // Type
  Token *tok;    // representative token
  bool is_local; // local or global/function
  int align;     // alignment

  // Local variable
  int offset;
  int reg_param;

  // Global variable or function
  bool is_function;
  bool is_definition;
  bool is_static;

  // Global variable
  bool is_tentative;
  bool is_tls;
  bool is_literal;
  char *init_data;
  Relocation *rel;

  // Function
  bool is_inline;
  Obj *params;
  Node *body;
  Obj *locals;
  Obj *va_area;
  Obj *alloca_bottom;
  int stack_size;
  int function_no;
  bool use_alloca;

  // Static inline function
  bool is_live;
  bool is_root;
  StringArray refs;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.
typedef struct Relocation Relocation;
struct Relocation {
  Relocation *next;
  int offset;
  char **label;
  long addend;
};

// AST node
typedef enum {
  ND_NULL_EXPR, // 0: Do nothing
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_MOD,       // %
  ND_BITAND,    // &
  ND_BITOR,     // |
  ND_BITXOR,    // ^
  ND_SHL,       // 10: <<
  ND_SHR,       // >>
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_GT,        // >
  ND_GE,        // >=
  ND_ASSIGN,    // =
  ND_COND,      // ?:
  ND_COMMA,     // 20: ,
  ND_MEMBER,    // . (struct member access)
  ND_ADDR,      // unary &
  ND_DEREF,     // unary *
  ND_NOT,       // !
  ND_BITNOT,    // ~
  ND_LOGAND,    // &&
  ND_LOGOR,     // ||
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // 30: "for" or "while"
  ND_DO,        // "do"
  ND_SWITCH,    // "switch"
  ND_CASE,      // "case"
  ND_BLOCK,     // { ... }
  ND_GOTO,      // "goto"
  ND_GOTO_EXPR, // "goto" labels-as-values
  ND_LABEL,     // Labeled statement
  ND_LABEL_VAL, // [GNU] Labels-as-values
  ND_FUNCALL,   // Function call
  ND_EXPR_STMT, // 40: Expression statement
  ND_STMT_EXPR, // Statement expression
  ND_VAR,       // Variable
  ND_VLA_PTR,   // VLA designator
  ND_NUM,       // 44: Integer
  ND_CAST,      // Type cast
  ND_MEMZERO,   // Zero-clear a stack variable
  ND_ASM,       // "asm"
  ND_CAS,       // Atomic compare-and-swap
  ND_EXCH,      // Atomic exchange
  ND_POST_INCDEC,// post inc/dec (++,--)
  ND_PRE_INCDEC, // pre  inc/dec (++,--)
} NodeKind;

// AST node type
struct Node {
  NodeKind kind; // Node kind
  Node *next;    // Next node
  Type *ty;      // Type, e.g. int or pointer to int
  Token *tok;    // Representative token
  char *loc;

  Node *lhs;     // Left-hand side
  Node *rhs;     // Right-hand side

  // "if" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // "break" and "continue" labels
  char *brk_label;
  char *cont_label;

  // Block or statement expression
  Node *body;

  // Struct member access
  Member *member;

  // Function call
  Type *func_ty;
  Node *args;
  bool pass_by_stack;
  Obj *ret_buffer;

  // Goto or labeled statement, or labels-as-values
  char *label;
  char *unique_label;
  Node *goto_next;

  // Switch
  Node *case_next;
  Node *default_case;

  // Case
  long begin;
  long end;

  // "asm" string literal
  char *asm_str;

  // Atomic compare-and-swap
  Node *cas_addr;
  Node *cas_old;
  Node *cas_new;

  // Atomic op= operators
  Obj *atomic_addr;
  Node *atomic_expr;

  // Variable
  Obj *var;

  // Numeric literal
  int64_t val;
  long double fval;

  // optimize hint
  bool retval_unused;
  bool bool_result_unused;
};

Node *new_cast(Node *expr, Type *ty);
int64_t const_expr(Token **rest, Token *tok);
Obj *parse(Token *tok);
Node *new_node(NodeKind kind, Token *tok);

//
// type.c
//

typedef enum {
  TY_VOID,	// 0
  TY_BOOL,
  TY_CHAR,
  TY_SHORT,
  TY_INT,
  TY_LONG,	// 5
  TY_FLOAT,
  TY_DOUBLE,
  TY_LDOUBLE,
  TY_ENUM,
  TY_PTR,	// 10
  TY_FUNC,
  TY_ARRAY,
  TY_VLA, // variable-length array
  TY_STRUCT,
  TY_UNION,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;           // sizeof() value
  int align;          // alignment
  bool is_unsigned;   // unsigned or signed
  bool is_atomic;     // true if _Atomic
  Type *origin;       // for type compatibility check

  // Pointer-to or array-of type. We intentionally use the same member
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // member instead of "kind" member to determine whether a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  Type *base;

  // Declaration
  Token *name;
  Token *name_pos;

  // Array
  int array_len;

  // Variable-length array
  Node *vla_len; // # of elements
  Obj *vla_size; // sizeof() value

  // Struct
  Member *members;
  bool is_flexible;
  bool is_packed;

  // Function type
  Type *return_ty;
  Type *params;
  bool is_variadic;
  Type *next;
};

// Struct member
struct Member {
  Member *next;
  Type *ty;
  Token *tok; // for error message
  Token *name;
  int idx;
  int align;
  int offset;

  // Bitfield
  bool is_bitfield;
  int bit_offset;
  int bit_width;
};

extern Type *ty_void;
extern Type *ty_bool;

extern Type *ty_char;
extern Type *ty_short;
extern Type *ty_int;
extern Type *ty_long;

extern Type *ty_uchar;
extern Type *ty_ushort;
extern Type *ty_uint;
extern Type *ty_ulong;

extern Type *ty_float;
extern Type *ty_double;
extern Type *ty_ldouble;

bool is_integer(Type *ty);
bool is_flonum(Type *ty);
bool is_numeric(Type *ty);
bool is_redundant_cast(Node *expr, Type *ty);
bool is_compatible(Type *t1, Type *t2);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int size);
Type *vla_of(Type *base, Node *expr);
Type *enum_type(void);
Type *struct_type(void);
void add_type(Node *node);

//
// ast_dump.c
//
void ast_node_dump(Node *node);

//
// optimize.c
//
Node *optimize_expr(Node *node);
Obj *optimize(Obj *prog);
Node *swap_lr(Node *node);

//
// codegen.c
//
__attribute__((format(printf, 1, 2)))
void println(char *fmt, ...);
__attribute__((format(printf, 1, 2)))
void printout(char *fmt, ...);
int  test_addr_x(Node *node);
void codegen(Obj *prog, FILE *out);
int align_to(int n, int align);
bool is_compare(Node *node);
bool is_compare_or_not(Node *node);
bool is_boolean_result(Node *node);
bool gen_direct(Node *rhs,char *opb, char *opa);
bool can_direct(Node *rhs);
void gen_expr(Node *node);
int gen_addr_x(Node *node,bool save_d);
int count(void);
void push(void);
void load_var(Node *node);
void cmp_zero(Type *ty);
Type *is_integer_constant(Node *node, int64_t *val);

extern int depth;
typedef	enum {
  IX_None,
  IX_BP,
  IX_VAR,
} IX_Type;

extern IX_Type	IX_Dest;

//
// gen_jump.c
//
int gen_jump_if_false(Node *node,char *if_false);
int gen_jump_if_true(Node *node,char *if_true);

//
// unicode.c
//

int encode_utf8(char *buf, uint32_t c);
uint32_t decode_utf8(char **new_pos, char *p);
bool is_ident1(uint32_t c);
bool is_ident2(uint32_t c);
int display_width(char *p, int len);

//
// hashmap.c
//

typedef struct {
  char *key;
  int keylen;
  void *val;
} HashEntry;

typedef struct {
  HashEntry *buckets;
  int capacity;
  int used;
} HashMap;

void *hashmap_get(HashMap *map, char *key);
void *hashmap_get2(HashMap *map, char *key, int keylen);
void hashmap_put(HashMap *map, char *key, void *val);
void hashmap_put2(HashMap *map, char *key, int keylen, void *val);
void hashmap_delete(HashMap *map, char *key);
void hashmap_delete2(HashMap *map, char *key, int keylen);
void hashmap_test(void);

//
// main.c
//

bool file_exists(char *path);

extern StringArray include_paths;
extern bool opt_fpic;
extern bool opt_fcommon;
extern bool opt_optimize;
extern bool opt_g;
extern char *base_file;
