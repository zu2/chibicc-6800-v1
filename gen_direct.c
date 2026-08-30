#include "chibicc.h"

static bool gen_direct_8bit_imm_ext_sub(Node *node, char *opb, bool test)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));

  if (!is_integer_or_ptr(node->ty)) {
    return 0;
  }

  switch(node->kind){
  case ND_NUM:
    if (is_store) {
      return 0;
    }
    if (test) return 1;
    if (strcmp(opb,"addb")==0 && (node->val & 0x00ff)==0) {
      return 1;
    }
    if (strcmp(opb,"ldab")==0) {
      ldab_i((uint16_t)node->val);
    } else if (strcmp(opb,"andb")==0) {
      andb_i((uint16_t)node->val);
    } else if (strcmp(opb,"orab")==0) {
      orab_i((uint16_t)node->val);
    } else if (strcmp(opb,"eorb")==0) {
      eorb_i((uint16_t)node->val);
    } else {
      println("\t%s #<%u", opb, (uint16_t)node->val);
    }
    return 1;
  case ND_VAR:
    if (node->var->ty->kind == TY_VLA) {
      return 0;
    }
    if (node->var->is_local) {
      return 0;
    }
    if (test) return 1;
    if (node->ty->size == 1) {
      println("\t%s _%s",opb,node->var->name);
    } else {
      println("\t%s _%s+%d",opb,node->var->name,node->ty->size-1);
    }
    if (is_store) {
      invalidate_EXT(node);
    }
    return 1;
  case ND_MEMBER:
    if (node->member->is_bitfield) {
      return 0;
    }
    if (!is_global_var(node->lhs)) {
      return 0;
    }
    if (test) return 1;
    println("\t%s _%s+%d",opb,node->lhs->var->name,
            node->member->offset + node->ty->size-1);
    if (is_store) {
      invalidate_EXT(node->lhs);
    }
    return 1;
  case ND_DEREF: {
    if (node->lhs->kind == ND_NUM) {
      assert (node->lhs->ty->kind == TY_PTR);
      if (test) return 1;
      println("\t%s %ld",opb,node->lhs->val + node->ty->size-1);
      return 1;
    }
    if (node->lhs->kind      == ND_CAST
    &&  node->lhs->ty->kind  == TY_PTR
    &&  node->lhs->lhs->kind == ND_NUM
    &&  is_integer(node->lhs->lhs->ty)) {
      if (test) return 1;
      println("\t%s %ld",opb,node->lhs->lhs->val + node->ty->size-1);
      return 1;
    }
    if (node->lhs->kind == ND_VAR
    &&  is_global_array(node->lhs)) {
      if (test) return 1;
      if (node->ty->size == 1) {
        println("\t%s _%s",opb,node->lhs->var->name);
      } else {
        println("\t%s _%s+%d",opb,node->lhs->var->name,node->ty->size-1);
      }
      if (is_store) {
        invalidate_EXT(node->lhs);
      }
      return 1;
    }
    if (node->lhs->kind == ND_ADD) {
      Node *lhs = node->lhs->lhs;
      Node *rhs = node->lhs->rhs;
      int64_t val;
      if (lhs->kind == ND_CAST
      &&  lhs->ty->kind == TY_PTR
      &&  is_global_array(lhs->lhs)
      &&  rhs->kind == ND_CAST
      &&  rhs->ty->kind == TY_PTR
      &&  is_integer_constant(rhs->lhs,&val)) {
        if (test) return 1;
        if (val + node->ty->size-1 == 0) {
          println("\t%s _%s",opb,lhs->lhs->var->name);
        } else {
          println("\t%s _%s+%ld",opb,lhs->lhs->var->name,val + node->ty->size-1);
        }
        if (is_store) {
          invalidate_EXT(lhs->lhs);
        }
        return 1;
      }
      if (is_global_array(lhs)
      &&  is_integer_constant(rhs,&val)) {
        if (test) return 1;
        if (val + node->ty->size-1 == 0) {
          println("\t%s _%s",opb,lhs->var->name);
        } else {
          println("\t%s _%s+%ld",opb,lhs->var->name,val + node->ty->size-1);
        }
        if (is_store) {
          invalidate_EXT(lhs);
        }
        return 1;
      }
    }
    return 0;
  } // ND_DEREF
  case ND_CAST:
    if (node->ty->kind == TY_BOOL) {
      return 0;
    }
    if (is_integer_or_ptr(node->lhs->ty)) {
      if (test) return can_direct_8bit_imm_ext(node->lhs);
      return gen_direct_8bit_imm_ext(node->lhs, opb);
    }
    return 0;
  }
  return 0;
}

bool can_direct_8bit_imm_ext(Node *rhs)
{
  return gen_direct_8bit_imm_ext_sub(rhs,NULL,true);
}

bool gen_direct_8bit_imm_ext(Node *rhs, char *opb)
{
  return gen_direct_8bit_imm_ext_sub(rhs,opb,0);
}

static bool gen_direct_8bit_ix_sub(Node *node, char *opb, bool test)
{
  if (!is_integer_or_ptr(node->ty)) {
    return 0;
  }

  switch(node->kind){
  case ND_NUM:
    return 0;
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return 0;
    }
    if (!node->var->is_local) {
      return 0;
    }
    if (!test_addr_x(node)) return 0;
    if (test) return 1;
    int off = gen_addr_x(node);
    println("\t%s %d,x",opb,off+node->ty->size-1);
    return 1;
  } // ND_VAR
  case ND_CAST:
    if (node->ty->kind == TY_BOOL) {
      return 0;
    }
    if (is_integer_or_ptr(node->lhs->ty)) {
      if (test) return can_direct_8bit_ix(node->lhs);
      return gen_direct_8bit_ix(node->lhs, opb);
    }
    return 0;
  default:
    if (test_addr_x(node)) {
      if (test) return 1;
      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off+node->ty->size-1);
      return 1;
    }
    return 0;
  }
  return 0;
}

bool can_direct_8bit_ix(Node *rhs)
{
  return gen_direct_8bit_ix_sub(rhs,NULL,true);
}

bool gen_direct_8bit_ix(Node *rhs, char *opb)
{
  return gen_direct_8bit_ix_sub(rhs,opb,0);
}

static bool gen_direct_8bit_sub(Node *node, char *opb, bool test)
{
  if (gen_direct_8bit_imm_ext_sub(node,opb,test))
    return 1;

  return gen_direct_8bit_ix_sub(node,opb,test);
}

bool can_direct_8bit(Node *rhs)
{
  return gen_direct_8bit_sub(rhs,NULL,true);
}

bool gen_direct_8bit(Node *rhs, char *opb)
{
  return gen_direct_8bit_sub(rhs,opb,0);
}

static bool gen_direct_imm_ext_sub(Node *node,char *opb, char *opa, bool test, bool is_char)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));
  char *addr;

  if (!is_store && (addr=is_addr_constant(node))) {
    switch (node->ty->kind) {
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
    case TY_ARRAY:
      if (test) return true;
      println("\t%s #<%s", opb, addr);
      if (opa)
        println("\t%s #>%s",   opa, addr);
      return 1;
    }
  }
  switch(node->kind){
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:		// TODO:
      if (test) return 1;
      if (strcmp(opb,"ldab")==0) {
        ldab_i((uint16_t)node->val);
        return 1;
      }
      println("\t%s #<%u", opb, (uint16_t)node->val);
      return 1;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test) return 1;
      if (strcmp(opb,"addb")==0 && node->val==0 ) { // subb used for compare
        return 1;
      }
      if (strcmp(opb,"ldab")==0) {
        if (opa) ldd_i ((uint16_t)node->val);
        else     ldab_i((uint16_t)node->val);
      } else if (strcmp(opb,"andb")==0) {
        if (opa) and_i ((uint16_t)node->val);
        else     andb_i((uint16_t)node->val);
      } else if (strcmp(opb,"orab")==0) {
        if (opa) ora_i ((uint16_t)node->val);
        else     orab_i((uint16_t)node->val);
      } else if (strcmp(opb,"eorb")==0) {
        if (opa) eor_i ((uint16_t)node->val);
        else     eorb_i((uint16_t)node->val);
      } else {
        println("\t%s #<%u", opb, (uint16_t)node->val);
        if (opa) println("\t%s #>%u", opa, (uint16_t)node->val);
      }
      return 1;
    default:
      return 0;
    }
  } // ND_NUM
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return 0;
    }
    if(node->var->is_local){
      if (node->ty->kind==TY_ARRAY) {
        if (test) return true;
        println("\t%s @bp+1",opb);
        if (opa)
          println("\t%s @bp",opa);
        if (strcmp(opb,"addb")==0 && node->var->offset==0) {
          return 1;
        }
        println("\t%s #<%d",opb,node->var->offset);
        if (opa)
          println("\t%s #>%d",opa,node->var->offset);
        return 1;
      }
    }else{
      // global
      if (node->ty->kind==TY_FUNC)
        return 0;
//    if (node->ty->kind==TY_CHAR && !node->ty->is_unsigned && !opa)
//        return 0;
      if (test) return 1;
      if (is_int8(node->ty)) {
   	    if (is_store) {
          println("\t%s _%s",opb,node->var->name);
          invalidate_EXT(node);
          return 1;
        }
        println("\t%s _%s",opb,node->var->name);
        if (opa)
          println("\t%s #0",opa);
	      return 1;
	    }
      if (node->ty->kind==TY_ARRAY) {
        println("\t%s #<_%s",opb,node->var->name);
        if (opa)
          println("\t%s #>_%s",opa,node->var->name);
	      return 1;
      }
      println("\t%s _%s+1",opb,node->var->name);
      if (opa)
        println("\t%s _%s",opa,node->var->name);
      if (is_store) {
        invalidate_EXT(node);
      }
      return 1;
    }
    return 0;
  } // ND_VAR
  case ND_DEREF:
    switch(node->lhs->kind){
    // (ND_DEREF ty_char (ND_NUM TY_PTR e000))
    case ND_NUM:
      assert (node->lhs->ty->kind == TY_PTR);
      if (test) return 1;
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\t%s %ld",opb,node->lhs->val);
        return 1;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        println("\t%s %ld+1",opb,node->lhs->val);
        if (opa)
          println("\t%s %ld",opa,node->lhs->val);
        return 1;
      } // ND_DEREF → ND_NUM
      break;
    // (ND_DEREF ty_int (ND_VAR TY_ARRAY(12) _L_1 global)
    case ND_VAR: {
      if (!is_integer(node->ty) || node->ty->kind==TY_LONG)
        return 0;
      if (is_global_array(node->lhs)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s _%s",opb,node->lhs->var->name);
          invalidate_EXT(node->lhs);
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s _%s+1",opb,node->lhs->var->name);
          if (opa)
            println("\t%s _%s",  opa,node->lhs->var->name);
          invalidate_EXT(node->lhs);
          return 1;
        }
      }
    } // ND_DEREF → ND_VAR
      break;
    case ND_CAST: {
      if (!is_integer_or_ptr(node->ty) || node->ty->kind==TY_LONG)
        return 0;
      if (node->lhs->ty->kind  == TY_PTR
      &&  node->lhs->lhs->kind == ND_NUM
      &&  is_integer(node->lhs->lhs->ty)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s %ld",opb,node->lhs->lhs->val);
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s %ld+1",opb,node->lhs->lhs->val);
          if (opa)
            println("\t%s %ld",  opa,node->lhs->lhs->val);
          return 1;
        }
      }
    } // ND_DEREF → ND_CAST
      break;
    case ND_ADD: { // ND_DEREF → ND_ADD
      // global array[const]
      Node *lhs = node->lhs->lhs;
      Node *rhs = node->lhs->rhs;
      int64_t val;
      if (lhs->kind == ND_CAST
      &&  lhs->ty->kind  == TY_PTR
      &&  is_global_array(lhs->lhs)
      &&  rhs->kind == ND_CAST
      &&  rhs->ty->kind == TY_PTR
      &&  is_integer_constant(rhs->lhs,&val)) {
        if (test) return 1;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (val==0) {
            println("\t%s _%s",opb,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->lhs->var->name,val);
          }
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->lhs->var->name);
            if (opa)
              println("\t%s _%s",opa,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->lhs->var->name,val);
            if (opa)
              println("\t%s _%s+%ld",opa,lhs->lhs->var->name,val);
          }
          return 1;
        }
      }
      if (is_global_array(lhs)
      &&  is_integer_constant(rhs,&val)) {
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) return 1;
          if (val==0) {
            println("\t%s _%s",opb,lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->var->name,val);
          }
          return 1;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return 1;
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->var->name);
            if (opa)
              println("\t%s _%s",  opa,lhs->var->name);
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->var->name,val);
            if (opa)
              println("\t%s _%s+%ld",  opa,lhs->var->name,val);
          }
          return 1;
        }
      }
    } // ND_DEREF → ND_ADD
    break;
    } // ND_DEREF
    return 0;
  case ND_CAST:
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_imm_ext_sub(node->lhs, opb, opa, test,0))
      return 1;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_imm_ext_sub(node->lhs, opb, opa, test,0)) {
      return 1;
    }
    if (node->ty->kind      == TY_PTR
    &&  gen_direct_imm_ext_sub(node->lhs, opb, opa, test, 0))
      return 1;
    // (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) m +0 )
    if (node->ty->kind == TY_PTR
    &&  node->lhs->kind == ND_VAR
    &&  is_local_array(node->lhs)) {
       if (test)
         return 1;
      println("\taddb @bp+1");
      println("\tadca @bp");
      if (node->lhs->var->offset){
        println("\taddb #<%d",node->lhs->var->offset);
        println("\tadca #>%d",node->lhs->var->offset);
      }
      return 1;
    }
    char *addr;
    if ((addr=is_addr_constant(node))!=NULL) {
      if (test) return 1;
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        println("\t%s #%s",opb,addr);
        return 1;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        println("\t%s #<%s", opb,addr);
        if (opa)
          println("\t%s #>%s", opa,addr);
        return 1;
      }
      return 0;
    }
    return 0;
  default:
    if (test_addr_x(node)) {
      if (node->kind == ND_MEMBER
      &&  !node->member->is_bitfield
      &&  is_global_var(node->lhs)) {
        char *name = node->lhs->var->name;
        int moff = node->member->offset;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) {
            return is_char || node->ty->is_unsigned;
          }
          println("\t%s _%s+%d",opb,name,moff);
          if (is_store) {
            invalidate_EXT(node->lhs);
          }else if (opa) {
            println("\t%s #0",opa);
          }
          return 1;
        case TY_INT:
        case TY_SHORT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return 1;
          println("\t%s _%s+%d",opb,name,moff+1);
          if (opa)
            println("\t%s _%s+%d",opa,name,moff);
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return 1;
        }
      }
    }
    return 0;
  }
  return 0;
}

bool can_direct_imm_ext(Node *rhs)
{
  int r = gen_direct_imm_ext_sub(rhs,NULL,NULL,true,0);

  return r;
}

bool can_direct_char_imm_ext(Node *rhs)
{
  int r = gen_direct_imm_ext_sub(rhs,NULL,NULL,true,1);

  return r;
}

bool gen_direct_imm_ext(Node *rhs,char *opb, char *opa)
{
  return gen_direct_imm_ext_sub(rhs,opb,opa,0,0);
}

bool gen_direct_char_imm_ext(Node *rhs,char *opb, char *opa)
{
  return gen_direct_imm_ext_sub(rhs,opb,opa,0,1);
}

static bool gen_direct_ix_sub(Node *node,char *opb, char *opa, bool test, bool is_char)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));

  switch(node->kind){
  case ND_NUM:
    return 0;
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return 0;
    }
    if (!node->var->is_local) {
      return 0;
    }
    if (node->ty->kind==TY_ARRAY) {
      return 0;
    }
    if (!test_addr_x(node)) return 0;
    if (is_int8(node->ty)) {
      if (test) {
        return is_char || node->ty->is_unsigned;
      }
      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off);
      if (!is_store && opa) {
        println("\t%s #0",opa);
      }
    }else{
      if (test) return 1;
      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off+1);
      if (opa)
        println("\t%s %d,x",opa,off);
    }
    return 1;
  } // ND_VAR
  case ND_CAST:
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test,0))
      return 1;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test,0)) {
      return 1;
    }
    if (node->ty->kind      == TY_PTR
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test, 0))
      return 1;
    return 0;
  default:
    if (test_addr_x(node)) {
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        if (test) {
          return is_char || node->ty->is_unsigned;
        }
        int off = gen_addr_x(node);
        println("\t%s %d,x",opb,off);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
        return 1;
      case TY_INT:
      case TY_SHORT:
      case TY_ENUM:
      case TY_PTR:
        if (test) return 1;
        off = gen_addr_x(node);
        println("\t%s %d,x",opb,off+1);
        if (opa)
          println("\t%s %d,x",opa,off);
        return 1;
      default:
        return 0;
      }
    }
    return 0;
  }
  return 0;
}

bool can_direct_ix(Node *rhs, bool is_char)
{
  return gen_direct_ix_sub(rhs,NULL,NULL,true,is_char);
}

bool gen_direct_ix(Node *rhs, char *opb, char *opa, bool is_char)
{
  return gen_direct_ix_sub(rhs,opb,opa,0,is_char);
}

static bool gen_direct_sub(Node *node,char *opb, char *opa, bool test, bool is_char)
{
  if (gen_direct_imm_ext_sub(node,opb,opa,test,is_char))
    return 1;

  return gen_direct_ix_sub(node,opb,opa,test,is_char);
}

bool can_direct(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,true,0);

  return r;
}

bool can_direct_char(Node *rhs)
{
  int r = gen_direct_sub(rhs,NULL,NULL,true,1);

  return r;
}

bool gen_direct(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0,0);
}

bool gen_direct_char(Node *rhs,char *opb, char *opa)
{
  return gen_direct_sub(rhs,opb,opa,0,1);
}

int gen_direct_lr_8bit(Node *node, char *opb)
{
    if (can_direct_char(node->rhs)){
      gen_expr(node->lhs);
      if (gen_direct_char(node->rhs,opb,NULL))
        return 1;
      assert(0);
    }
    if (can_direct_char(node->lhs)){
      gen_expr(node->rhs);
      if (gen_direct_char(node->lhs,opb,NULL))
        return 1;
      assert(0);
    }
    return 0;
}

int gen_direct_lr(Node *node, char *opb, char *opa)
{
    int can_direct_lhs = can_direct(node->lhs);
    int can_direct_rhs = can_direct(node->rhs);

#if 0
    if (can_direct_lhs && can_direct_rhs){
      if (node->rhs->kind == ND_CAST
      &&  is_int16(node->rhs->ty)
      &&  is_int8(node->rhs->lhs->ty)
      &&  node->rhs->lhs->ty->is_unsigned) {
        gen_expr(node->rhs);
        if (gen_direct(node->lhs,opb,opa))
          return 1;
        assert(0);
      }
    }
#endif

    if (can_direct_rhs){
      gen_expr(node->lhs);
      if (gen_direct(node->rhs,opb,opa))
        return 1;
      assert(0);
    }
    if (can_direct_lhs){
      gen_expr(node->rhs);
      if (gen_direct(node->lhs,opb,opa))
        return 1;
      assert(0);
    }
    return 0;
}
