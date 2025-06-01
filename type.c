#include "chibicc.h"

Type *ty_void = &(Type){TY_VOID, 1, 1};
Type *ty_bool = &(Type){TY_BOOL, 1, 1, true};

Type *ty_char = &(Type){TY_CHAR, 1, 1};
Type *ty_short = &(Type){TY_SHORT, 2, 1};
Type *ty_int = &(Type){TY_INT, 2, 1};
Type *ty_long = &(Type){TY_LONG, 4, 1};

Type *ty_uchar = &(Type){TY_CHAR, 1, 1, true};
Type *ty_ushort = &(Type){TY_SHORT, 2, 1, true};
Type *ty_uint = &(Type){TY_INT, 2, 1, true};
Type *ty_ulong = &(Type){TY_LONG, 4, 1, true};

Type *ty_float = &(Type){TY_FLOAT, 4, 1};
Type *ty_double = &(Type){TY_DOUBLE, 4, 1};
Type *ty_ldouble = &(Type){TY_LDOUBLE, 4, 1};

static Type *new_type(TypeKind kind, int size, int align) {
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = kind;
  ty->size = size;
  ty->align = 1; // align must be 1
  return ty;
}

bool is_integer(Type *ty) {
  TypeKind k = ty->kind;
  return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT ||
         k == TY_INT  || k == TY_LONG || k == TY_ENUM;
}

bool is_integer_or_ptr(Type *ty) {
  TypeKind k = ty->kind;
  return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT ||
         k == TY_INT  || k == TY_LONG || k == TY_ENUM  ||
         k == TY_PTR;
}

bool is_flonum(Type *ty) {
  return ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE ||
         ty->kind == TY_LDOUBLE;
}

bool is_numeric(Type *ty) {
  return is_integer(ty) || is_flonum(ty);
}

// add from slimcc
bool is_array(Type *ty) {
  return ty->kind == TY_ARRAY || ty->kind == TY_VLA;
}

bool is_bitfield(Node *node) {
  return node->kind == ND_MEMBER && node->member->is_bitfield;
}

static bool is_bitfield2(Node *node, int *width) {
  switch (node->kind) {
  case ND_ASSIGN:
  case ND_POST_INCDEC:
    return is_bitfield2(node->lhs, width);
  case ND_COMMA:
    return is_bitfield2(node->rhs, width);
  case ND_STMT_EXPR: {
    Node *stmt = node->body;
    while (stmt->next)
      stmt = stmt->next;
    if (stmt->kind == ND_EXPR_STMT)
      return is_bitfield2(stmt->lhs, width);
  }
  case ND_MEMBER:
    if (!node->member->is_bitfield)
      return false;
    *width = node->member->bit_width;
    return true;
  }
  return false;
}

// add from slimcc
bool is_redundant_cast(Node *expr, Type *ty) {
  if (expr->kind != ND_CAST)
    return false;

  Type *ty2 = expr->ty;
  Type *ty3 = expr->lhs->ty;
  int sz = ty->size;
  int sz2 = ty2->size;
  int sz3 = ty3->size;

  if (is_integer(ty) && is_integer(ty2) && is_integer(ty3)) {
    if (ty3->kind == TY_BOOL)
      return true;
    if (ty2->kind == TY_BOOL)
      return false;
    if (ty->kind == TY_BOOL)
      return sz2 >= sz3;

    if (sz <= sz3)
      return sz <= sz2;
    if (sz <= sz2)
      return true;
    if (sz2 == sz3)
      return ty3->is_unsigned == ty2->is_unsigned;
    if (sz2 > sz3)
      return ty3->is_unsigned || !ty2->is_unsigned;
  }
  return false;
}


bool is_compatible(Type *t1, Type *t2) {
  if (t1 == t2)
    return true;

  if (t1->origin)
    return is_compatible(t1->origin, t2);

  if (t2->origin)
    return is_compatible(t1, t2->origin);

  if (t1->kind != t2->kind)
    return false;

  switch (t1->kind) {
  case TY_CHAR:
  case TY_SHORT:
  case TY_INT:
  case TY_LONG:
    return t1->is_unsigned == t2->is_unsigned;
  case TY_FLOAT:
  case TY_DOUBLE:
  case TY_LDOUBLE:
    return true;
  case TY_PTR:
    return is_compatible(t1->base, t2->base);
  case TY_FUNC: {
    if (!is_compatible(t1->return_ty, t2->return_ty))
      return false;
    if (t1->is_variadic != t2->is_variadic)
      return false;

    Type *p1 = t1->params;
    Type *p2 = t2->params;
    for (; p1 && p2; p1 = p1->next, p2 = p2->next)
      if (!is_compatible(p1, p2))
        return false;
    return p1 == NULL && p2 == NULL;
  }
  case TY_ARRAY:
    if (!is_compatible(t1->base, t2->base))
      return false;
    return t1->array_len < 0 && t2->array_len < 0 &&
           t1->array_len == t2->array_len;
  }
  return false;
}

Type *copy_type(Type *ty) {
  Type *ret = calloc(1, sizeof(Type));
  *ret = *ty;
  ret->origin = ty;
  return ret;
}

Type *pointer_to(Type *base) {
  Type *ty = new_type(TY_PTR, 2, 1);
  ty->base = base;
  ty->is_unsigned = true;
  return ty;
}

Type *func_type(Type *return_ty) {
  // The C spec disallows sizeof(<function type>), but
  // GCC allows that and the expression is evaluated to 1.
  Type *ty = new_type(TY_FUNC, 1, 1);
  ty->return_ty = return_ty;
  return ty;
}

Type *array_of(Type *base, int len) {
  Type *ty = new_type(TY_ARRAY, base->size * len, 1);
//Type *ty = new_type(TY_ARRAY, base->size * len, base->align);
  ty->base = base;
  ty->array_len = len;
  return ty;
}

Type *vla_of(Type *base, Node *len) {
  Type *ty = new_type(TY_VLA, 2, 1);
//Type *ty = new_type(TY_VLA, 8, 8);
  ty->base = base;
  ty->vla_len = len;
  return ty;
}

int int_rank(Type *t) {
  switch (t->kind) {
    case TY_BOOL:
    case TY_CHAR:
    case TY_SHORT:
      return 0;
    case TY_INT:
      return 1;
    case TY_LONG:
      return 2;
  }
  assert(0);
}

Type *enum_type(void) {
  return new_type(TY_ENUM, 2, 1);
}

Type *struct_type(void) {
  return new_type(TY_STRUCT, 0, 1);
}

static void int_promotion(Node **node) {
  Type *ty = (*node)->ty;
  int bit_width;

  if (is_bitfield2(*node, &bit_width)) {
    int int_width = ty_int->size * 8;	// byte:8bit

    if (bit_width == int_width && ty->is_unsigned) {
      *node = new_cast(*node, ty_uint);
    } else if (bit_width <= int_width) {
      *node = new_cast(*node, ty_int);
    } else {
      *node = new_cast(*node, ty);
    }
    return;
  }

  if (ty->size < ty_int->size) {
    *node = new_cast(*node, ty_int);
    return;
  }

  if (ty->size == ty_int->size && int_rank(ty) < int_rank(ty_int)) {
    if (ty->is_unsigned)
      *node = new_cast(*node, ty_uint);
    else
      *node = new_cast(*node, ty_int);
    return;
  }
}

static Type *get_common_type(Type *ty1, Type *ty2) {
  if (ty1->base)
    return pointer_to(ty1->base);

  if (ty1->kind == TY_FUNC)
    return pointer_to(ty1);
  if (ty2->kind == TY_FUNC)
    return pointer_to(ty2);

  if (ty1->kind == TY_LDOUBLE || ty2->kind == TY_LDOUBLE)
    return ty_ldouble;
  if (ty1->kind == TY_DOUBLE || ty2->kind == TY_DOUBLE)
    return ty_double;
  if (ty1->kind == TY_FLOAT || ty2->kind == TY_FLOAT)
    return ty_float;

  if (ty1->size < 2)
    ty1 = ty_int;
  if (ty2->size < 2)
    ty2 = ty_int;

  if (ty1->size != ty2->size)
    return (ty1->size < ty2->size) ? ty2 : ty1;

  if (ty2->is_unsigned)
    return ty2;
  return ty1;
}

// For many binary operators, we implicitly promote operands so that
// both operands have the same type. Any integral type smaller than
// int is always promoted to int. If the type of one operand is larger
// than the other's (e.g. "long" vs. "int"), the smaller operand will
// be promoted to match with the other.
//
// This operation is called the "usual arithmetic conversion".
static void usual_arith_conv(Node **lhs, Node **rhs) {
  Type *ty = get_common_type((*lhs)->ty, (*rhs)->ty);
  *lhs = new_cast(*lhs, ty);
  *rhs = new_cast(*rhs, ty);
}

void add_type(Node *node) {
  if (!node || node->ty)
    return;

  add_type(node->lhs);
  add_type(node->rhs);
  add_type(node->cond);
  add_type(node->then);
  add_type(node->els);
  add_type(node->init);
  add_type(node->inc);

  for (Node *n = node->body; n; n = n->next)
    add_type(n);
  for (Node *n = node->args; n; n = n->next)
    add_type(n);

  switch (node->kind) {
  case ND_NUM:
    node->ty = ty_int;	// TODO: LONG, CHAR
    return;
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_MOD:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    usual_arith_conv(&node->lhs, &node->rhs);
    node->ty = node->lhs->ty;
    return;
  case ND_NEG: {
    if (!is_numeric(node->lhs->ty))
      error_tok(node->lhs->tok, "invalid operand");
    if (is_integer(node->lhs->ty))
      int_promotion(&node->lhs);
    node->ty = node->lhs->ty;
    return;
  }
  case ND_ASSIGN:
    if (node->lhs->ty->kind == TY_ARRAY)
      error_tok(node->lhs->tok, "not an lvalue");
    if (node->lhs->ty->kind != TY_STRUCT)
      node->rhs = new_cast(node->rhs, node->lhs->ty);
    node->ty = node->lhs->ty;
    return;
  case ND_ADDEQ:
  case ND_SUBEQ:
    if (node->lhs->ty->kind == TY_ARRAY)
      error_tok(node->lhs->tok, "not an lvalue");
    if (node->lhs->ty->kind == TY_PTR) {
      if (!is_integer(node->rhs->ty)) {
        error_tok(node->lhs->tok, "invalid operands");
      }
    }else if (!is_numeric(node->lhs->ty) || !is_numeric(node->rhs->ty)) {
      error_tok(node->lhs->tok, "invalid operands");
    }
    node->rhs = new_cast(node->rhs, node->lhs->ty);
    node->ty = node->lhs->ty;
    return;
  case ND_MULEQ:
  case ND_DIVEQ:
    if (node->lhs->ty->kind == TY_ARRAY)
      error_tok(node->lhs->tok, "not an lvalue");
    if (!is_numeric(node->lhs->ty)
    ||  !is_numeric(node->rhs->ty)) {
      error_tok(node->lhs->tok, "invalid operands");
    }
    node->rhs = new_cast(node->rhs, node->lhs->ty);
    node->ty = node->lhs->ty;
    return;
  case ND_MODEQ:
  case ND_ANDEQ:
  case ND_OREQ:
  case ND_XOREQ:
    if (node->lhs->ty->kind == TY_ARRAY)
      error_tok(node->lhs->tok, "not an lvalue");
    if (!is_integer(node->lhs->ty)
    ||  !is_integer(node->rhs->ty)) {
      error_tok(node->lhs->tok, "invalid operands");
    }
    node->rhs = new_cast(node->rhs, node->lhs->ty);
    node->ty = node->lhs->ty;
    return;
  case ND_SHLEQ:
  case ND_SHREQ:
    if (node->lhs->ty->kind == TY_ARRAY)
      error_tok(node->lhs->tok, "not an lvalue");
    if (!is_integer(node->lhs->ty)
    ||  !is_integer(node->rhs->ty)) {
      error_tok(node->lhs->tok, "invalid operands");
    }
    node->rhs = new_cast(node->rhs, ty_uchar);
    node->ty = node->lhs->ty;
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    usual_arith_conv(&node->lhs, &node->rhs);
    node->ty = ty_int;	// TODO: bool?
    return;
  case ND_FUNCALL:
    node->ty = node->func_ty->return_ty;
    return;
  case ND_NOT:
  case ND_LOGOR:
  case ND_LOGAND:
    node->ty = ty_int;	// TODO: bool?
    return;
  case ND_BITNOT:
    if (!is_integer(node->lhs->ty))
      error_tok(node->lhs->tok, "invalid operand");
    int_promotion(&node->lhs);
    node->ty = node->lhs->ty;
    return;
  case ND_SHL:
  case ND_SHR:
    if (!is_integer(node->lhs->ty))
      error_tok(node->lhs->tok, "invalid operand");
    if (!is_integer(node->rhs->ty))
      error_tok(node->rhs->tok, "invalid operand");
    int_promotion(&node->lhs);
    node->rhs = new_cast(node->rhs, ty_uchar);
    node->ty = node->lhs->ty;
    return;
  case ND_VAR:
  case ND_VLA_PTR:
    node->ty = node->var->ty;
    return;
  case ND_COND:
    if (node->then->ty->kind == TY_VOID || node->els->ty->kind == TY_VOID) {
      node->ty = ty_void;
    } else {
      usual_arith_conv(&node->then, &node->els);
      node->ty = node->then->ty;
    }
    return;
  case ND_COMMA:
    node->ty = node->rhs->ty;
    return;
  case ND_MEMBER:
    node->ty = node->member->ty;
    return;
  case ND_ADDR: {
    Type *ty = node->lhs->ty;
    if (ty->kind == TY_ARRAY)
      node->ty = pointer_to(ty->base);
    else
      node->ty = pointer_to(ty);
    return;
  }
  case ND_DEREF:
    if (!node->lhs->ty->base)
      error_tok(node->tok, "invalid pointer dereference");
    if (node->lhs->ty->base->kind == TY_VOID)
      error_tok(node->tok, "dereferencing a void pointer");

    node->ty = node->lhs->ty->base;
    return;
  case ND_STMT_EXPR:
    if (node->body) {
      Node *stmt = node->body;
      while (stmt->next)
        stmt = stmt->next;
      if (stmt->kind == ND_EXPR_STMT) {
        node->ty = stmt->lhs->ty;
        return;
      }
    }
    error_tok(node->tok, "statement expression returning void is not supported");
    return;
  case ND_LABEL_VAL:
    node->ty = pointer_to(ty_void);
    return;
  case ND_CAS:
    add_type(node->cas_addr);
    add_type(node->cas_old);
    add_type(node->cas_new);
    node->ty = ty_bool;

    if (node->cas_addr->ty->kind != TY_PTR)
      error_tok(node->cas_addr->tok, "pointer expected");
    if (node->cas_old->ty->kind != TY_PTR)
      error_tok(node->cas_old->tok, "pointer expected");
    return;
  case ND_EXCH:
    if (node->lhs->ty->kind != TY_PTR)
      error_tok(node->cas_addr->tok, "pointer expected");
    node->ty = node->lhs->ty->base;
    return;
  }
}
