#include "chibicc.h"

bool is_integral_promotion(Node *node)
{
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_INT
  && !node->ty->is_unsigned
  &&  is_int8(node->lhs->ty)) {
    return true;
  }
  return false;
}

bool is_integral_promotion_or_byte(Node *node)
{
  if (is_integral_promotion(node)) {
    return true;
  }
  if (is_int8(node->ty)) {
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

bool is_uint_promotion(Node *node)
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

Node *skip_uint_promotion(Node *node)
{
  if (is_uint_promotion(node)) {
    return node->lhs;
  }
  return node;
}

bool is_u8num(Node *node)
{
  int64_t val;

  if (node->kind == ND_NUM && is_integer_constant(node, &val)) {
    if (val >= 0  && val <= 255) {
      return true;
    }
  }
  return false;
}

bool is_s8num(Node *node)
{
  int64_t val;

  if (node->kind == ND_NUM && is_integer_constant(node, &val)) {
    if (val >= -128  && val <= 127) {
      return true;
    }
  }
  return false;
}

bool is_8num(Node *node, Type *ty)
{
  if (!is_int8(ty)) {
    return false;
  }
  return ty->is_unsigned? is_u8num(node): is_s8num(node);
}

bool is_uchar_or_unum(Node *node)
{
  if (node->ty->kind == TY_CHAR && node->ty->is_unsigned)
    return true;
  if (is_u8num(node))
    return true;

  return false;
}

bool is_schar_or_snum(Node *node)
{
  if (node->ty->kind == TY_CHAR && !node->ty->is_unsigned)
    return true;
  if (is_u8num(node))
    return true;

  return false;
}


Node *negate_condition(Node *node)
{
  switch(node->kind){
  case ND_EQ:
    node->kind = ND_NE;
    break;
  case ND_NE:
    node->kind = ND_EQ;
    break;
  case ND_LT:
    node->kind = ND_GE;
    break;
  case ND_LE:
    node->kind = ND_GT;
    break;
  case ND_GT:
    node->kind = ND_LE;
    break;
  case ND_GE:
    node->kind = ND_LT;
    break;
  default:
    assert(0);
  }
  return node;
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
  }else if (node->kind == ND_CAST
        &&  node->ty->kind == TY_PTR
        &&  node->lhs->kind == ND_NUM) {
    return 5;
  }else if (is_global_var(node)) {
    return 150+sign;
  }else if (can_direct(node)) {
    return 200+sign;
  }else if (is_addr_constant(node)) {
    return 220+sign;
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

void make_comma_retval_unused(Node *node, bool all)
{
  for ( ; node ; node=node->rhs) {
    if (node->rhs || all) {
      node->lhs->retval_unused = true;
      if (node->lhs->kind == ND_COMMA) {
        make_comma_retval_unused(node->lhs,all);
      }
    }
  }
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
  if (!is_integral_promotion_or_byte(node->lhs)) {
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
      new->rhs = node->rhs;
      new->rhs->ty = ty_uchar;
      new = new_cast(new,ty_int);
      return optimize_const_expr(new);
    }
  }
  // char op -128..127
  //    (int)char op -128..127 -> (int)(uchar op 0..255)
  if (!skip_integral_promotion(node->lhs)->ty->is_unsigned) {
    if (node->rhs->kind           == ND_NUM
    &&  node->rhs->ty->kind       == TY_INT
    &&  node->rhs->val            >= -128
    &&  node->rhs->val            <= 127) {
      Node *new = new_copy(node);
      new->ty  = ty_char;
      new->lhs = skip_integral_promotion(node->lhs);
      new->rhs = node->rhs;
      new->rhs->ty = ty_uchar;
      new = new_cast(new,ty_int);
      return optimize_const_expr(new);
    }
  }
  // char op char, uchar op uchar
  //    (int)lhs op (int)rhs -> (int)(lhs op rhs)
  if (is_integral_promotion_or_byte(node->rhs)
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
  &&  is_integral_promotion_or_byte(node->rhs)
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
  Node *new;
  int64_t val;
  double fval;

  if (!node)
    return node;

  switch (node->kind) {
  case ND_NULL_EXPR:
  case ND_NUM:
    return node;
  case ND_NEG:
    node = optimize_l(node);
    if (is_integer_constant(node->lhs,&val)) {
      return optimize_const_expr(node);
    }else if (node->lhs->kind==ND_NUM
    &&  node->lhs->ty->kind == TY_FLOAT){
      Node *new = new_copy(node->lhs);
      new->fval = -node->lhs->fval;
      return new;
    }
    return node;
  case ND_VAR:
    if (!node->ty) {
      fprintf(stderr,"; need debug %s %d\n",__FILE__,__LINE__);
      return node;
    }
    switch (node->ty->kind) {
    case TY_ARRAY:
    case TY_STRUCT:
    case TY_UNION:
    case TY_FUNC:
    case TY_VLA:
      return node;
    }
    return node;
  case ND_MEMBER:
    return node;
  case ND_DEREF:
    node->lhs = optimize_expr(node->lhs);
    return node;
  case ND_ADDR:
    node->lhs = optimize_expr(node->lhs);
    if (node->lhs->kind == ND_DEREF)
      return node->lhs->lhs;
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
      node->rhs = new_num(val&255,node->rhs->tok);
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
  case ND_COMMA:
    new =  optimize_lr(node);
    new->lhs->retval_unused = true;
    return new;
  case ND_CAST: {
    if (node->ty->kind == TY_VOID) {
      node->lhs = optimize_expr(node->lhs);
      return node;
    }
    node->lhs = optimize_expr(node->lhs);
    if (node->lhs->kind == ND_CAST) {
      if (node->ty->kind == TY_PTR
      &&  node->lhs->ty->kind == TY_PTR) {
        node = node->lhs;
      }
    }
    if (node->ty->kind==TY_BOOL
    &&  is_boolean_result(node->lhs)){
      return node->lhs;
    }
    if (node->ty->kind == TY_CHAR
    ||  node->ty->kind == TY_BOOL) {
      switch(node->lhs->kind) {
      case ND_LOGAND:
      case ND_LOGOR:
      case ND_NOT:
        node->lhs->ty = node->ty;
        return optimize_expr(node->lhs);
      }
    }
    if (node->lhs->kind == ND_CAST
    &&  node->ty == node->lhs->ty) {
      return node->lhs;
    }
    if (node->ty->kind  == TY_CHAR
    &&  node->lhs->kind == ND_CAST
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
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
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
    &&  is_uint_promotion(node->lhs->lhs)
    &&  is_uint_promotion(node->lhs->rhs)) {
      Node *new = new_copy(node->lhs);
      new->lhs = skip_uint_promotion(node->lhs->lhs);
      new->rhs = skip_uint_promotion(node->lhs->rhs);
      new->ty = node->ty;
      return optimize_const_expr(new);
    }
    // (ND_CAST TY_CHAR(2) (- ty_int 8 (ND_CAST TY_INT(4) (ND_VAR ty_uchar _L_35 global)))
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  node->lhs->lhs->kind == ND_NUM
    &&  (is_integral_promotion(node->lhs->rhs)
       ||is_uint_promotion(node->lhs->rhs)) ) {
      Node *new = new_copy(node->lhs);
      new->ty = node->ty;
      new->lhs = node->lhs->lhs;
      new->lhs->ty = ty_char;
      new->rhs = node->lhs->rhs->lhs;
      node->lhs = new;
      return node;
    }
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  is_integral_promotion(node->lhs->lhs)
    &&  node->lhs->rhs->kind == ND_NUM) {
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
      return node->lhs;
    }
    // (ND_CAST TY_CHAR(2) (|&^ TY_INT(4) (int) (int)))
    if (node->ty->kind == TY_CHAR) {
      switch(node->lhs->kind) {
      case ND_BITAND:
      case ND_BITOR:
      case ND_BITXOR:
        if (is_integral_promotion_or_byte(node->lhs->lhs)) {
          node->lhs->lhs = skip_integral_promotion(node->lhs->lhs);
        }
        if (is_integral_promotion_or_byte(node->lhs->rhs)) {
          node->lhs->rhs = skip_integral_promotion(node->lhs->rhs);
        }
        if (!is_integral_promotion_or_byte(node->lhs->lhs)) {
          node->lhs->lhs = new_cast(node->lhs->lhs,ty_char);
        }
        if (!is_integral_promotion_or_byte(node->lhs->rhs)) {
          node->lhs->rhs = new_cast(node->lhs->rhs,ty_char);
        }
        node->lhs->ty = node->ty;
        return optimize_expr(node->lhs);
      }
    }
    if (node->ty->size < node->lhs->ty->size) {
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
#if 0
    if (is_integer(node->ty)
    &&  node->lhs->kind==ND_NUM
    &&  is_integer(node->lhs->ty)) {
      Node *new = new_copy(node->lhs);
      uint64_t val = node->lhs->val;
      switch (node->ty->size) {
      case 1:
        val =  node->ty->is_unsigned ? (uint8_t)val : (int8_t)val;
        break;
      case 2:
        val =  node->ty->is_unsigned ? (uint16_t)val : (int16_t)val;
        break;
      case 4:
        val =  node->ty->is_unsigned ? (uint32_t)val : (int32_t)val;
        break;
      }
      new->val = val;
      new->ty  = node->ty;
      return new;
    }
#endif
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
    node->cond = optimize_expr(node->cond);
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
  // rewrite the relational operator.
  // In the case of float, rewriting is not possible because there is NaN.
  case ND_NOT:
    node->lhs = skip_integral_promotion(node->lhs);
    if (is_compare(node->lhs)
    &&  is_integer(node->lhs->lhs->ty)
    &&  is_integer(node->lhs->rhs->ty)){
      return negate_condition(optimize_lr(node->lhs));
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
    return optimize_const_expr(node);
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
        return node->rhs;
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
        return node->rhs;
      }
    }
    return optimize_const_expr(node);
  case ND_ADD: {
    int64_t val;
    int64_t val2;
    Node *new;

    node = optimize_lr_swap(node);
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
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->lhs,&val)
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->rhs->lhs,&val2)) {
      new = new_copy(node->lhs);
      new->lhs = new_num(val+val2,node->tok);
      new->lhs->ty = node->lhs->lhs->ty;
      return new;
    }
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->lhs,&val)
    &&  is_integer_constant(node->rhs,&val2)) {
      new = new_copy(node->lhs);
      new->lhs = new_num(val+val2,node->rhs->tok);
      new->lhs->ty = node->rhs->ty;
      return new;
    }

    return node;
  } // ND_ADD
  case ND_SUB: {
    int64_t val;
    int64_t val2;
    Node *new;

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
    if (node->lhs->kind == ND_CAST
    &&  node->lhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->lhs->lhs,&val)
    &&  node->rhs->kind == ND_CAST
    &&  node->rhs->ty->kind == TY_PTR
    &&  is_integer_constant(node->rhs->lhs,&val2)) {
      new = new_copy(node->lhs);
      new->lhs = new_num(val-val2,node->tok);
      new->lhs->ty = node->lhs->lhs->ty;
      return new;
    }
    return node;
  }
  case ND_MUL: {
    int64_t val;

    node = optimize_lr_swap(node);
    if (is_integer_constant(node->lhs,&val)
    &&  is_integer_constant(node->rhs,&val)) {
      return optimize_const_expr(node);
    }

    return node;
  } // ND_MUL
  case ND_DIV:
  case ND_MOD: {
    int64_t val;

    node = optimize_lr(node);
    if (is_integer_constant(node->lhs,&val)
    &&  is_integer_constant(node->rhs,&val)) {
      return optimize_const_expr(node);
    }

    return node;
  } // ND_SUB,MUL,DIV,MOD
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    node = optimize_lr_swap(node);
    return optimize_bitop_integral_promotion(node);
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE: {
    int64_t val;
    int64_t lval, rval;

    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);
    if (node_cost(node->lhs) < node_cost(node->rhs)) {
      node = flip_condition(swap_lr(node));
    }
    if (is_integer_constant(node->lhs,&lval)
    &&  is_integer_constant(node->rhs,&rval)) {
      if ((node->lhs->ty == ty_int) && (node->rhs->ty == ty_int)) {
        switch(node->kind) {
        case ND_EQ:
          node = new_num(lval==rval,node->tok);
          node->ty = ty_int;
          return node;
        case ND_NE:
          node = new_num(lval!=rval,node->tok);
          node->ty = ty_int;
          return node;
        case ND_LT:
          node = new_num(lval<rval,node->tok);
          node->ty = ty_int;
          return node;
        case ND_LE:
          node = new_num(lval<=rval,node->tok);
          node->ty = ty_int;
          return node;
        case ND_GT:
          node = new_num(lval>rval,node->tok);
          node->ty = ty_int;
          return node;
        case ND_GE:
          node = new_num(lval>=rval,node->tok);
          node->ty = ty_int;
          return node;
        }
      }
    }
    if (is_integral_promotion(node->lhs)
    &&  is_integral_promotion(node->rhs)
    &&  node->lhs->lhs->ty->is_unsigned == node->rhs->lhs->ty->is_unsigned) {
      node->lhs = node->lhs->lhs;
      node->rhs = node->rhs->lhs;
    }
    if (is_integral_promotion(node->lhs)
    &&  is_8num(node->rhs,node->lhs->lhs->ty)) {
      node->rhs->ty = node->lhs->lhs->ty;
      node->lhs = node->lhs->lhs;
    }
    if ( is_integer(node->ty)
    &&  node->kind==ND_LE
    && is_integer_constant(node->rhs,&val)) {
      switch(node->ty->kind){
      case TY_CHAR:
        if (( node->ty->is_unsigned && node->rhs->val < UINT8_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT8_MAX)) {
          node->rhs->val++;
          node->kind = ND_LT;
        }
        break;
      case TY_SHORT:
      case TY_INT:
        if (( node->ty->is_unsigned && node->rhs->val < UINT16_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT16_MAX)) {
          if (node->rhs->val!=0) {
            node->rhs->val++;
            node->kind = ND_LT;
          }
        }
        break;
      case TY_LONG:
        if (( node->ty->is_unsigned && node->rhs->val < UINT32_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT32_MAX)) {
          node->rhs->val++;
          node->kind = ND_LT;
        }
        break;
      }
    }
    if ( is_integer(node->ty)
    &&  node->kind==ND_GT
    && is_integer_constant(node->rhs,&val)) {
      switch(node->ty->kind){
      case TY_CHAR:
        if (( node->ty->is_unsigned && node->rhs->val < UINT8_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT8_MAX)) {
          node->rhs->val++;
          node->kind = ND_GE;
        }
        break;
      case TY_SHORT:
      case TY_INT:
        if (( node->ty->is_unsigned && node->rhs->val < UINT16_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT16_MAX)) {
          if (node->rhs->val!=0) {
            node->rhs->val++;
            node->kind = ND_GE;
          }
        }
        break;
      case TY_LONG:
        if (( node->ty->is_unsigned && node->rhs->val < UINT32_MAX)
        ||  (!node->ty->is_unsigned && node->rhs->val < INT32_MAX)) {
          node->rhs->val++;
          node->kind = ND_GE;
        }
        break;
      }
    }  
//  lhs -1 <  rhs → lhs<=rhs
//  lhs +1 <= rhs → lhs< rhs
//  signed integer: UB, unsigned integer: modulo
    if ( is_integer(node->ty) && !node->lhs->ty->is_unsigned) {
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
    if ( is_integer(node->ty) && !node->rhs->ty->is_unsigned) {
      if ((node->kind==ND_LT && node->rhs->kind==ND_ADD)
      ||  (node->kind==ND_LE && node->rhs->kind==ND_SUB)) {
        if(is_integer_constant(node->rhs->rhs,&val) &&  val==1 ){
          node->kind = (node->kind==ND_LT)?ND_LE:ND_LT;
          node->rhs = optimize_expr(node->rhs->lhs);
        }
//  lhs >= rhs +1 → lhs>  rhs
//  lhs >  rhs -1 → lhs>= rhsA
      }else if ((node->kind==ND_GE && node->rhs->kind==ND_ADD)
      ||  (node->kind==ND_GT && node->rhs->kind==ND_SUB)) {
        if(is_integer_constant(node->rhs->rhs,&val) &&  val==1 ){
          node->kind = (node->kind==ND_GE)?ND_GT:ND_GE;
          node->rhs = optimize_expr(node->rhs->lhs);
        }
      }
    }

//  println("; optimize RO %d cost:%d %d",node->kind,node_cost(node->lhs),node_cost(node->rhs));
    if (node->ty->kind != TY_CHAR) {
      if ( node_cost(node->lhs) == node_cost(node->rhs)
      ||   test_addr_x(node->lhs)
      || (is_addr_constant(node->lhs)!=NULL)) {
        switch(node->kind){
        case ND_LE:
        case ND_GT:
          node = flip_condition(swap_lr(node));
          return optimize_const_expr(node);
        }
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
      } else if (val<0) {
//      warn_tok(node->tok,"shift count negative, undefined behavior");
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

  if (node->kind == ND_NOT && node->lhs->kind == ND_NOT) {
    node = optimize_condition(node->lhs->lhs);
  }
  if (node->kind == ND_NOT && is_compare(node->lhs)) {
    node = negate_condition(optimize_condition(node->lhs));
  }
  if (node->kind==ND_EQ
  &&  is_integer_or_ptr(node->lhs->ty)
  &&  is_integer_constant(node->rhs,&val)
  &&  val==0 ) {
    node = new_unary(ND_NOT, optimize_condition(node->lhs), node->tok);
    node->ty = ty_bool;
  }
  if (node->kind==ND_NE
  &&  is_integer_or_ptr(node->lhs->ty)
  &&  is_integer_constant(node->rhs,&val)
  &&  val==0 ) {
    node = optimize_condition(node->lhs);
  }
  if (node->kind == ND_CAST
  &&  node->ty->kind == TY_INT
  &&  (node->lhs->ty->kind == TY_INT || node->lhs->ty->kind == TY_CHAR)) {
    node = node->lhs;
  }

  return node;
}


Obj *optimize(Obj *prog)
{
	return prog;
}

