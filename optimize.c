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
  switch(node->kind){
  case ND_NUM:
    return 1;
  case ND_VAR:
    if (node->var->ty->kind == TY_VLA)	return 200;
    if (node->var->is_local)		return test_addr_x(node)?50:100;
    return 20;
  case ND_CAST:
    return node_cost(node->lhs)+10;
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

Node *optimize_expr(Node *node)
{
  Node *lhs = node->lhs;
  Node *rhs = node->rhs;
  Node *tmp;

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
    //gen_expr(node->lhs);
    //gen_expr(node->rhs);
    return node;
  case ND_CAST:
    node->lhs = optimize_expr(node->lhs);
//    return node;
#if 1
#if 0
//    println("; ND_CAST: node->lhs->ty->kind %d, node->ty->kind %d, %s %d",node->lhs->ty->kind,node->ty->kind,__FILE__,__LINE__);
//    println("; ND_CAST: node->lhs->ty %p, node->ty %p",node->lhs->ty,node->ty);
      println("; ND_CAST: lhs:%s node:%s",type_str(node->lhs),type_str(node));
      println("; ND_CAST: lhs->ty->size:%d node->ty->size:%d",node->lhs->ty->size,node->ty->size);
      println("; ND_CAST: lhs->ty->is_integer:%d node->ty->is_integer:%d",is_integer(node->lhs->ty),is_integer(node->ty));
      println("; ND_CAST: lhs->ty->is_unsigned:%d node->ty->is_unsigned:%d",node->lhs->ty->is_unsigned,node->ty->is_unsigned);
#endif
    if (is_integer(node->ty) && is_integer(node->lhs->ty)){
      if (node->ty->size == node->lhs->ty->size
      &&  (node->ty->is_unsigned == node->lhs->ty->is_unsigned)){
	println("; delete ND_CAST");
        return optimize_expr(node->lhs);
      }
    }
    if (node->lhs->ty == node->ty){
      println("; delete ND_CAST");
      return optimize_expr(node->lhs);
    }
    if (node->lhs->ty->kind==4 && node->ty->kind==5){
//      println("; ND_CAST: int to long");
      if (node->lhs->kind == ND_NUM){
	node->lhs->ty = node->ty;
	return node->lhs;
      }
    }
    if (node->lhs->ty->kind==4 && node->ty->kind==10){
      println("; ND_CAST: int to ptr: skip cast");
      return optimize_expr(node->lhs);
    }
#if 1
    if (node->lhs->ty->kind==4 && node->ty->kind==4
    &&  node->lhs->ty->is_unsigned != node->ty->is_unsigned){
      node->lhs->ty->is_unsigned = node->ty->is_unsigned;
      return optimize_expr(node->lhs);
    }
#endif
    return node;
#endif
  case ND_MEMZERO:
  case ND_COND:
  case ND_FUNCALL:
  case ND_LABEL_VAL:
    return node;
  case ND_NOT:
  case ND_BITNOT:
  case ND_LOGAND:
  case ND_LOGOR:
    return optimize_l(node);
  // Below is a binary operator
  case ND_ADD:
    node = optimize_lr_swap(node);
    if (lhs->kind == ND_NUM && rhs->kind == ND_NUM && lhs->ty->kind == rhs->ty->kind){
      lhs->val +=  rhs->val;
      return lhs;
    }
    return node;
  case ND_SUB:
    node = optimize_lr(node);
    if (lhs->kind == ND_NUM && rhs->kind == ND_NUM && lhs->ty->kind == rhs->ty->kind){
      lhs->val -= rhs->val;
      return lhs;
    }
    return node;
  case ND_MUL:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    return node = optimize_lr_swap(node);
  case ND_DIV:
  case ND_MOD:
    return optimize_lr(node);
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
    node->lhs = optimize_expr(node->lhs);
    node->rhs = optimize_expr(node->rhs);
// bug: 9006-knight don't run
#if 0
    println("; optimize RO %d cost:%d %d",node->kind,node_cost(node->lhs),node_cost(node->rhs));
    if ( node_cost(node->lhs) < node_cost(node->rhs)
    ||  (node_cost(node->lhs) == node_cost(node->rhs)
      &&(node->kind == ND_LE || node->kind == ND_GT))){
      println("; optimize RO %d swap_lr",node->kind);
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
#endif
    return node;
  case ND_SHL:
  case ND_SHR:
    return optimize_lr(node);
  }
  return node;
}

Obj *optimize(Obj *prog)
{
	return prog;
}
