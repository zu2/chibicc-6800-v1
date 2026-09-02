#include "chibicc.h"

static bool gen_direct_8bit_imm_sub(Node *node, char *opb, bool test)
{
  if (opb!=NULL
  && ((strncmp(opb,"sta",3)==0) || (strcmp(opb,"clr")==0))) {
    assert(0);
  }

  if (!is_integer_or_ptr(node->ty)) {
    return false;
  }

  switch(node->kind){
  case ND_NUM:

    if (test) return true;

    if (strcmp(opb,"addb")==0 && (node->val & 0x00ff)==0) {
      return true;
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
    return true;
  case ND_CAST:
    if (node->ty->kind == TY_BOOL) {
      return false;
    }
    if (is_integer_or_ptr(node->lhs->ty)) {
      if (test) {
        return can_direct_8bit_imm(node->lhs);
      }
      return gen_direct_8bit_imm(node->lhs, opb);
    }
    return false;
  }
  return false;
}

bool can_direct_8bit_imm(Node *node)
{
  return gen_direct_8bit_imm_sub(node,NULL,true);
}

bool gen_direct_8bit_imm(Node *rhs, char *opb)
{
  return gen_direct_8bit_imm_sub(rhs,opb,false);
}

static bool gen_direct_8bit_ext_sub(Node *node, char *opb, bool test)
{
  int is_store = ((opb!=NULL) && ((strncmp(opb,"sta",3)==0)
                               || (strcmp (opb,"clr")==0)));

  if (is_store && !is_int8(node->ty)) {
    assert(0);
  }

  if (!is_integer_or_ptr(node->ty)) {
    return false;
  }

  switch(node->kind){
  case ND_VAR:
    if (node->var->ty->kind == TY_VLA) {
      return false;
    }
    if (node->var->is_local) {
      return false;
    }

    if (test) return true;

    if (node->ty->size == 1) {
      println("\t%s _%s",opb,node->var->name);
    } else {
      println("\t%s _%s+%d",opb,node->var->name,node->ty->size-1);
    }
    if (is_store) {
      invalidate_EXT(node);
    }
    return true;

  case ND_MEMBER:
    if (node->member->is_bitfield) {
      return false;
    }
    if (!is_global_var(node->lhs)) {
      return false;
    }

    if (test) return true;

    println("\t%s _%s+%d",opb,node->lhs->var->name,
                          node->member->offset + node->ty->size-1);
    if (is_store) {
      invalidate_EXT(node->lhs);
    }
    return true;

  case ND_DEREF: {
    if (node->lhs->kind == ND_NUM) {
      assert (node->lhs->ty->kind == TY_PTR);

      if (test) return true;

      println("\t%s %ld",opb,node->lhs->val + node->ty->size-1);

      return true;
    }
    if (node->lhs->kind      == ND_CAST
    &&  node->lhs->ty->kind  == TY_PTR
    &&  node->lhs->lhs->kind == ND_NUM
    &&  is_integer(node->lhs->lhs->ty)) {

      if (test) return true;

      println("\t%s %ld",opb,node->lhs->lhs->val + node->ty->size-1);

      return true;
    }

    if (node->lhs->kind == ND_VAR
    &&  is_global_array(node->lhs)) {

      if (test) return true;

      if (node->ty->size == 1) {
        println("\t%s _%s",opb,node->lhs->var->name);
      } else {
        println("\t%s _%s+%d",opb,node->lhs->var->name,node->ty->size-1);
      }

      if (is_store) {
        invalidate_EXT(node->lhs);
      }
      return true;
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

        if (test) return true;

        if (val + node->ty->size-1 == 0) {
          println("\t%s _%s",opb,lhs->lhs->var->name);
        } else {
          println("\t%s _%s+%ld",opb,lhs->lhs->var->name,val + node->ty->size-1);
        }

        if (is_store) {
          invalidate_EXT(lhs->lhs);
        }

        return true;
      }

      if (is_global_array(lhs)
      &&  is_integer_constant(rhs,&val)) {

        if (test) return true;

        if (val + node->ty->size-1 == 0) {
          println("\t%s _%s",opb,lhs->var->name);
        } else {
          println("\t%s _%s+%ld",opb,lhs->var->name,val + node->ty->size-1);
        }

        if (is_store) {
          invalidate_EXT(lhs);
        }

        return true;
      }
    }
    return false;
  } // ND_DEREF

  case ND_CAST:
    if (node->ty->kind == TY_BOOL) {
      return false;
    }
    if (is_integer_or_ptr(node->lhs->ty)) {
      if (test) {
        return can_direct_8bit_ext(node->lhs);
      }
      return gen_direct_8bit_ext(node->lhs, opb);
    }
    return false;
  }
  return false;
}

bool can_direct_8bit_ext(Node *node)
{
  return gen_direct_8bit_ext_sub(node,NULL,true);
}

bool gen_direct_8bit_ext(Node *node, char *opb)
{
  return gen_direct_8bit_ext_sub(node,opb,false);
}

bool can_direct_8bit_imm_ext(Node *node)
{
  if (can_direct_8bit_imm(node)
  ||  can_direct_8bit_ext(node)) {
    return true;
  }

  return false;
}

bool gen_direct_8bit_imm_ext(Node *node, char *opb)
{
  if (can_direct_8bit_imm(node)) {
    return gen_direct_8bit_imm(node,opb);
  }
  if (can_direct_8bit_ext(node)) {
    return gen_direct_8bit_ext(node,opb);
  }

  return false;
}

static bool gen_direct_8bit_ix_sub(Node *node, char *opb, bool test)
{
  int is_store = ((opb!=NULL) && ((strncmp(opb,"sta",3)==0)
                               || (strcmp (opb,"clr")==0)));

  if (is_store && !is_int8(node->ty)) {
    assert(0);
  }

  if (!is_integer_or_ptr(node->ty)) {
    return false;
  }

  switch(node->kind){
  case ND_NUM:
    return false;

  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return false;
    }
    if (!node->var->is_local) {
      return false;
    }
    if (!test_addr_x(node)) {
      return false;
    }

    if (test) return true;

    int off = gen_addr_x(node);
    println("\t%s %d,x",opb,off+node->ty->size-1);

    return true;

  } // ND_VAR

  case ND_CAST:
    if (node->ty->kind == TY_BOOL) {
      return false;
    }

    if (is_integer_or_ptr(node->lhs->ty)) {
      if (test) {
        return can_direct_8bit_ix(node->lhs);
      }
      return gen_direct_8bit_ix(node->lhs, opb);
    }
    return false;

  default:
    if (test_addr_x(node)) {

      if (test) return true;

      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off+node->ty->size-1);

      return true;
    }
    return false;
  }
  return false;
}

bool can_direct_8bit_ix(Node *node)
{
  return gen_direct_8bit_ix_sub(node,NULL,true);
}

bool gen_direct_8bit_ix(Node *node, char *opb)
{
  return gen_direct_8bit_ix_sub(node,opb,0);
}

bool can_direct_8bit_ext_ix(Node *node)
{
  if (can_direct_8bit_ext(node)
  ||  can_direct_8bit_ix (node)) {
    return true;
  }
  return false;
}

bool gen_direct_8bit_ext_ix(Node *node, char *opb)
{
  if (can_direct_8bit_ext(node)) {
    return gen_direct_8bit_ext(node, opb);
  }
  if (can_direct_8bit_ix(node)) {
    return gen_direct_8bit_ix(node, opb);
  }
  return false;
}

bool can_direct_8bit(Node *node)
{
  if (can_direct_8bit_imm(node)
  ||  can_direct_8bit_ext(node)
  ||  can_direct_8bit_ix (node)) {
    return true;
  }
  return false;
}

bool gen_direct_8bit(Node *node, char *opb)
{
  if (can_direct_8bit_imm(node)) {
    return gen_direct_8bit_imm(node, opb);
  }
  if (can_direct_8bit_ext(node)) {
    return gen_direct_8bit_ext(node, opb);
  }
  if (can_direct_8bit_ix(node)) {
    return gen_direct_8bit_ix(node, opb);
  }

  return false;
}

int gen_direct_lr_8bit(Node *node, char *opb)
{
    if (can_direct_8bit(node->rhs)){
      gen_expr(node->lhs);
      if (gen_direct_8bit(node->rhs,opb)) {
        return true;
      }
      assert(0);
    }
    if (can_direct_8bit(node->lhs)){
      gen_expr(node->rhs);
      if (gen_direct_8bit(node->lhs,opb)) {
        return true;
      }
      assert(0);
    }
    return false;
}

static bool gen_direct_imm_sub(Node *node,char *opb, char *opa, bool test)
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
      if (opa) {
        println("\t%s #>%s",   opa, addr);
      }
      return true;
    }
  }
  switch(node->kind){
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_BOOL:
    case TY_CHAR:		// TODO:
      if (test) return true;
      if (strcmp(opb,"ldab")==0) {
        ldab_i((uint16_t)node->val);
        return true;
      }
      println("\t%s #<%u", opb, (uint16_t)node->val);
      return true;
    case TY_SHORT:
    case TY_INT:
    case TY_ENUM:
    case TY_PTR:
      if (test) return true;
      if (strcmp(opb,"addb")==0 && node->val==0 ) { // subb used for compare
        return true;
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
      return true;
    default:
      return false;
    }
  } // ND_NUM
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return false;
    }
    if(node->var->is_local){
      if (node->ty->kind==TY_ARRAY) {
        if (test) return true;
        println("\t%s @bp+1",opb);
        if (opa) {
          println("\t%s @bp",opa);
        }
        if (strcmp(opb,"addb")==0 && node->var->offset==0) {
          return true;
        }
        println("\t%s #<%d",opb,node->var->offset);
        if (opa) {
          println("\t%s #>%d",opa,node->var->offset);
        }
        return true;
      }
    }else{
      if (node->ty->kind==TY_FUNC) return false;
      if (node->ty->kind==TY_ARRAY) {
        if (test) return true;
        println("\t%s #<_%s",opb,node->var->name);
        if (opa) {
          println("\t%s #>_%s",opa,node->var->name);
        }
	      return true;
      }
    }
    return false;
  } // ND_VAR
  case ND_CAST:
    if (!is_int8(node->lhs->ty)
    &&  !is_int16_or_ptr_or_array(node->lhs->ty)) {
      return false;
    }
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_imm_sub(node->lhs, opb, opa, test))
      return true;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_imm_sub(node->lhs, opb, opa, test)) {
      return true;
    }
    if (node->ty->kind      == TY_PTR
    &&  (!is_int8(node->lhs->ty) || node->lhs->ty->is_unsigned) // !signed char
    &&  gen_direct_imm_sub(node->lhs, opb, opa, test))
      return true;
    // (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) m +0 )
    if (node->ty->kind == TY_PTR
    &&  node->lhs->kind == ND_VAR
    &&  is_local_array(node->lhs)) {
       if (test) return true;
      println("\taddb @bp+1");
      println("\tadca @bp");
      if (node->lhs->var->offset){
        println("\taddb #<%d",node->lhs->var->offset);
        println("\tadca #>%d",node->lhs->var->offset);
      }
      return true;
    }
    return false;
  }
  return false;
}

bool can_direct_imm(Node *node)
{
  return gen_direct_imm_sub(node,NULL,NULL,true);
}

bool gen_direct_imm(Node *node,char *opb, char *opa)
{
  return gen_direct_imm_sub(node,opb,opa,false);
}

static bool gen_direct_ext_sub(Node *node,char *opb, char *opa, bool test)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));

  if (!is_int8(node->ty)
  &&  !is_int16_or_ptr(node->ty)) {
    return false;
  }

  switch(node->kind){
  case ND_NUM:
    return false;
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return false;
    }
    if(node->var->is_local){
      return false;
    }else{
      // global
      if (node->ty->kind==TY_FUNC) return false;
//    if (node->ty->kind==TY_CHAR && !node->ty->is_unsigned && !opa)
//        return false;
      if (node->ty->kind==TY_ARRAY) return false;
      if (test) return true;
      if (is_int8(node->ty)) {
        println("\t%s _%s",opb,node->var->name);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
   	    if (is_store) {
          invalidate_EXT(node);
        }
	      return true;
	    }
      println("\t%s _%s+1",opb,node->var->name);
      if (opa) {
        println("\t%s _%s",opa,node->var->name);
      }
      if (is_store) {
        invalidate_EXT(node);
      }
      return true;
    }
    return false;
  } // ND_VAR
  case ND_DEREF:
    switch(node->lhs->kind){
    // (ND_DEREF ty_char (ND_NUM TY_PTR e000))
    case ND_NUM:
      assert (node->lhs->ty->kind == TY_PTR);
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        if (test) return true;
        println("\t%s %ld",opb,node->lhs->val);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
        return true;
      case TY_SHORT:
      case TY_INT:
      case TY_ENUM:
      case TY_PTR:
        if (test) return true;
        println("\t%s %ld+1",opb,node->lhs->val);
        if (opa) {
          println("\t%s %ld",opa,node->lhs->val);
        }
        return true;
      } // ND_DEREF → ND_NUM
      return false;
    // (ND_DEREF ty_int (ND_VAR TY_ARRAY(12) _L_1 global)
    case ND_VAR: {
      if (!is_integer(node->ty) || node->ty->kind==TY_LONG) {
        return false;
      }
      if (is_global_array(node->lhs)) {
        if (test) return true;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s _%s",opb,node->lhs->var->name);
          if (!is_store && opa) {
            println("\t%s #0",opa);
          }
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return true;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s _%s+1",opb,node->lhs->var->name);
          if (opa) {
            println("\t%s _%s",  opa,node->lhs->var->name);
          }
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return true;
        }
      }
    } // ND_DEREF → ND_VAR
      break;
    case ND_CAST: {
      if (!is_integer_or_ptr(node->ty) || node->ty->kind==TY_LONG) {
        return false;
      }
      if (node->lhs->ty->kind  == TY_PTR
      &&  node->lhs->lhs->kind == ND_NUM
      &&  is_integer(node->lhs->lhs->ty)) {
        if (test) return true;
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          println("\t%s %ld",opb,node->lhs->lhs->val);
          if (!is_store && opa) {
            println("\t%s #0",opa);
          }
          return true;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          println("\t%s %ld+1",opb,node->lhs->lhs->val);
          if (opa) {
            println("\t%s %ld",  opa,node->lhs->lhs->val);
          }
          return true;
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

        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) return true;
          if (val==0) {
            println("\t%s _%s",opb,lhs->lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->lhs->var->name,val);
          }
          if (!is_store && opa) {
            println("\t%s #0",opa);
          }
          return true;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return true;
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->lhs->var->name);
            if (opa) {
              println("\t%s _%s",opa,lhs->lhs->var->name);
            }
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->lhs->var->name,val);
            if (opa) {
              println("\t%s _%s+%ld",opa,lhs->lhs->var->name,val);
            }
          }
          return true;
        }
        return false;
      }
      if (is_global_array(lhs)
      &&  is_integer_constant(rhs,&val)) {
        switch(node->ty->kind) {
        case TY_BOOL:
        case TY_CHAR:
          if (test) return true;
          if (val==0) {
            println("\t%s _%s",opb,lhs->var->name);
          }else{
            println("\t%s _%s+%ld",opb,lhs->var->name,val);
          }
          if (!is_store && opa) {
            println("\t%s #0",opa);
          }
          return true;
        case TY_SHORT:
        case TY_INT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return true;
          if (val==0) {
            println("\t%s _%s+1",opb,lhs->var->name);
            if (opa) {
              println("\t%s _%s",  opa,lhs->var->name);
            }
          }else{
            println("\t%s _%s+%ld+1",opb,lhs->var->name,val);
            if (opa) {
              println("\t%s _%s+%ld",  opa,lhs->var->name,val);
            }
          }
          return true;
        }
      }
    } // ND_DEREF → ND_ADD
    break;
    } // ND_DEREF
    return false;
  case ND_CAST:
    if (!is_int8(node->lhs->ty)
    &&  !is_int16_or_ptr_or_array(node->lhs->ty)) {
      return false;
    }
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_ext_sub(node->lhs, opb, opa, test))
      return true;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_ext_sub(node->lhs, opb, opa, test)) {
      return true;
    }
    if (node->ty->kind      == TY_PTR
    &&  (!is_int8(node->lhs->ty) || node->lhs->ty->is_unsigned) // !signed char
    &&  gen_direct_ext_sub(node->lhs, opb, opa, test))
      return true;
    return false;
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
          if (!node->ty->is_unsigned) return false;
          if (test) return true;
          println("\t%s _%s+%d",opb,name,moff);
          if (!is_store && opa) {
            println("\t%s #0",opa);
          }
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return true;
        case TY_INT:
        case TY_SHORT:
        case TY_ENUM:
        case TY_PTR:
          if (test) return true;
          println("\t%s _%s+%d",opb,name,moff+1);
          if (opa) {
            println("\t%s _%s+%d",opa,name,moff);
          }
          if (is_store) {
            invalidate_EXT(node->lhs);
          }
          return true;
        }
      }
    }
    return false;
  }
  return false;
}

bool can_direct_ext(Node *node)
{
  return gen_direct_ext_sub(node,NULL,NULL,true);
}

bool can_direct_store_ext(Node *node)
{
  return gen_direct_ext_sub(node,NULL,NULL,true);
}

bool gen_direct_store_ext(Node *node,char *opb, char *opa)
{
  return gen_direct_ext_sub(node,opb,is_int8(node->ty)?NULL:opa,false);
}

bool gen_direct_ext(Node *node,char *opb, char *opa)
{
  return gen_direct_ext_sub(node,opb,opa,false);
}

bool can_direct_imm_ext(Node *node)
{
  if (can_direct_imm(node)
  ||  can_direct_ext(node)) {
    return true;
  }

  return false;
}

bool gen_direct_imm_ext(Node *node,char *opb, char *opa)
{
  if (can_direct_imm(node)) {
    return gen_direct_imm(node,opb,opa);
  }
  if (can_direct_ext(node)) {
    return gen_direct_ext(node,opb,opa);
  }

  return false;
}

static bool gen_direct_ix_sub(Node *node,char *opb, char *opa, bool test)
{
  int is_store = ((opb!=NULL) && ((strcmp(opb,"stab")==0) || (strcmp(opb,"clr")==0)));

  if (!is_int8(node->ty)
  &&  !is_int16_or_ptr(node->ty)) {
    return false;
  }

  switch(node->kind){
  case ND_NUM:
    return false;
  case ND_VAR: {
    if (node->var->ty->kind == TY_VLA ) {
      return false;
    }
    if (!node->var->is_local) {
      return false;
    }
    if (node->ty->kind==TY_ARRAY) {
      return false;
    }
    if (!test_addr_x(node)) return false;
    if (is_int8(node->ty)) {
      if (!node->ty->is_unsigned) return false;
      if (test) return true;
      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off);
      if (!is_store && opa) {
        println("\t%s #0",opa);
      }
    }else{
      if (test) return true;
      int off = gen_addr_x(node);
      println("\t%s %d,x",opb,off+1);
      if (opa) {
        println("\t%s %d,x",opa,off);
      }
    }
    return true;
  } // ND_VAR
  case ND_CAST:
    if (is_empty_cast(node->lhs->ty, node->ty)
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test))
      return true;
    if (is_int16(node->ty)
    &&  node->lhs->ty->kind == TY_CHAR
    &&  node->lhs->ty->is_unsigned
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test)) {
      return true;
    }
    if (node->ty->kind      == TY_PTR
    &&  gen_direct_ix_sub(node->lhs, opb, opa, test))
      return true;
    return false;
  default:
    if (test_addr_x(node)) {
      switch(node->ty->kind) {
      case TY_BOOL:
      case TY_CHAR:
        if (!node->ty->is_unsigned) return false;
        if (test) return true;
        int off = gen_addr_x(node);
        println("\t%s %d,x",opb,off);
        if (!is_store && opa) {
          println("\t%s #0",opa);
        }
        return true;
      case TY_INT:
      case TY_SHORT:
      case TY_ENUM:
      case TY_PTR:
        if (test) return true;
        off = gen_addr_x(node);
        println("\t%s %d,x",opb,off+1);
        if (opa) {
          println("\t%s %d,x",opa,off);
        }
        return true;
      default:
        return false;
      }
    }
    return false;
  }
  return false;
}

bool can_direct_ix(Node *node)
{
  return gen_direct_ix_sub(node,NULL,NULL,true);
}

bool gen_direct_ix(Node *node,char *opb, char *opa)
{
  return gen_direct_ix_sub(node,opb,opa,false);
}

bool can_direct_store_ix(Node *node)
{
  return gen_direct_ix_sub(node,NULL,NULL,true);
}

bool gen_direct_store_ix(Node *node,char *opb, char *opa)
{
  return gen_direct_ix_sub(node,opb,is_int8(node->ty)?NULL:opa,false);
}

bool can_direct_ext_ix(Node *node)
{
  if (can_direct_ext(node)
  ||  can_direct_ix (node)) {
    return true;
  }
  return false;
}

bool gen_direct_ext_ix(Node *node,char *opb, char *opa)
{
  if (can_direct_ext(node)) {
    return gen_direct_ext(node, opb, opa);
  }
  if (can_direct_ix(node)) {
    return gen_direct_ix(node, opb, opa);
  }
  return false;
}

bool can_direct_store_ext_ix(Node *node)
{
  if (can_direct_store_ext(node)
  ||  can_direct_store_ix (node)) {
    return true;
  }
  return false;
}

bool gen_direct_store_ext_ix(Node *node,char *opb, char *opa)
{
  if (can_direct_store_ext(node)) {
    return gen_direct_store_ext(node, opb, opa);
  }
  if (can_direct_store_ix(node)) {
    return gen_direct_store_ix(node, opb, opa);
  }
  return false;
}

bool can_direct(Node *node)
{
  if (can_direct_imm_ext(node)
  ||  can_direct_ix     (node)) {
    return true;
  }
  return false;
}

bool gen_direct(Node *node,char *opb, char *opa)
{
  if (can_direct_imm_ext(node)) {
    return gen_direct_imm_ext(node,opb,opa);
  }
  if (can_direct_ix(node)) {
    return gen_direct_ix(node,opb,opa);
  }
  return false;
}

int gen_direct_lr(Node *node, char *opb, char *opa)
{
    int can_direct_lhs = can_direct(node->lhs);
    int can_direct_rhs = can_direct(node->rhs);

    if (can_direct_rhs){
      gen_expr(node->lhs);
      if (gen_direct(node->rhs,opb,opa)) {
        return true;
      }
      assert(0);
    }
    if (can_direct_lhs){
      gen_expr(node->rhs);
      if (gen_direct(node->lhs,opb,opa)) {
        return true;
      }
      assert(0);
    }
    return false;
}
