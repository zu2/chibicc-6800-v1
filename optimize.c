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

static Node *negate_condition(Node *node)
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

static Node *swap_lr(Node *node)
{
  Node *tmp;
  tmp = node->lhs;
  node->lhs = node->rhs;
  node->rhs = tmp;

  return node;
}

static int node_cost(Node *node)
{
  int sign;

  switch(node->kind){
  case ND_NUM:
    return 1;
  case ND_VAR:
    sign = !node->ty->is_unsigned;
    if (node->var->ty->kind == TY_VLA)
      return 150 + sign;
    if (node->var->is_local)
      return (test_addr_x(node)?50:100) +sign;
    return (test_addr_x(node)?160:200) + sign;
  case ND_CAST:
    sign = !node->ty->is_unsigned;
    return node_cost(node->lhs)+10+sign;
  }
  return 255;
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


Node *optimize_expr(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  Node *new;

  if (!node)
    return node;

  switch (node->kind) {
  case ND_NULL_EXPR:
  case ND_NUM:
    return node;
  case ND_NEG:
    if (node->lhs->kind == ND_NUM){
      switch(node->lhs->ty->kind){
      case TY_CHAR:
      case TY_SHORT:
      case TY_INT:
      case TY_LONG:
	node->lhs->val = -node->lhs->val;
	return node->lhs;
      case TY_FLOAT:
      case TY_DOUBLE:
      case TY_LDOUBLE:
      default:
	return node;
      }
    }
  case ND_VAR:
    if (!node->ty)	// why??
      return node;

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
    node->lhs = optimize_expr(node->lhs);
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
    if (node->ty->kind==TY_BOOL
    &&  is_boolean_result(node->lhs)){
      return node->lhs;
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
    return node;
  case ND_LABEL_VAL:
    return node;
  // If the ND_NOT negates the result of a relational operator,
  // rewrite the relational operator.
  // In the case of float, rewriting is not possible because there is NaN.
  case ND_NOT:
#if 0
    if (node->lhs->kind==ND_NOT) {	// !! need boolize
	return optimize_expr(node->lhs->lhs);
    }
#endif
    if (is_compare(node->lhs)
    &&  is_integer(node->lhs->lhs->ty)
    &&  is_integer(node->lhs->rhs->ty)){
      return negate_condition(optimize_l(node->lhs));
    }
    return optimize_l(node);
  case ND_BITNOT:
    return optimize_l(node);
  case ND_LOGAND:
  case ND_LOGOR:
    return optimize_lr(node);
  // Below is a binary operator
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_MOD:
    node =  optimize_lr(node);
    if (lhs->kind==ND_NUM   && rhs->kind==ND_NUM
    &&  is_integer(lhs->ty) && is_integer(rhs->ty)){
       node->val = eval2(node,NULL);
       node->kind = ND_NUM;
       return node;
    }
    return node;
    break;
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    return node = optimize_lr_swap(node);
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);
//  println("; optimize RO %d cost:%d %d",node->kind,node_cost(node->lhs),node_cost(node->rhs));
    if ( node_cost(node->lhs) < node_cost(node->rhs)
    ||  (node_cost(node->lhs) == node_cost(node->rhs)
      &&(node->kind == ND_LE || node->kind == ND_GT))){
//    println("; optimize RO %d swap_lr",node->kind);
      node = swap_lr(node);
      switch(node->kind){
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
      }
      return node;
    }
    return node;
  case ND_SHL: {
  case ND_SHR:
    int64_t val;
    int64_t lval;
    int64_t rval;

    node = optimize_lr(node);
    if (is_const_expr(node)) {
      val = eval(node);
      switch (lhs->ty->size) {
      case 1: val =  lhs->ty->is_unsigned ? (uint8_t)val : (int8_t)val;
      case 2: val =  lhs->ty->is_unsigned ? (uint16_t)val : (int16_t)val;
      case 4: val =  lhs->ty->is_unsigned ? (uint32_t)val : (int32_t)val;
      }
      Node *new = new_node(ND_NUM, node->tok);
      new->val = val;
      new->ty = lhs->ty;
      return new;
    }
    return node;
  } // ND_SHL, ND_SHR
  case ND_POST_INCDEC:
  case ND_PRE_INCDEC:
    return optimize_lr(node);
  }
  return node;
}

Obj *optimize(Obj *prog)
{
	return prog;
}
