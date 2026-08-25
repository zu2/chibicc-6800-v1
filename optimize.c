#include "chibicc.h"

// use TY_CHAR not is_int8: (_Bool)(1^3) is 1, but (_Bool)1 ^ (_Bool)3 is 0
bool is_integral_promotion(Node *node)
{
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_INT
  && !node->ty->is_unsigned
  &&  node->lhs->ty->kind == TY_CHAR) {
    return true;
  }
  return false;
}

bool is_integral_promotion_or_char(Node *node)
{
  if (is_integral_promotion(node)) {
    return true;
  }
  if (node->ty->kind == TY_CHAR) {
    return true;
  }
  return false;
}

Node *skip_integral_promotion(Node *node)
{
  if (is_integral_promotion(node)) {
    return node->lhs;
  }
  return node;
}

bool is_byte_to_uint(Node *node)
{
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_INT
  &&  node->ty->is_unsigned) {
    if (node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned) {
      return true;
    }
    if (node->lhs->ty->kind == TY_BOOL) {
      return true;
    }
  }
  return false;
}

Node *skip_byte_to_uint(Node *node)
{
  if (is_byte_to_uint(node)) {
    return node->lhs;
  }
  return node;
}

bool is_byte_to_int(Node *node)
{
  return is_integral_promotion(node) || is_byte_to_uint(node);
}

Node *skip_byte_to_int(Node *node)
{
  if (is_byte_to_int(node)) {
    return node->lhs;
  }
  return node;
}


// the value is 0..255, so "op uchar" is enough: a comparison, or a cast up from uchar
static bool can_op_uchar(Node *node)
{
  if (is_compare_or_not(node)) {
    return true;
  }
  if (node->kind == ND_CAST
  &&  is_int16(node->ty)
  &&  node->lhs->ty->kind == TY_CHAR
  &&  node->lhs->ty->is_unsigned) {
    return true;
  }
  if (node->kind == ND_CAST
  &&  is_int16(node->ty)
  &&  node->lhs->ty->kind == TY_BOOL) {
    return true;
  }
  return false;
}

bool is_u8num(Node *node)
{
  int64_t val;

  if (is_integer_constant(node, &val)) {
    if (val >= 0  && val <= 255) {
      return true;
    }
  }
  return false;
}

bool is_s8num(Node *node)
{
  int64_t val;

  if (is_integer_constant(node, &val)) {
    if (val >= -128  && val <= 127) {
      return true;
    }
  }
  return false;
}

bool is_uchar_or_u8num(Node *node)
{
  if (node->ty->kind == TY_CHAR && node->ty->is_unsigned)
    return true;
  if (is_u8num(node))
    return true;

  return false;
}

bool is_schar_or_s8num(Node *node)
{
  if (node->ty->kind == TY_CHAR && !node->ty->is_unsigned)
    return true;
  if (is_s8num(node))
    return true;

  return false;
}

bool is_char_or_8num(Node *node)
{
  if (is_int8(node->ty))
    return true;
  if (is_u8num(node) || is_s8num(node))
    return true;

  return false;
}

// for a power of 2, return the shift count. -1 for any other value
int exact_log2(int64_t val)
{
  int n = 0;

  if (val <= 0 || (val & (val-1)) != 0) {
    return -1;
  }
  while ((val >>= 1) > 0) {
    n++;
  }
  return n;
}

static bool can_negate_compare(Node *node)
{
  switch(node->kind) {
  case ND_EQ:
  case ND_NE:
    return true;
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    return !is_flonum(node->lhs->ty) && !is_flonum(node->rhs->ty);
  }
  return false;
}

static int64_t ty_max_value(Type *ty)
{
  switch(ty->kind){
  case TY_BOOL: return 1;
  case TY_CHAR: return ty->is_unsigned? UINT8_MAX: INT8_MAX;
  case TY_SHORT:
  case TY_INT:
  case TY_ENUM: return ty->is_unsigned? UINT16_MAX: INT16_MAX;
  case TY_LONG: return ty->is_unsigned? UINT32_MAX: INT32_MAX;
  }
  return 0;
}

static NodeKind negate_kind(NodeKind kind)
{
  switch(kind){
  case ND_EQ: return ND_NE;
  case ND_NE: return ND_EQ;
  case ND_LT: return ND_GE;
  case ND_LE: return ND_GT;
  case ND_GT: return ND_LE;
  case ND_GE: return ND_LT;
  }
  assert(0);
}

//   !(a<b)         -> a>=b
//   !(a<b && c<d)  -> a>=b || c>=d
//   !(a<b && x)    -> a>=b || !x
//   !(x && y)      -> !x || !y
//   !(x || y)      -> !x && !y
static Node *optimize_demorgan(Node *node)
{
  Node *lhs, *rhs, *new;

  if (can_negate_compare(node)) {
    new = new_binary(negate_kind(node->kind), node->lhs, node->rhs, node->tok);
    new->ty = node->ty;
    return new;
  }
  if (node->kind != ND_LOGAND && node->kind != ND_LOGOR) {
    return NULL;
  }
  lhs = optimize_demorgan(node->lhs);
  rhs = optimize_demorgan(node->rhs);
  if (!lhs) {
    lhs = new_unary(ND_NOT, node->lhs, node->lhs->tok);
    lhs->ty = ty_int;
  }
  if (!rhs) {
    rhs = new_unary(ND_NOT, node->rhs, node->rhs->tok);
    rhs->ty = ty_int;
  }
  new = new_binary(node->kind == ND_LOGAND ? ND_LOGOR : ND_LOGAND, lhs, rhs, node->tok);
  new->ty = node->ty;
  return new;
}

Node *flip_condition(Node *node)
{
  switch(node->kind){
  case ND_EQ:
  case ND_NE:
    break;
  case ND_LT:
    node->kind = ND_GT;
    break;
  case ND_LE:
    node->kind = ND_GE;
    break;
  case ND_GT:
    node->kind = ND_LT;
    break;
  case ND_GE:
    node->kind = ND_LE;
    break;
  default:
    assert(0);
  }
  return node;
}

Node *swap_lr(Node *node)
{
  Node *tmp;
  tmp = node->lhs;
  node->lhs = node->rhs;
  node->rhs = tmp;

  return node;
}

static int node_cost(Node *node)
{
  int sign = !node->ty->is_unsigned;

  if (node->kind==ND_NUM) {
    return 2;
  }else if (is_pointer_constant(node,NULL)) {
    return 5;
  }else if (is_addr_constant(node)) {
    return 5+sign;
  }else if (is_global_var(node)) {
    return 150+sign;
  }else if (is_var_addr_constant(node)) {
    return 160+sign;
  }else if (can_direct(node)) {
    return 200+sign;
  }else if (test_addr_x(node)) {
    return 250+sign;
  }else if (node->kind==ND_VAR) {
    return 300+sign
      + ((node->ty->kind == TY_PTR)?   100:0)
      + ((node->ty->kind == TY_ARRAY)? 200:0);
  }else if (node->kind==ND_CAST) {
    return node_cost(node->lhs)+10+sign;
  }else if (node->kind==ND_DEREF) {
    return node_cost(node->lhs)+500+sign;
  }else if (node->kind==ND_FUNCALL) {
    return 1000;
  }

  return 2000;
}

static Node *optimize_lr(Node *node)
{
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);
    return node;
}

static Node *optimize_l(Node *node)
{
    node->lhs = optimize_expr(node->lhs);
    return node;
}

static Node *optimize_lr_swap(Node *node)
{
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);

    if (node_cost(node->lhs) < node_cost(node->rhs))
      return swap_lr(node);
    return node;
}

Node *optimize_const_expr(Node *node)
{
    if (node->kind == ND_NUM) {
      return node;
    }
    if (is_const_expr(node)) {
      Node *new = new_node(ND_NUM, node->tok);
      new->fval = eval_double(node);
      new->val = eval(node);
      new->ty = node->ty;
      return new;
    }
    return node;
}
Node *optimize_args(Node *args)
{
  if (!args->next) {
    return optimize_expr(args);
  }
  Node *new;

  new = optimize_expr(args);
  new->next = optimize_args(args->next);
  return new;
}

Node *optimize_bitop_integral_promotion(Node *node)
{
  // node returns int?
  if (node->ty->kind != TY_INT
  ||  node->ty->is_unsigned) {
    return node;  // no
  }
  switch(node->lhs->kind){
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    node->lhs = optimize_bitop_integral_promotion(node->lhs);
  }
  switch(node->rhs->kind){
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    node->rhs = optimize_bitop_integral_promotion(node->rhs);
  }
  // (~(int)m & (int)x → (int)(~m & x)
  //  m: char or uchar
  //  x: uchar
  // (& TY_INT(4) (ND_CAST TY_INT(4) x) (~ TY_INT(4) (ND_CAST TY_INT(4) m)))
  if (node->kind == ND_BITAND
  &&  node->rhs->kind == ND_BITNOT
  &&  is_integral_promotion(node->rhs->lhs)
  &&  is_integral_promotion(node->lhs)
  &&  node->lhs->lhs->ty->is_unsigned) {
    Node *lhs = node->lhs->lhs;
    Node *rhs = node->rhs->lhs->lhs;
    Node *new = new_copy(node);
    new->ty = ty_uchar;
    new->lhs = lhs;
    new->rhs = new_unary(ND_BITNOT,rhs,rhs->tok);
    new->rhs->ty = ty_uchar;
    new = optimize_lr_swap(new);
    return optimize_const_expr(new_cast(new,ty_int));
  }
  if (node->kind == ND_BITAND
  &&  node->lhs->kind == ND_BITNOT
  &&  is_integral_promotion(node->lhs->lhs)
  &&  is_integral_promotion(node->rhs)
  &&  node->rhs->lhs->ty->is_unsigned) {
    Node *lhs = node->lhs->lhs->lhs;
    Node *rhs = node->rhs->lhs;
    Node *new = new_copy(node);
    new->ty = ty_uchar;
    new->lhs = new_unary(ND_BITNOT,lhs,lhs->tok);
    new->lhs->ty = ty_uchar;
    new->rhs = rhs;
    new = optimize_lr_swap(new);
    return optimize_const_expr(new_cast(new,ty_int));
  }
  // LHS check
  if (!is_integral_promotion_or_char(node->lhs)) {
    return optimize_const_expr(node);
  }
    
  // uchar op 0-255
  //    (int)uchar op 0..255 -> (int)(uchar op 0..255)
  if (skip_integral_promotion(node->lhs)->ty->is_unsigned) {
    if (node->rhs->kind           == ND_NUM
    &&  node->rhs->ty->kind       == TY_INT
    &&  node->rhs->val            >= 0
    &&  node->rhs->val            <= 255) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs = skip_integral_promotion(node->lhs);
      new->rhs = new_copy(node->rhs);
      new->rhs->ty = ty_uchar;
      new = new_cast(new,ty_int);
      return optimize_const_expr(new);
    }
  }
  // (int)char & 0..127 -> (int)(uchar & 0..127)
  // (int)-1 | 0x7F -> -1, not (int)(uchar)0xFF. Same for ^.
  if (node->kind == ND_BITAND
  &&  !skip_integral_promotion(node->lhs)->ty->is_unsigned
  &&  node->rhs->kind     == ND_NUM
  &&  node->rhs->ty->kind == TY_INT
  &&  node->rhs->val      >= 0
  &&  node->rhs->val      <= 127) {
    Node *new = new_copy(node);
    new->ty  = ty_uchar;
    new->lhs = skip_integral_promotion(node->lhs);
    new->rhs = new_copy(node->rhs);
    new->rhs->ty = ty_uchar;
    new = new_cast(new,ty_int);
    return optimize_const_expr(new);
  }
  // char op -128..127
  //    (int)char op -128..127 -> (int)(char op -128..127)
  if (!skip_integral_promotion(node->lhs)->ty->is_unsigned) {
    if (node->rhs->kind           == ND_NUM
    &&  node->rhs->ty->kind       == TY_INT
    &&  node->rhs->val            >= -128
    &&  node->rhs->val            <= 127) {
      Node *new = new_copy(node);
      new->ty  = ty_char;
      new->lhs = skip_integral_promotion(node->lhs);
      new->rhs = new_copy(node->rhs);
      new->rhs->ty = ty_char;
      new = new_cast(new,ty_int);
      return optimize_const_expr(new);
    }
  }
  // char op char, uchar op uchar
  //    (int)lhs op (int)rhs -> (int)(lhs op rhs)
  if (is_integral_promotion_or_char(node->rhs)
  &&  (skip_integral_promotion(node->lhs)->ty->is_unsigned
    == skip_integral_promotion(node->rhs)->ty->is_unsigned)) {
    Node *new = new_copy(node);
    new->ty  = skip_integral_promotion(node->lhs)->ty;
    new->lhs = skip_integral_promotion(node->lhs);
    new->rhs = skip_integral_promotion(node->rhs);
    new = new_cast(new,ty_int);
    return optimize_const_expr(new);
  }
  // char & uchar
  if (node->kind == ND_BITAND
  &&  is_integral_promotion_or_char(node->rhs)
  &&  (skip_integral_promotion(node->lhs)->ty->is_unsigned
    != skip_integral_promotion(node->rhs)->ty->is_unsigned)) {
    Node *new = new_copy(node);
    new->ty  = copy_type(skip_integral_promotion(node->lhs)->ty);
    new->ty->is_unsigned = true;
    new->lhs = skip_integral_promotion(node->lhs);
    new->rhs = skip_integral_promotion(node->rhs);
    new = new_cast(new,ty_int);
    return optimize_const_expr(new);
  }

  return optimize_const_expr(node);
}

Node *optimize_expr(Node *node)
{
  int64_t val;
  double fval;

  if (!node)
    return node;

  node = optimize_const_expr(node);

  switch (node->kind) {
  case ND_NULL_EXPR:
  case ND_NUM:
    return node;
  case ND_NEG:
    node = optimize_l(node);
    if (is_integer_constant(node->lhs,&val)) {
      return optimize_const_expr(node);
    }else if (node->lhs->kind==ND_NUM
    &&  is_flonum(node->lhs->ty)){
      Node *new = new_copy(node->lhs);
      new->fval = -node->lhs->fval;
      return new;
    }
    return node;
  case ND_VAR:
    if (!node->ty) {
      fprintf(stderr,"; need debug %s %d\n",__FILE__,__LINE__);
      assert(0);
      return node;
    }
    return node;
  case ND_MEMBER:
    return optimize_l(node);
  case ND_DEREF:
    node->lhs = optimize_expr(node->lhs);
    return node;
  case ND_ADDR:
    node->lhs = optimize_expr(node->lhs);
    if (node->lhs->kind == ND_DEREF) {
      Node *lhs = node->lhs->lhs;
      /* keep the T* type. an outer pointer cast folds this away */
      if (lhs->ty->kind == TY_ARRAY)
        return new_cast(lhs, node->ty);
      return lhs;
    }
    return node;
  case ND_ASSIGN:
    node = optimize_lr(node);
    if(node->lhs->ty->kind == TY_BOOL
    && is_integer_constant(node->rhs,&val)) {
      node->rhs = new_num(val!=0,node->rhs->tok);
      node->rhs->ty = node->lhs->ty;
      node->ty = node->lhs->ty;
      return node;
    }
    if(node->lhs->ty->kind == TY_CHAR
    && is_integer_constant(node->rhs,&val)) {
      node->rhs = new_num(val,node->rhs->tok);
      node->rhs->ty = node->lhs->ty;
      node->ty = node->lhs->ty;
      return node;
    }
    if (node->lhs->ty->kind == TY_CHAR
    &&  is_integral_promotion(node->rhs)) {
      node->rhs = node->rhs->lhs;
      return node;
    }
    return node;
  case ND_STMT_EXPR:
    //for (Node *n = node->body; n; n = n->next)
    //  optimize_stmt(n);
    return node;
  case ND_COMMA: {
    Node *new =  optimize_lr(node);
    new->lhs->retval_unused = true;
    return new;
  }
  case ND_CAST: {
    if (node->ty->kind == TY_VOID) {
      node->lhs = optimize_expr(node->lhs);
      return node;
    }
    node->lhs = optimize_expr(node->lhs);
    // optimize_expr() may have replaced the operand with a wider one
    if (is_integer(node->lhs->ty) && is_integer(node->ty)
    &&  node->ty->kind != TY_BOOL
    &&  node->lhs->ty->size == node->ty->size
    &&  node->lhs->ty->is_unsigned == node->ty->is_unsigned) {
      return node->lhs;
    }
    if (node->lhs->kind == ND_CAST) {
      if (node->ty->kind == TY_PTR
      &&  node->lhs->ty->kind == TY_PTR) {
        node->lhs = node->lhs->lhs;
      }
    }
    if (node->ty->kind==TY_BOOL
    &&  is_boolean_result(node->lhs)){
      return node->lhs;
    }
    if (is_int8(node->ty)) {
      switch(node->lhs->kind) {
      case ND_LOGAND:
      case ND_LOGOR:
      case ND_NOT:
        node->lhs->ty = node->ty;
        return optimize_expr(node->lhs);
      }
    }
    // (int)float and (char)float may cause undefined behavior.
    if (node->ty->kind  == TY_CHAR
    &&  node->lhs->kind == ND_CAST
    &&  !is_flonum(node->lhs->lhs->ty)
    &&  (node->lhs->ty->kind == TY_SHORT
      || node->lhs->ty->kind == TY_INT
      || node->lhs->ty->kind == TY_LONG)) {
      node->lhs = node->lhs->lhs;
      return node;
    }
    // (ND_CAST TY_CHAR(2) (-/~ (ND_CAST TY_INT(4) (expr ...) )))
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_NEG || node->lhs->kind == ND_BITNOT)
    &&  node->lhs->lhs->kind == ND_CAST
    &&  node->lhs->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->lhs->ty->kind == TY_CHAR) {
      Node *new = new_unary(node->lhs->kind, node->lhs->lhs->lhs, node->tok);
      new->ty = node->ty;
      return new;
    }
    // int = char &|^ char; int = uchar &|^ uchar;
    if (is_integral_promotion(node)
    &&  (node->lhs->kind == ND_BITAND
      || node->lhs->kind == ND_BITOR
      || node->lhs->kind == ND_BITXOR)
    &&  is_integral_promotion(node->lhs->lhs)
    &&  is_integral_promotion(node->lhs->rhs)) {
      if (node->lhs->lhs->lhs->ty->is_unsigned == node->lhs->rhs->lhs->ty->is_unsigned) {
        node->lhs->lhs = node->lhs->lhs->lhs;
        node->lhs->rhs = node->lhs->rhs->lhs;
        return optimize_const_expr(node);
      }
    }
    // char = (char)((int)char &|^ (int)char);
    // char = (char)((int)uchar &|^ (int)uchar);
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_BITAND
      || node->lhs->kind == ND_BITOR
      || node->lhs->kind == ND_BITXOR)
    &&  is_integral_promotion(node->lhs->lhs)
    &&  is_integral_promotion(node->lhs->rhs)) {
      node->lhs->ty = node->lhs->lhs->lhs->ty;
      node->lhs->lhs = node->lhs->lhs->lhs;
      node->lhs->rhs = node->lhs->rhs->lhs;
      return optimize_const_expr(node);
    }

    // (ND_CAST TY_CHAR(2) (- ty_uint 6 (ND_CAST TY_INT(4) (ND_VAR ty_uchar _L_2 global))))
    // (ND_CAST TY_CHAR(2) (+ TY_INT(4) (ND_CAST TY_INT(4) (ND_VAR TY_CHAR(2) y0 +9 )) (ND_CAST TY_INT(4) (ND_VAR ty_uchar y +2 ))))
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB
      || node->lhs->kind == ND_MUL)
    &&  is_integral_promotion(node->lhs->lhs)
    &&  is_integral_promotion(node->lhs->rhs)) {
      Node *new = new_copy(node->lhs);
      new->lhs = skip_integral_promotion(node->lhs->lhs);
      new->rhs = skip_integral_promotion(node->lhs->rhs);
      new->ty = node->ty;
      return optimize_const_expr(new);
    }
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  is_byte_to_uint(node->lhs->lhs)
    &&  is_byte_to_uint(node->lhs->rhs)) {
      Node *new = new_copy(node->lhs);
      new->lhs = skip_byte_to_uint(node->lhs->lhs);
      new->rhs = skip_byte_to_uint(node->lhs->rhs);
      new->ty = node->ty;
      return optimize_const_expr(new);
    }
    if (node->ty->kind == TY_CHAR) {
      switch(node->lhs->kind) {
      case ND_ADD:
      case ND_SUB:
//    case ND_MUL:  // need test.
//    case ND_DIV:
//    case ND_MOD:
//    case ND_BITAND:
//    case ND_BITOR:
//    case ND_BITXOR:
        if (is_byte_to_int(node->lhs->lhs)
        &&  is_compare_or_not(node->lhs->rhs)) {
          node->lhs->lhs = skip_byte_to_int(node->lhs->lhs);
          node->lhs->rhs->ty = node->lhs->lhs->ty;
          node->lhs->ty      = node->lhs->lhs->ty;
          return optimize_const_expr(node->lhs);
        }
        if (is_byte_to_int(node->lhs->rhs)
        &&  is_compare_or_not(node->lhs->lhs)) {
          node->lhs->rhs = skip_byte_to_int(node->lhs->rhs);
          node->lhs->lhs->ty = node->lhs->rhs->ty;
          node->lhs->ty      = node->lhs->rhs->ty;
          return optimize_const_expr(node->lhs);
        }
        if (is_compare_or_not(node->lhs->lhs)
        &&  is_compare_or_not(node->lhs->rhs)) {
          node->lhs->lhs->ty = node->ty;
          node->lhs->rhs->ty = node->ty;
          node->lhs->ty      = node->ty;
          return optimize_const_expr(node->lhs);
        }
      }
    }

    // (ND_CAST TY_CHAR(2) (- ty_int 8 (ND_CAST TY_INT(4) (ND_VAR ty_uchar _L_35 global)))
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  node->lhs->lhs->kind == ND_NUM
    &&  (is_integral_promotion(node->lhs->rhs)
       ||is_byte_to_uint(node->lhs->rhs)) ) {
      Node *new = new_copy(node->lhs);
      new->ty = node->ty;
      new->lhs = node->lhs->lhs;
      new->lhs->ty = ty_char;
      new->rhs = node->lhs->rhs->lhs;
      node->lhs = new;
      return node;
    }
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB
      || node->lhs->kind == ND_MUL)
    &&  node->lhs->rhs->kind == ND_NUM
    &&  (is_integral_promotion(node->lhs->lhs)
       ||is_byte_to_uint(node->lhs->lhs)) ) {
      Node *new = new_copy(node->lhs);
      new->ty = node->ty;
      new->lhs = node->lhs->lhs->lhs;
      new->rhs = node->lhs->rhs;
      new->rhs->ty = ty_char;
      node->lhs = new;
      return node;
    }
    // (ND_CAST TY_CHAR(2) (<< TY_INT(4)
    if (node->ty->kind == TY_CHAR
    &&  node->lhs->kind == ND_SHL) {
      node->lhs->ty = node->ty;
      if (is_integral_promotion(node->lhs->lhs)) {
        node->lhs->lhs = node->lhs->lhs->lhs;
      }
      return optimize_expr(node->lhs);
    }
    // (ND_CAST TY_CHAR(2) (|&^ TY_INT(4) (int) (int)))
    if (node->ty->kind == TY_CHAR) {
      switch(node->lhs->kind) {
      case ND_BITAND:
      case ND_BITOR:
      case ND_BITXOR:
        if (is_integral_promotion_or_char(node->lhs->lhs)) {
          node->lhs->lhs = skip_integral_promotion(node->lhs->lhs);
        }
        if (is_integral_promotion_or_char(node->lhs->rhs)) {
          node->lhs->rhs = skip_integral_promotion(node->lhs->rhs);
        }
        if (!is_integral_promotion_or_char(node->lhs->lhs)) {
          node->lhs->lhs = new_cast(node->lhs->lhs,ty_char);
        }
        if (!is_integral_promotion_or_char(node->lhs->rhs)) {
          node->lhs->rhs = new_cast(node->lhs->rhs,ty_char);
        }
        node->lhs->ty = node->ty;
        return optimize_expr(node->lhs);
      }
    }
    // not for _Bool: (_Bool)(1^3) is 1, but (_Bool)1 ^ (_Bool)3 is 0
    if (node->ty->kind != TY_BOOL
    &&  node->ty->size < node->lhs->ty->size) {
      switch(node->lhs->kind) {
      case ND_BITAND:
      case ND_BITOR:
      case ND_BITXOR:
        node->lhs->lhs = new_cast(node->lhs->lhs,node->ty);
        node->lhs->rhs = new_cast(node->lhs->rhs,node->ty);
        node->lhs->ty = node->ty;
        return optimize_expr(node->lhs);
      }
    }
    if (node->ty->kind == TY_CHAR
    &&  node->lhs->kind == ND_COND) {
      node->lhs->ty = node->ty;
      node->lhs->then = optimize_expr(new_cast(node->lhs->then,node->ty));
      node->lhs->els  = optimize_expr(new_cast(node->lhs->els ,node->ty));
      return node->lhs;
    }
    if (is_flonum(node->ty)
    &&  node->lhs->kind==ND_NUM
    &&  is_integer(node->lhs->ty)) {
      Node *new = new_copy(node->lhs);
      new->fval = node->lhs->val;
      new->ty   = node->ty;
      return new;
    }
    return optimize_const_expr(node);
  } // ND_CAST
  case ND_MEMZERO:
    return node;
  case ND_COND:
    node->cond = optimize_condition(node->cond);
    node->then = optimize_expr(node->then);
    node->els  = optimize_expr(node->els);
    switch(node->then->kind) {
    case ND_CAST:
    case ND_COND:
      node->then->ty = node->ty;
      node->then = optimize_expr(node->then);
    }
    switch(node->els->kind) {
    case ND_CAST:
    case ND_COND:
      node->els->ty = node->ty;
      node->els = optimize_expr(node->els);
    }
    return node;
  case ND_FUNCALL:
    node->lhs = optimize_expr(node->lhs);
    Node *top = new_num(0,node->tok);
    top->next = node->args;
    node->args = optimize_args(top)->next;
    return node;
  case ND_LABEL_VAL:
    return node;
  // If the ND_NOT negates the result of a relational operator,
  // Rewrite !(node) into the negated related op.
  // when float compare, only == and != rewrited.
  case ND_NOT: {
    node->lhs = optimize_expr(node->lhs);
    node->lhs = skip_integral_promotion(node->lhs);
    if (can_negate_compare(node->lhs)) {
      node->kind = negate_kind(node->lhs->kind);
      node->rhs  = node->lhs->rhs;
      node->ty   = node->lhs->ty;
      node->lhs  = node->lhs->lhs; // must be last
      return optimize_expr(node);
    }
    if (node->lhs->kind == ND_NOT) {
      Node *new = new_cast(new_cast(node->lhs->lhs, ty_bool), node->ty);
      return optimize_expr(new);
    }
    if (is_integer_constant(node->lhs,&val)) {
      Node *new = new_num((val==0),node->tok);
      new->ty = ty_bool;
      return new;
    }
    if (is_flonum_constant(node->lhs,&fval)) {
      Node *new = new_num((fval==0.0),node->tok);
      new->ty = ty_bool;
      return new;
    }
    Node *new = optimize_demorgan(node->lhs);

    if (new) {
      new->ty = node->ty;
      return optimize_expr(new);
    }
    if (opt('O','s') && is_integer_or_ptr(node->lhs->ty)) {  // Smaller, but slower
      Node *zero = new_num(0,node->tok);

      zero->ty = node->lhs->ty;
      new = new_binary(ND_EQ, node->lhs, zero, node->tok);
      new->ty = node->ty;
      return new;
    }
    return optimize_const_expr(node);
  } // ND_NOT
  case ND_BITNOT:
    node = optimize_l(node);
    return optimize_const_expr(node);
  // Below is a binary operator
  case ND_LOGAND: {
    node = optimize_lr(node);
    node->lhs = skip_integral_promotion(node->lhs);
    node->rhs = skip_integral_promotion(node->rhs);
    if (is_integer_constant(node->lhs,&val)) {
      if (val==0) {
        node = new_num(0,node->tok);
        node->ty = ty_int;
        return node;
      }else if (is_integer_constant(node->rhs,&val)) {
        node = new_num((val!=0),node->tok);
        node->ty = ty_int;
        return node;
      }else{
        // C99 6.5.13p3: && yields int 0 or 1 (6.3.1.2: scalar to _Bool) 
        return optimize_expr(new_cast(new_cast(node->rhs,ty_bool),ty_int));
      }
    }
    return optimize_const_expr(node);
  }
  case ND_LOGOR:
    node = optimize_lr(node);
    node->lhs = skip_integral_promotion(node->lhs);
    node->rhs = skip_integral_promotion(node->rhs);
    if (is_integer_constant(node->lhs,&val)) {
      if (val!=0) {
        node = new_num(1,node->tok);
        node->ty = ty_int;
        return node;
      }else if (is_integer_constant(node->rhs,&val)) {
        node = new_num((val!=0),node->tok);
        node->ty = ty_int;
        return node;
      }else{
        // C99 6.5.14p3: || yields int 0 or 1 (6.3.1.2: scalar to _Bool) 
        return optimize_expr(new_cast(new_cast(node->rhs,ty_bool),ty_int));
      }
    }
    return optimize_const_expr(node);
  case ND_ADD: {
    int64_t val;
    int64_t val2;
    Node *new;
    Node *base;

    node = optimize_lr_swap(node);
    // a comparison gives 0 or 1, so the operation fits in a uchar
    if (is_int16(node->ty)
    &&  is_compare_or_not(node->lhs)
    &&  is_compare_or_not(node->rhs)) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs->ty = ty_uchar;
      new->rhs->ty = ty_uchar;
      return optimize_expr(new_cast(new,node->ty));
    }
    if (is_integer_constant(node->lhs,&val)
    &&  is_integer_constant(node->rhs,&val2)) {
      return optimize_const_expr(node);
    }
    if (is_integer_constant(node->rhs,&val)) {
      if (node->lhs->kind == ND_ADD
      &&  is_integer_constant(node->lhs->rhs,&val2)) {
        new = new_copy(node->lhs);
        new->rhs = new_num(val+val2,node->tok);
        new->rhs->ty = node->lhs->rhs->ty;
        return new;
      }
      if (node->lhs->kind == ND_SUB
      &&  is_integer_constant(node->lhs->rhs,&val2)) {
        new = new_copy(node->lhs);
        new->kind = ND_ADD;
        new->rhs = new_num(val-val2,node->tok);
        new->rhs->ty = node->lhs->rhs->ty;
        return new;
      }
    }
    // (+ arr (- V C)) -> (+ arr (+ V -C))
    if (is_global_array(node->rhs)
    &&  node->lhs->kind == ND_SUB
    &&  is_int16(node->lhs->ty)
    &&  is_integer_constant(node->lhs->rhs,&val)) {
      new = new_copy(node);
      new->lhs = new_copy(node->lhs);
      new->lhs->kind = ND_ADD;
      new->lhs->rhs = new_num(-val,node->tok);
      new->lhs->rhs->ty = node->lhs->rhs->ty;
      return optimize_expr(new);
    }
    // (+ arr (ND_CAST int (- V C))) -> (+ arr (ND_CAST int (+ V -C)))
    if (is_global_array(node->rhs)
    &&  node->lhs->kind == ND_CAST
    &&  is_int16(node->lhs->ty)
    &&  is_int16(node->lhs->lhs->ty)
    &&  node->lhs->lhs->kind == ND_SUB
    &&  is_integer_constant(node->lhs->lhs->rhs,&val)) {
      new = new_copy(node);
      new->lhs = new_copy(node->lhs);
      new->lhs->lhs = new_copy(node->lhs->lhs);
      new->lhs->lhs->kind = ND_ADD;
      new->lhs->lhs->rhs = new_num(-val,node->tok);
      new->lhs->lhs->rhs->ty = node->lhs->lhs->rhs->ty;
      return optimize_expr(new);
    }
    // (+ (ND_CAST TY_PTR (+ arr V)) C) -> (+ (ND_CAST TY_PTR (+ arr C)) V)
    if (is_integer_constant(node->rhs,&val)
    &&  (base = is_array_base(node->lhs))) {
      Node *arr  = is_global_array(base->lhs) ? base->lhs :
                   is_global_array(base->rhs) ? base->rhs : NULL;
      if (arr) {
        Node *idx = (arr == base->lhs) ? base->rhs : base->lhs;
        if (is_integer(idx->ty)
        && !is_integer_constant(idx,&val2)) {
          Node *add = new_copy(base);
          add->lhs = arr;
          add->rhs = node->rhs;
          new = new_copy(node);
          new->lhs = new_copy(node->lhs);
          new->lhs->lhs = add;
          new->rhs = idx;
          return new;
        }
      }
    }
    // (+ (ND_CAST TY_PTR (+ arr c1)) c2) -> (ND_CAST TY_PTR (+ arr (c1+c2)))
    if (is_integer_constant(node->rhs,&val)
    &&  (base = is_array_base(node->lhs))
    &&  is_integer_constant(base->rhs,&val2)) {
      Node *add  = new_copy(node->lhs->lhs);
      add->rhs   = new_num(val2+val,node->tok);
      add->rhs->ty = node->lhs->lhs->rhs->ty;
      new        = new_copy(node->lhs);
      new->lhs   = add;
      return new;
    }
    return node;
  } // ND_ADD
  case ND_SUB: {
    int64_t val;
    int64_t val2;
    Node *new;
    Node *base;

    node = optimize_lr(node);
    if (is_integer_constant(node->lhs,&val)
    &&  is_integer_constant(node->rhs,&val2)) {
      return optimize_const_expr(node);
    }
    if (is_integer_constant(node->rhs,&val)) {
      if (node->lhs->kind == ND_ADD
      &&  is_integer_constant(node->lhs->rhs,&val2)) {
        new = new_copy(node->lhs);
        new->rhs = new_num(val2-val,node->tok);
        new->rhs->ty = node->lhs->rhs->ty;
        return new;
      }
      if (node->lhs->kind == ND_SUB
      &&  is_integer_constant(node->lhs->rhs,&val2)) {
        new = new_copy(node->lhs);
        new->rhs = new_num(val+val2,node->tok);
        new->rhs->ty = node->lhs->rhs->ty;
        return new;
      }
    }
    // (- (ND_CAST TY_PTR (+ arr V)) C) -> (+ (ND_CAST TY_PTR (+ arr -C)) V)
    if (is_integer_constant(node->rhs,&val)
    &&  (base = is_array_base(node->lhs))) {
      Node *arr  = is_global_array(base->lhs) ? base->lhs :
                   is_global_array(base->rhs) ? base->rhs : NULL;
      Node *idx  = (arr == base->lhs) ? base->rhs : base->lhs;
      if (arr
      &&  is_integer(idx->ty)
      && !is_integer_constant(idx,&val2)) {
        Node *add = new_copy(base);
        add->lhs = arr;
        add->rhs = new_num(-val,node->tok);
        add->rhs->ty = node->rhs->ty;
        new = new_copy(node);
        new->kind = ND_ADD;
        new->lhs = new_copy(node->lhs);
        new->lhs->lhs = add;
        new->rhs = idx;
        return new;
      }
    }
    // (- (ND_CAST TY_PTR (+ arr c1)) c2) -> (ND_CAST TY_PTR (+ arr (c1-c2)))
    if (is_integer_constant(node->rhs,&val)
    &&  (base = is_array_base(node->lhs))
    &&  is_integer_constant(base->rhs,&val2)) {
      Node *add  = new_copy(node->lhs->lhs);
      add->rhs   = new_num(val2-val,node->tok);
      add->rhs->ty = node->lhs->lhs->rhs->ty;
      new        = new_copy(node->lhs);
      new->lhs   = add;
      return new;
    }
    return node;
  }
  case ND_MUL: {
    int64_t val;
    int64_t val2;

    node = optimize_lr_swap(node);
    if (is_integer_constant(node->lhs,NULL)
    &&  is_integer_constant(node->rhs,NULL)) {
      return optimize_const_expr(node);
    }
    // (x * c1) * c2 -> x * (c1 * c2)
    if (is_integer_constant(node->rhs,&val)
    &&  node->lhs->kind == ND_MUL
    &&  is_integer_constant(node->lhs->rhs,&val2)) {
      Node *new = new_copy(node->lhs);
      new->ty  = node->ty;
      new->rhs = new_num(fit_to_type((uint64_t)val*val2,node->ty), node->tok);
      new->rhs->ty = node->ty;
      return optimize_expr(new);
    }
    // (x ± c1) * c2 -> x * c2 ± c1 * c2  (arr[i+1] makes this tree)
    if (is_integer_constant(node->rhs,&val)
    && (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  is_integer_constant(node->lhs->rhs,&val2)) {
      Node *new = new_copy(node->lhs);
      new->ty  = node->ty;
      new->lhs = new_copy(node);
      new->lhs->lhs = node->lhs->lhs;
      new->lhs->rhs = node->rhs;
      new->rhs = new_num(fit_to_type((uint64_t)val*val2,node->ty), node->tok);
      new->rhs->ty = node->ty;
      return optimize_const_expr(new);
    }

    return node;
  } // ND_MUL
  case ND_DIV:
  case ND_MOD: {
    int64_t val;

    node = optimize_lr(node);
    if (is_integer_constant(node->lhs,NULL)
    &&  is_integer_constant(node->rhs,NULL)) {
      return optimize_const_expr(node);
    }
    // unsigned x / 2**n -> x >> n
    if (node->kind == ND_DIV
    &&  is_int16(node->ty)
    &&  (node->ty->is_unsigned
      || skip_byte_to_int(node->lhs)->ty->is_unsigned)
    &&  is_integer_constant(node->rhs,&val)
    &&  exact_log2(val) > 0) {
      Node *new = new_copy(node);
      new->kind = ND_SHR;
      new->rhs  = new_num(exact_log2(val),node->rhs->tok);
      new->rhs->ty = node->rhs->ty;
      return optimize_expr(new);
    }
    // unsigned x % 2**n -> x & (2**n -1)
    if (node->kind == ND_MOD
    &&  (node->ty->is_unsigned
      || skip_byte_to_int(node->lhs)->ty->is_unsigned)
    &&  is_integer_constant(node->rhs,&val)
    &&  exact_log2(val) >= 0) {
      Node *new = new_copy(node);
      new->kind = ND_BITAND;
      new->rhs  = new_num(val-1,node->rhs->tok);
      new->rhs->ty = node->rhs->ty;
      return optimize_expr(new);
    }
    // both sides are 0..255, so the quotient fits in a uchar
    if (node->kind == ND_DIV
    &&  is_int16(node->ty)
    &&  can_op_uchar(node->lhs)
    &&  is_integer_constant(node->rhs,&val)
    &&  1 <= val && val <= 255) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs = new_cast(node->lhs,ty_uchar);
      new->rhs = new_cast(node->rhs,ty_uchar);
      return optimize_expr(new_cast(new,node->ty));
    }

    return node;
  } // ND_SUB,MUL,DIV,MOD
  case ND_BITAND: {
    int64_t val;

    node = optimize_lr_swap(node);
    // both sides are 0..255, so the bit operation fits in a uchar
    if (is_int16(node->ty)
    &&  can_op_uchar(node->lhs)
    &&  can_op_uchar(node->rhs)) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs = new_cast(node->lhs,ty_uchar);
      new->rhs = new_cast(node->rhs,ty_uchar);
      return optimize_expr(new_cast(new,node->ty));
    }
    node = optimize_bitop_integral_promotion(node);

    // x & 0..255 -> (T)((uchar)x & 0..255)
    if (node->kind == ND_BITAND
    &&  is_int16(node->ty)
    &&  is_integer_constant(node->rhs,&val)
    &&  0 <= val && val <= 255) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs = new_cast(node->lhs,ty_uchar);
      return optimize_expr(new_cast(new,node->ty));
    }
    return node;
  }
  case ND_BITOR:
  case ND_BITXOR:
    node = optimize_lr_swap(node);
    // both sides are 0..255, so the bit operation fits in a uchar
    if (is_int16(node->ty)
    &&  can_op_uchar(node->lhs)
    &&  can_op_uchar(node->rhs)) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs = new_cast(node->lhs,ty_uchar);
      new->rhs = new_cast(node->rhs,ty_uchar);
      return optimize_expr(new_cast(new,node->ty));
    }
    return optimize_bitop_integral_promotion(node);
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE: {
    int64_t val;

    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);

    if (node_cost(node->lhs) < node_cost(node->rhs)) {
      node = flip_condition(swap_lr(node));
    }
//
// For signed integer,
//   x+1 invokes UB at INT_MAX,
//   x-1 invokes UB at INT_MIN.
// Assuming no UB, fold comparisons involving x +/- 1.
//
// Unsigned wraparound changes the semantics.
//
//  lhs -1 <  rhs → lhs<=rhs
//  lhs +1 <= rhs → lhs< rhs
    if (!opt_fwrapv) {
      if ( is_integer(node->lhs->ty) && !node->lhs->ty->is_unsigned) {
        if ((node->kind==ND_LT && node->lhs->kind==ND_SUB)
        ||  (node->kind==ND_LE && node->lhs->kind==ND_ADD)) {
          if(is_integer_constant(node->lhs->rhs,&val) &&  val==1 ){
            node->kind = (node->kind==ND_LT)?ND_LE:ND_LT;
            node->lhs = optimize_expr(node->lhs->lhs);
          }
  //  lhs -1 >= rhs → lhs>  rhs
  //  lhs +1 >  rhs → lhs>= rhs
        }else if ((node->kind==ND_GE && node->lhs->kind==ND_SUB)
        ||  (node->kind==ND_GT && node->lhs->kind==ND_ADD)) {
          if(is_integer_constant(node->lhs->rhs,&val) &&  val==1 ){
            node->kind = (node->kind==ND_GE)?ND_GT:ND_GE;
            node->lhs = optimize_expr(node->lhs->lhs);
          }
        }
      }
//  lhs <  rhs +1 → lhs<=rhs
//  lhs <= rhs -1 → lhs< rhs
      if ( is_integer(node->rhs->ty) && !node->rhs->ty->is_unsigned) {
        if ((node->kind==ND_LT && node->rhs->kind==ND_ADD)
        ||  (node->kind==ND_LE && node->rhs->kind==ND_SUB)) {
          if(is_integer_constant(node->rhs->rhs,&val) &&  val==1 ){
            node->kind = (node->kind==ND_LT)?ND_LE:ND_LT;
            node->rhs = optimize_expr(node->rhs->lhs);
          }
  //  lhs >= rhs +1 → lhs>  rhs
  //  lhs >  rhs -1 → lhs>= rhs
        }else if ((node->kind==ND_GE && node->rhs->kind==ND_ADD)
        ||  (node->kind==ND_GT && node->rhs->kind==ND_SUB)) {
          if(is_integer_constant(node->rhs->rhs,&val) &&  val==1 ){
            node->kind = (node->kind==ND_GE)?ND_GT:ND_GE;
            node->rhs = optimize_expr(node->rhs->lhs);
          }
        }
      }
    }
    if (is_integral_promotion(node->lhs)
    &&  is_integral_promotion(node->rhs)
    &&  node->lhs->lhs->ty->is_unsigned == node->rhs->lhs->ty->is_unsigned) {
      node->lhs = skip_integral_promotion(node->lhs);
      node->rhs = skip_integral_promotion(node->rhs);
    }
    if (is_integral_promotion(node->lhs)
    &&  node->rhs->kind==ND_NUM) {
      if (is_uchar_or_u8num(node->lhs->lhs)
       && is_uchar_or_u8num(node->rhs)) {
        node->lhs = skip_integral_promotion(node->lhs);
        node->rhs = skip_integral_promotion(node->rhs);
        if (node->lhs->kind == ND_NUM) node->lhs->ty = ty_uchar;
        if (node->rhs->kind == ND_NUM) node->rhs->ty = ty_uchar;
      }else if (is_schar_or_s8num(node->lhs->lhs)
             && is_schar_or_s8num(node->rhs)){
        node->lhs = skip_integral_promotion(node->lhs);
        node->rhs = skip_integral_promotion(node->rhs);
        if (node->lhs->kind == ND_NUM) node->lhs->ty = ty_char;
        if (node->rhs->kind == ND_NUM) node->rhs->ty = ty_char;
      }
    }
    // uchar op 8 bit constant
    if (is_byte_to_uint(node->lhs)
    &&  node->rhs->kind == ND_NUM) {
      if (is_uchar_or_u8num(node->lhs->lhs)
       && is_uchar_or_u8num(node->rhs)) {
        node->lhs = skip_byte_to_uint(node->lhs);
        if (node->lhs->kind == ND_NUM) node->lhs->ty = ty_uchar;
        node->rhs->ty = ty_uchar;
      }
    }
    //  x <= C -> x <  C+1
    //  x >  C -> x >= C+1
    if ((node->kind==ND_LE || node->kind==ND_GT)
    &&  is_integer_constant(node->rhs,&val)
    &&  val != 0
    &&  val < ty_max_value(node->lhs->ty)) {
      node->rhs->val++;
      node->kind = (node->kind==ND_LE)? ND_LT: ND_GE;
    }

    if (node->kind == ND_LE || node->kind == ND_GT) {
      if (node->lhs->ty->kind != TY_CHAR
      && ( node_cost(node->lhs) == node_cost(node->rhs)
      ||   test_addr_x(node->lhs)
      || (is_addr_constant(node->lhs)!=NULL))) {
        node = flip_condition(swap_lr(node));
      }
    }
    return optimize_const_expr(node);
  } // relative op
  case ND_SHL:
  case ND_SHR: {
    int64_t val;
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(new_cast(node->rhs,ty_char));

    if (is_integer_constant(node->rhs,&val)) {
      if (val==0) {
        return node->lhs;
      } else if (val<0
      ||  val>=(node->ty->size)*8) { // TODO: bit field
//      warn_tok(node->tok,"shift count negative or too big, undefined behavior");
        Node *new = new_num(0,node->tok);
        new->ty   = node->ty;
        return new;
      }
    }
    // (cmp) << 0..7 -> (T)((uchar)cmp << 0..7)
    if (node->kind == ND_SHL
    &&  is_int16(node->ty)
    &&  is_compare_or_not(node->lhs)
    &&  is_integer_constant(node->rhs,&val)
    &&  0 <= val && val <= 7) {
      Node *new = new_copy(node);
      new->ty  = ty_uchar;
      new->lhs->ty = ty_uchar;
      return optimize_expr(new_cast(new,node->ty));
    }
// (>> TY_INT(4) (ND_CAST TY_INT(4) (ND_VAR ty_uchar _L_1 global)) (ND_CAST TY_CHAR(2) (ND_VAR ty_int _L_5 global)))
    if (node->kind == ND_SHR
    &&  node->ty->kind == TY_INT
    &&  is_integral_promotion(node->lhs)) {
      Node *n1 = new_copy(node);
      n1->lhs = node->lhs->lhs;
      n1->rhs = node->rhs;
      n1->ty = node->lhs->lhs->ty;
      Node *n2 = new_copy(node->lhs);
      n2->lhs = n1;
      return n2;
    }
// (<< TY_CHAR(2) (ND_CAST TY_INT(4) (ND_VAR ty_uchar _L_36 global)) (ND_CAST ty_char (- ty_char (ND_NUM ty_char 8) (ND_VAR ty_uchar _L_35 global))))
    return optimize_const_expr(node);
  } // ND_SHL, ND_SHR
  case ND_POST_INCDEC:
  case ND_PRE_INCDEC:
    return optimize_lr(node);
  case ND_ADDEQ:
  case ND_SUBEQ:
    node = optimize_lr(node);
    return optimize_const_expr(node);
  case ND_MULEQ:
  case ND_DIVEQ:
  case ND_MODEQ:
  case ND_ANDEQ:
  case ND_OREQ:
  case ND_XOREQ:
    node = optimize_lr(node);
    return optimize_const_expr(node);
  case ND_SHLEQ:
  case ND_SHREQ: {
    int64_t val;
    node = optimize_lr(node);
    if (is_integer_constant(node->rhs,&val)) {
      if (val==0) {
        return node->lhs;
      } else if (val<0 || val >= (node->ty->size)*8) {
//      warn_tok(node->tok,"shift count negative/too big, undefined behavior");
        Node *new = new_num(0,node->tok);
        new->ty   = node->ty;
        new =  new_binary(ND_ASSIGN, node->lhs, new, node->tok);
        new->ty = node->ty;
        return new;
      }
    }
    return optimize_const_expr(node);
  }
  case ND_BULKINIT:
    return node;
  }
  return node;
}

Node *optimize_condition(Node *node)
{
  int64_t val;

  if (!node)
    return node;

  node = optimize_expr(node);

  switch (node->kind) {
  case ND_LOGAND:
  case ND_LOGOR:
    node->lhs = optimize_condition(node->lhs);
    node->rhs = optimize_condition(node->rhs);
    break;
  }
  // !!x -> x
  if (node->kind == ND_NOT && node->lhs->kind == ND_NOT) {
    node = optimize_condition(node->lhs->lhs);
  }
  // x==0 -> !x
  if (node->kind==ND_EQ
  &&  is_integer_or_ptr(node->lhs->ty)
  &&  is_integer_constant(node->rhs,&val)
  &&  val==0 ) {
    Token *tok = node->tok;
    node = optimize_condition(node->lhs);
    if (node->kind == ND_NOT) {
      node = node->lhs;       // !!x -> x
    }else{
      node = new_unary(ND_NOT, node, tok);
     node->ty = ty_bool;
    }
  }
  // x != 0 -> x
  if (node->kind==ND_NE
  &&  is_integer_or_ptr(node->lhs->ty)
  &&  is_integer_constant(node->rhs,&val)
  &&  val==0 ) {
    node = optimize_condition(node->lhs);
  }
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_BOOL) {
    node = optimize_condition(node->lhs);
  }
  // In conditions, drop cast to wider integer types.
  // The boolean result is the same, but it costs more.
  if (node->kind == ND_CAST
  &&  is_integer(node->ty)
  &&  is_integer(node->lhs->ty)
  &&  node->ty->size >= node->lhs->ty->size) {
    node = node->lhs;
  }

  return node;
}


Obj *optimize(Obj *prog)
{
	return prog;
}

