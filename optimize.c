#include "chibicc.h"

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
    println("; ND_CAST: node->lhs->ty->kind %d, node->ty->kind %d",node->lhs->ty->kind,node->ty->kind);
    if (node->lhs->ty == node->ty)
      return optimize_expr(node->lhs);
    if (node->lhs->ty->kind==4 && node->ty->kind==5){
      println("; ND_CAST: int to long");
      if (node->lhs->kind == ND_NUM){
	node->lhs->ty = node->ty;
	return node->lhs;
      }
    }
    if (node->lhs->ty->kind==4 && node->ty->kind==10){
      println("; ND_CAST: int to ptr: skip cast");
      return optimize_expr(node->lhs);
    }
    return node;
  case ND_MEMZERO:
  case ND_COND:
  case ND_NOT:
  case ND_BITNOT:
  case ND_LOGAND:
  case ND_LOGOR:
  case ND_FUNCALL:
  case ND_LABEL_VAL:
    return node;
  // Below is a binary operator
  case ND_ADD:
    if (node->lhs->kind==ND_NUM
    && node->rhs->kind==ND_NUM
    && node->lhs->ty == node->rhs->ty){
      if (node->lhs->ty->kind == TY_CHAR
      ||  node->lhs->ty->kind == TY_INT
      ||  node->lhs->ty->kind == TY_LONG){
        node->lhs->val += node->rhs->val;
	return node->lhs;
      }
    }
    return node;
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_GT:
  case ND_GE:
  case ND_MOD:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
  case ND_SHL:
  case ND_SHR:
    lhs = optimize_expr(node->lhs);
    rhs = optimize_expr(node->rhs);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
  }
  return node;
}

Obj *optimize(Obj *prog)
{
	return prog;
}
