#include "chibicc.h"


static char *type_str(Type *ty)
{
  if (!ty) return "NULL";

  if (ty == ty_void) return "ty_void";
  if (ty == ty_bool) return "ty_bool";
  if (ty == ty_char) return "ty_char";
  if (ty == ty_short) return "ty_short";
  if (ty == ty_int) return "ty_int";
  if (ty == ty_long) return "ty_long";
  if (ty == ty_uchar) return "ty_uchar";
  if (ty == ty_ushort) return "ty_ushort";
  if (ty == ty_uint) return "ty_uint";
  if (ty == ty_ulong) return "ty_ulong";
  if (ty == ty_float) return "ty_float";
  if (ty == ty_double) return "ty_double";
  if (ty == ty_ldouble) return "ty_ldouble";

  switch(ty->kind){
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

static void ast_dump(Node *node);

static void ast_dump_unary(Node *node, char *name)
{
  printout("(%s ",name);
  ast_dump(node->lhs);
  printout(")");
}

static void ast_dump_unary_ty(Node *node, char *name)
{
  printout("(%s %s ",name,type_str(node->ty
  ));
  ast_dump(node->lhs);
  printout(")");
}

static void ast_dump_binary(Node *node, char *name)
{
  printout("(%s ",name);
  ast_dump(node->lhs);
  printout(" ");
  ast_dump(node->rhs);
  printout(")");
}

static void ast_dump_binary_ty(Node *node, char *name)
{
  printout("(%s %s ",name,type_str(node->ty));
  ast_dump(node->lhs);
  printout(" ");
  ast_dump(node->rhs);
  printout(")");
}

static void ast_dump(Node *node)
{
  if (!node){
    printout("()");
    return;
  }
  switch(node->kind){
  case ND_NULL_EXPR:
    printout("()");
    return;
  case ND_ADD:
    ast_dump_binary_ty(node,"+");
    return;
  case ND_SUB:
    ast_dump_binary_ty(node,"-");
    return;
  case ND_MUL:
    ast_dump_binary_ty(node,"*");
    return;
  case ND_DIV:
    ast_dump_binary_ty(node,"/");
    return;
  case ND_NEG:
    ast_dump_unary(node,"-");
    return;
  case ND_MOD:
    ast_dump_binary_ty(node,"%");
    return;
  case ND_BITAND:
    ast_dump_binary_ty(node,"&");
    return;
  case ND_BITOR:
    ast_dump_binary_ty(node,"|");
    return;
  case ND_BITXOR:
    ast_dump_binary_ty(node,"^");
    return;
  case ND_SHL:
    ast_dump_binary_ty(node,"<<");
    return;
  case ND_SHR:
    ast_dump_binary_ty(node,">>");
    return;
  case ND_EQ:
    ast_dump_binary_ty(node,"==");
    return;
  case ND_NE:
    ast_dump_binary_ty(node,"!=");
    return;
  case ND_LT:
    ast_dump_binary_ty(node,"<");
    return;
  case ND_LE:
    ast_dump_binary_ty(node,"<=");
    return;
  case ND_GT:
    ast_dump_binary_ty(node,">");
    return;
  case ND_GE:
    ast_dump_binary_ty(node,">=");
    return;
  case ND_ASSIGN:
    ast_dump_binary_ty(node,"=");
    return;
  case ND_COND:
    printout("(ND_COND %s ",type_str(node->ty));
    ast_dump(node->cond);
    printout(" ");
    ast_dump(node->then);
    printout(" ");
    ast_dump(node->els);
    printout(")");
    return;
  case ND_COMMA:
    ast_dump_binary(node,",");
    return;
  case ND_MEMBER:
    printout("(ND_MEMBER %s ",type_str(node->ty));
    ast_dump(node->lhs);
    printout(" ");
    printout("+%d)",node->member->offset);
    return;
  case ND_ADDR:
    ast_dump_unary(node,"ND_ADDR");
    return;
  case ND_DEREF:
    ast_dump_unary_ty(node,"ND_DEREF");
    return;
  case ND_NOT:
    ast_dump_unary_ty(node,"!");
    return;
  case ND_BITNOT:
    ast_dump_unary_ty(node,"~");
    return;
  case ND_LOGAND:
    ast_dump_binary_ty(node,"&&");
    return;
  case ND_LOGOR:
    ast_dump_binary_ty(node,"||");
    return;
  case ND_RETURN:
    ast_dump_unary(node,"ND_RETURN");
    return;
  case ND_IF:
    printout("(ND_IF ");
    ast_dump(node->cond);
    printout(" ");
    ast_dump(node->then);
    if (node->els) {
      printout(" ");
      ast_dump(node->els);
    }
    printout(")");
    return;
  case ND_FOR:
    if (node->init || node->inc){
      printout("(ND_FOR ");
      ast_dump(node->init);
      printout(" ");
      ast_dump(node->cond);
      printout(" ");
      ast_dump(node->inc);
      printout(" ");
      ast_dump(node->then);
      printout(")");
    }else{
      printout("(ND_WHILE ");
      ast_dump(node->cond);
      printout(" ");
      ast_dump(node->then);
      printout(")");
    }
    return;
  case ND_DO:
    printout("(ND_DO ");
    ast_dump(node->then);
    printout(" ");
    ast_dump(node->cond);
    printout(")");
    return;
  case ND_SWITCH:
    printout("(ND_SWITCH ");
    ast_dump(node->cond);
    printout(" ");
    ast_dump(node->then);
    printout(")");
    return;
  case ND_CASE:
    printout("(ND_CASE ");
    ast_dump(node->lhs);
    printout(")");
    return;
  case ND_BLOCK:
    printout("(ND_BLOCK ");
    ast_dump(node->body);
    printout(")");
    return;
  case ND_GOTO:
    printout("(ND_GOTO ");
    printout("%s",node->unique_label);
    printout(")");
    return;
  case ND_GOTO_EXPR:
    ast_dump_unary(node,"ND_GOTO_EXPR");
    return;
  case ND_LABEL:
    printout("(ND_LABEL ");
    printout("%s",node->label);
    printout(")");
    return;
  case ND_LABEL_VAL:
    printout("(ND_LABEL_VAL ");
    printout("%s",node->label);
    printout(")");
    return;
  case ND_FUNCALL:
    printout("(ND_FUNCALL %s", type_str(node->ty));
    ast_dump(node->lhs);
    printout(" ... )\n; ");
    return;
  case ND_EXPR_STMT:
    ast_dump_unary(node,"ND_EXPR_STMT");
    return;
  case ND_VAR:
    if(node->var->ty->kind == TY_VLA) {
      printout("(ND_VAR TY_VLA ... )");
      return;
    }
    if (strlen(node->var->name)){
      printout("(ND_VAR %s %s ",type_str(node->ty),node->var->name);
    }else{
      printout("(ND_VAR %s no-name ",type_str(node->ty));
    }
    if (node->var->is_local){
      printout("+%d ",node->var->offset);
    }else{
      printout("global");
    }
    printout(")");
    return;
//43   ND_VLA_PTR,   // VLA designator
  case ND_NUM:
    switch(node->ty->kind){
    case TY_BOOL:
      if (node->val)
        printout("true");
      else
        printout("false");
      break;
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
    case TY_LONG:
      printout("%ld", node->val);
      break;
    case TY_FLOAT:
      printout("%Le", node->fval);
      break;
    case TY_PTR:
      printout("(ND_NUM TY_PTR %04lx)",node->val);
      break;
    default:
      fprintf(stdout,"ND_NUM %d\n",node->ty->kind);
      assert(0);
    }
    return;
  case ND_CAST:
    printout("(ND_CAST %s ",type_str(node->ty));
    ast_dump(node->lhs);
    printout(")");
    return;
//46   ND_MEMZERO,   // Zero-clear a stack variable
  case ND_MEMZERO:
    printout("(ND_MEMZERO %s %d)",node->var->name,node->var->ty->size);
    return;
  case ND_ASM:
    printout("(ND_ASM \"%s\")\n; ",node->asm_str);
    return;
//48   ND_CAS,       // Atomic compare-and-swap
//49   ND_EXCH,      // Atomic exchange
  case ND_POST_INCDEC:
    ast_dump_binary(node,"ND_POST_INCDEC");
    return;
  case ND_ADDEQ:
    ast_dump_binary(node,"+=");
    return;
  case ND_SUBEQ:
    ast_dump_binary(node,"-=");
    return;
  case ND_MULEQ:
    ast_dump_binary(node,"*=");
    return;
  case ND_DIVEQ:
    ast_dump_binary(node,"/=");
    return;
  case ND_MODEQ:
    ast_dump_binary(node,"%=");
    return;
  case ND_ANDEQ:
    ast_dump_binary(node,"&=");
    return;
  case ND_OREQ:
    ast_dump_binary(node,"|=");
    return;
  case ND_XOREQ:
    ast_dump_binary(node,"^=");
    return;
  case ND_SHLEQ:
    ast_dump_binary(node,"<<=");
    return;
  case ND_SHREQ:
    ast_dump_binary(node,">>=");
    return;
  case ND_BULKINIT:
    printout("(ND_BULKINIT %s ",node->var->name);
    printout("%s ",type_str(node->var->ty));
    if (!node->var->is_local) {
      printout("global ");
    }
    if (node->var->is_static) {
      printout("static ");
    }
    if (node->var->is_local) {
      printout("+%d ",node->var->offset);
    }
    printout("%s ", node->bulk_data->name);
    printout("%d) ",node->ty->size);
    return;
  default:
    return;
  }
}

void ast_node_dump(Node *node)
{
  printout("; ");
  ast_dump(node);
  printout("\n");
}
