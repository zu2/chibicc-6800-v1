#include "chibicc.h"

bool builtin_memset(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "memset")
  && node->args && node->args->next && node->args->next->next) {
    int64_t dst, val, num;
    if (is_pointer_constant(node->args, &dst)
    &&  is_integer_constant(node->args->next, &val)
    &&  is_integer_constant(node->args->next->next, &num)) {
      println("; ND_FUNCALL: builtin_memset(0x%04x,%u,%u)",
                                (uint16_t)dst,(uint16_t)val,(uint16_t)num);
      if (num==0) {
        return true;
      }
      int odd = (num & 1);
      char *loop = new_label("L_%d");
      println("\tldab #%d",(uint16_t)val);
      println("\tldx #%d",(uint16_t)dst);
      println("%s:",loop);
      for (int i=odd; i<2; i++) {
        println("\tstab 0,x");
        println("\tinx");
      }
      println("\tcpx #%d",(uint16_t)(dst+num));
      println("\tbne %s",loop);
      IX_Dest = IX_None;
      if (!node->retval_unused) {
        ldd_i(dst);
      }
      return true;
    }
  }
  return false;
}

bool builtin_memcpy(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "memcpy")
  && node->args && node->args->next && node->args->next->next) {
    int64_t dst, src, num;
    if (is_pointer_constant(node->args, &dst)
    &&  is_pointer_constant(node->args->next, &src)
    &&  is_integer_constant(node->args->next->next, &num)
    &&  abs(dst-src)<=255 ) {
      println("; ND_FUNCALL: builtin_memcpy(0x%04x,0x%04x,%u)",
                                (uint16_t)dst,(uint16_t)src,(uint16_t)num);
      if (num==0) {
        return true;
      }
      int odd = (num & 1);
      char *loop = new_label("L_%d");
      if (dst>src) {
        println("\tldx #%d",(uint16_t)src);
        println("%s:",loop);
        for (int i=odd; i<2; i++) {
          println("\tldaa 0,x");
          println("\tstaa %d,x",(uint16_t)(dst-src));
          println("\tinx");
        }
        println("\tcpx #%d",(uint16_t)(src+num));
        println("\tbne %s",loop);
      }else if (dst<src) {
        println("\tldx #%d",(uint16_t)dst);
        println("%s:",loop);
        for (int i=odd; i<2; i++) {
          println("\tldaa %d,x",(uint16_t)(src-dst));
          println("\tstaa 0,x");
          println("\tinx");
        }
        println("\tcpx #%d",(uint16_t)(dst+num));
        println("\tbne %s",loop);
      }
      IX_Dest = IX_None;
      if (!node->retval_unused) {
        ldd_i(dst);
      }
      return true;
    }
  }
  return false;
}

bool builtin_strcpy(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "strcpy")
  && node->args && node->args->next) {
// (ND_CAST TY_PTR(10) (ND_VAR TY_ARRAY(12) _L_45 global))
//	ldab #<__L_45
//	ldaa #>__L_45
//
//  if (var->init_data) {
//    if (var->is_literal)
// var->ty->size = 6
// node->kind == ND_VAR
    Node *arg1 = node->args;
    Node *arg2 = node->args->next;
    ast_node_dump(arg2);
    
    if (arg2->kind == ND_CAST
    &&  arg2->ty->kind == TY_PTR) {
      arg2 = arg2->lhs;
    }
    if (arg2->kind != ND_VAR) {
      return false;
    }
    Obj *var = arg2->var;
    println("; var->init_data %p",var->init_data);
    if (!var->init_data) {
      return false;
    }
    println("; var->is_literal %d",var->is_literal);
    if (!var->is_literal) {
      return false;
    }
    println("; var->ty->size %d", var->ty->size);
    println("; _%s:", var->name);
    println("; var->init_data[] %s",var->init_data);
    size_t size = var->ty->size;  // Size includes the trailing \0

    IX_Dest = IX_None;
    int off = 0;
    if (test_expr_x(arg1)) {
      off = gen_expr_x(arg1,false);
      IX_Dest = IX_None;
    }else{
      gen_expr(arg1);
      tfr_dx();
    }
    if (size==0) {  // XXX ?
      error("strcpy literal size==0");
    }else if ((opt_O != '0')
          ||  (opt_O == 's' && size<=4)
          ||  (opt_O == '1' && size<=16)
          ||  (opt_O >= '2' && size<=32)){
      println("; ND_FUNCALL: builtin_strcpy(..., _%s)",var->name);
      if (size+off>=256) {
        println("\tldab #%d",off);
        println("\tjsr __abx");
      }
      size_t *space = malloc(size*sizeof(size_t));
      for (int i=0; i<size; i++) {
        space[i] = size;
        for (int j=size-1; j>i; j--) {
          if (var->init_data[i] == var->init_data[j]) {
            space[i] = j-i;
          }
        }
      }
      size_t spacing_A=size, spacing_B=size;
      int AccA=0, AccB=0;
      for (size_t i=0; i<size; i++) {
        if (var->init_data[i]==0) {
          println("\tclr %ld,x",i+off);
          break;
        }
        spacing_A--; spacing_B--;
        if (var->init_data[i]==AccA) {
          println("\tstaa %ld,x",i+off);
          spacing_A = space[i];
        }else if (var->init_data[i]==AccB) {
          println("\tstab %ld,x",i+off);
          spacing_B = space[i];
        }else if (spacing_A>spacing_B) {
          if (isprint(var->init_data[i]) &&  var->init_data[i]!='\\'){
            println("\tldaa #$%02x   ; '%c'",
              var->init_data[i],var->init_data[i]);
          }else{
            println("\tldaa #$%02x",var->init_data[i]);
          }
          println("\tstaa %ld,x",i+off);
          AccA = var->init_data[i];
          spacing_A = space[i];
        }else{
          if (isprint(var->init_data[i]) &&  var->init_data[i]!='\\'){
            println("\tldab #$%02x   ; '%c'",
              var->init_data[i],var->init_data[i]);
          }else{
            println("\tldab #$%02x",var->init_data[i]);
          }
          println("\tstab %ld,x",i+off);
          AccB = var->init_data[i];
          spacing_B = space[i];
        }
      }
      return true;
    }
  }
  return false;
}
