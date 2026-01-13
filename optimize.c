#include "chibicc.h"

static char *type_str(Node *node)
{
  if (!node || !node->ty) return "NULL";

  if (node->ty == ty_void) return "ty_void";
  if (node->ty == ty_bool) return "ty_bool";
  if (node->ty == ty_char) return "ty_char";
  if (node->ty == ty_short) return "ty_short";
  if (node->ty == ty_int) return "ty_int";
  if (node->ty == ty_long) return "ty_long";
  if (node->ty == ty_uchar) return "ty_uchar";
  if (node->ty == ty_ushort) return "ty_ushort";
  if (node->ty == ty_uint) return "ty_uint";
  if (node->ty == ty_ulong) return "ty_ulong";
  if (node->ty == ty_float) return "ty_float";
  if (node->ty == ty_double) return "ty_double";
  if (node->ty == ty_ldouble) return "ty_ldouble";

  switch(node->ty->kind){
  case TY_VOID:	return "TY_VOID(0)";
  case TY_BOOL: return "TY_BOOL(1)";
  case TY_CHAR: return "TY_CHAR(2)";
  case TY_SHORT:return "TY_SHORT(3)";
  case TY_INT:  return "TY_INT(4)";
  case TY_LONG: return "TY_LONG(5)";
  case TY_FLOAT:return "TY_FLOAT(6)";
  case TY_DOUBLE:return "TY_DOUBLE(7)";
  case TY_LDOUBLE:return "TY_LDOUBLE(8)";
  case TY_ENUM: return "TY_ENUM(9)";
  case TY_PTR:  return "TY_PTR(10)";
  case TY_FUNC: return "TY_FUNC(11)";
  case TY_ARRAY:return "TY_ARRAY(12)";
  case TY_VLA:  return "TY_VLA(13)";
  case TY_STRUCT:return "TY_STRUCT(14)";
  case TY_UNION:return "TY_UNION(15)";
  }

  assert(0);
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
  int  sign = !node->ty->is_unsigned;

  if (node->kind==ND_NUM) {
    return 1;
  }else if (is_global_var(node)) {
    return 150+sign;
  }else if (test_addr_x(node)) {
    return 200+sign;
  }else if (node->kind==ND_VAR) {
    return 250+sign;
  }else if (node->kind==ND_CAST) {
    return node_cost(node->lhs)+10+sign;
  }else if (node->kind==ND_FUNCALL) {
    return 400;
  }

  return 1000;
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
    int64_t val;

    if (is_const_expr(node)) {
      val = eval(node);
      Node *new = new_node(ND_NUM, node->tok);
      new->val = val;
      new->ty = node->lhs->ty;
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
    return node;
  case ND_ASSIGN:
    node = optimize_lr(node);
    if(node->rhs->ty->kind == TY_BOOL
    || node->rhs->ty->kind == TY_CHAR) {
      if (is_integer_constant(node->rhs,&val)) {
        node->rhs->ty = node->lhs->ty;
      }
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
    if (node->ty->kind  == TY_CHAR
    &&  node->lhs->kind == ND_CAST
    &&  (node->lhs->ty->kind == TY_SHORT
      || node->lhs->ty->kind == TY_INT
      || node->lhs->ty->kind == TY_LONG)) {
      node->lhs = node->lhs->lhs;
    }
    // (ND_CAST TY_CHAR(2) (- (ND_CAST TY_INT(4) (expr ...) )))
    if (node->ty->kind == TY_CHAR
    &&  node->lhs->kind == ND_NEG
    &&  node->lhs->lhs->kind == ND_CAST
    &&  node->lhs->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->lhs->ty->kind == TY_CHAR) {
      Node *new = new_unary(ND_NEG, node->lhs->lhs->lhs, node->tok);
      new->ty = node->ty;
      return new;
    }
    // (ND_CAST TY_CHAR(2) (+ TY_INT(4) (ND_CAST TY_INT(4) (ND_VAR TY_CHAR(2) y0 +9 )) (ND_CAST TY_INT(4) (ND_VAR ty_uchar y +2 ))))
    if (node->ty->kind == TY_CHAR
    &&  (node->lhs->kind == ND_ADD || node->lhs->kind == ND_SUB)
    &&  node->lhs->lhs->kind == ND_CAST
    &&  node->lhs->lhs->ty->kind == TY_INT
    &&  node->lhs->lhs->lhs->ty->kind == TY_CHAR
    &&  node->lhs->rhs->kind == ND_CAST
    &&  node->lhs->rhs->ty->kind == TY_INT
    &&  node->lhs->rhs->lhs->ty->kind == TY_CHAR) {
      Node *new = new_binary(node->lhs->kind,node->lhs->lhs->lhs,node->lhs->rhs->lhs,node->tok);
      new->ty = node->ty;
      return new;
    }
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
    if (is_flonum(node->ty)
    &&  node->lhs->kind==ND_NUM
    &&  is_integer(node->lhs->ty)) {
      Node *new = new_copy(node->lhs);
      new->fval = node->lhs->val;
      new->ty   = node->ty;
      return new;
    }
    return node;
  } // ND_CAST
  case ND_MEMZERO:
    return node;
  case ND_COND:
    node->cond = optimize_expr(node->cond);
    node->then = optimize_expr(node->then);
    node->els  = optimize_expr(node->els);
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
    if (is_compare(node->lhs)
    &&  is_integer(node->lhs->lhs->ty)
    &&  is_integer(node->lhs->rhs->ty)){
      return negate_condition(optimize_l(node->lhs));
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
    node =  optimize_l(node);
    return optimize_const_expr(node);
  // Below is a binary operator
  case ND_LOGAND: {
    node = optimize_lr(node);
    if (is_integer(node->lhs->ty)
    &&  is_integer_constant(node->lhs,&val)
    &&  node->lhs->ty == ty_int
    &&  val==0) {
      node = new_num(0,node->tok);
      node->ty = ty_int;
      return node;
    }
    return optimize_const_expr(node);
  }
  case ND_LOGOR:
    node = optimize_lr(node);
    if (is_integer(node->lhs->ty)
    &&  is_integer_constant(node->lhs,&val)
    &&  node->lhs->ty == ty_int
    &&  val!=0) {
      node = node->rhs;
      return node;
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
    if (node->ty->kind            == TY_INT
    &&  !node->ty->is_unsigned
    &&  node->lhs->kind           == ND_CAST
    &&  node->lhs->ty->kind       == TY_INT
    &&  !node->lhs->ty->is_unsigned
    &&  node->lhs->lhs->ty->kind  == TY_CHAR
    &&  node->lhs->lhs->ty->is_unsigned) {
      if (node->rhs->kind           == ND_NUM
      &&  node->rhs->ty->kind       == TY_INT
      &&  node->rhs->val            >= 0
      &&  node->rhs->val            <= 255) {
        Node *new = new_copy(node);
        new->ty  = ty_uchar;
        new->lhs = node->lhs->lhs;
        new->rhs = node->rhs;
        new->rhs->ty = ty_uchar;
        new = new_cast(new,ty_int);
        return optimize_const_expr(new);
      }
      if (node->rhs->kind           == ND_CAST
      &&  node->rhs->ty->kind       == TY_INT
      &&  !node->rhs->ty->is_unsigned
      &&  node->rhs->lhs->ty->kind  == TY_CHAR
      &&  node->rhs->lhs->ty->is_unsigned) {
        Node *new = new_copy(node);
        new->ty  = ty_uchar;
        new->lhs = node->lhs->lhs;
        new->rhs = node->rhs->lhs;
        new = new_cast(new,ty_int);
        return optimize_const_expr(new);
      }
    }
    return optimize_const_expr(node);
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE: {
    int64_t val;
    int64_t lval, rval;
    Type *lty, *rty;

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
#if 1
// (== ty_int (ND_CAST TY_INT(4) (ND_DEREF ty_uchar (ND_VAR TY_PTR(10) str +4 ))) 45) 
    if ( (lty=is_byte(node->lhs))
    &&   (rty=is_byte(node->rhs)) ) {
      Node *lhs = node->lhs;
      Node *rhs = node->rhs;
      if (lhs->kind == ND_CAST) {
        lhs = lhs->lhs;
      }
      if (rhs->kind == ND_CAST) {
        rhs = rhs->lhs;
      }
      if (rhs->kind == ND_NUM
      &&  is_integer_constant(rhs,&val)) { // Is the right-hand side within the range?
        if (lty->is_unsigned) {
          if (val >= 0 && val <= 255) {
            rhs->ty = ty_uchar;
            node->lhs = lhs;
            node->rhs = rhs;
            node->ty = ty_uchar;
          }
        }else{
          if (val >= -128 && val <= 127) {
            rhs->ty = ty_char;
            node->lhs = lhs;
            node->rhs = rhs;
            node->ty = ty_char;
          }
        }
      }
    }
#endif
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
          node->rhs->val++;
          node->kind = ND_LT;
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
          node->rhs->val++;
          node->kind = ND_GE;
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
//  lhs >  rhs -1 → lhs>= rhs
      }else if ((node->kind==ND_GE && node->rhs->kind==ND_ADD)
      ||  (node->kind==ND_GT && node->rhs->kind==ND_SUB)) {
        if(is_integer_constant(node->rhs->rhs,&val) &&  val==1 ){
          node->kind = (node->kind==ND_GE)?ND_GT:ND_GE;
          node->rhs = optimize_expr(node->rhs->lhs);
        }
      }
    }

//  println("; optimize RO %d cost:%d %d",node->kind,node_cost(node->lhs),node_cost(node->rhs));
    if ( node_cost(node->lhs) < node_cost(node->rhs)
    ||  (node_cost(node->lhs) == node_cost(node->rhs))) {
//    println("; optimize RO %d swap_lr",node->kind);
      switch(node->kind){
      case ND_EQ:
        node = swap_lr(node);
        break;
      case ND_NE:
        node = swap_lr(node);
        break;
      case ND_LE:
        node = swap_lr(node);
        node->kind = ND_GE;
        break;
      case ND_GT:
        node = swap_lr(node);
        node->kind = ND_LT;
        break;
      }
      return node;
    }
    return node;
  } // relative op
  case ND_SHL:
  case ND_SHR: {
    int64_t val;
    node = optimize_lr(node);
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
    if (node->kind == ND_SHR) {
      if (node->ty->kind == TY_INT
      &&  node->lhs->kind == ND_CAST
      &&  node->lhs->ty->kind == TY_INT
      &&  node->lhs->lhs->ty->kind == TY_CHAR) {
        Node *n1 = new_binary(ND_SHR,node->lhs->lhs,node->rhs,node->tok);
        n1->ty = node->lhs->lhs->ty;
        Node *n2 = new_copy(node->lhs);
        n2->lhs = n1;
        n2->ty = node->ty;
        return n2;
      }
    }
    return optimize_const_expr(node);
  } // ND_SHL, ND_SHR
  case ND_POST_INCDEC:
  case ND_PRE_INCDEC:
    return optimize_lr(node);
  case ND_ADDEQ:
  case ND_SUBEQ:
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
      } else if (val<0
      ||  val>=(node->ty->size)*8) { // TODO: bit field
//      warn_tok(node->tok,"shift count negative or too big, undefined behavior");
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

Obj *optimize(Obj *prog)
{
	return prog;
}
