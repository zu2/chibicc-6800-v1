#include "chibicc.h"
#include <math.h>

//
// signbit(x):
//
bool builtin_signbit(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "signbit")
  && node->args && !node->args->next) {
    double  fval;

    if (!is_flonum(node->args->ty)) {
      error_tok(node->args->tok, "a non-floating point value as an argument");
    }
    if (is_flonum_constant(node->args, &fval)) {
      println("\tclra");
      if (signbit(fval)) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      return true;
    }
    Node   *base;
    int64_t off = 0;

    if ((base = find_base_var(node->args,&off))) {
      println("\tclra");
      if (off == 0) {
        println("\tldab _%s",base->var->name);
      } else {
        println("\tldab _%s+%ld",base->var->name,off);
      }
      println("\tandb #$80");
      return true;
    }
    if (test_addr_x(node->args)) {
      int off = gen_addr_x(node->args);
      println("\tclra");
      println("\tldab %d,x",off);
      println("\tandb #$80");
      return true;
    }
    gen_expr(node->args);
    println("\tclra");
    println("\tldab @long");
    println("\tandb #$80");
    return true;
  }
  return false;
}

//
// isnan(x):
//
bool builtin_isnan(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "isnan")
  && node->args && !node->args->next) {
    double  fval;

    if (!is_flonum(node->args->ty)) {
      error_tok(node->args->tok, "a non-floating point value as an argument");
    }
    if (is_flonum_constant(node->args, &fval)) {
      println("\tclra");
      if (isnan(fval)) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      return true;
    }
    Node   *base;
    int64_t off = 0;

    if ((base = find_base_var(node->args,&off))) {
      println("\tldab _%s+%ld", base->var->name, off+1);
      if (off == 0) {
        println("\tldaa _%s", base->var->name);
      } else {
        println("\tldaa _%s+%ld", base->var->name, off);
      }
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab _%s+%ld", base->var->name, off+2);
      println("\torab _%s+%ld", base->var->name, off+3);
      println("\tnegb");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    if (test_addr_x(node->args)) {
      int off = gen_addr_x(node->args);
      println("\tldab %d,x", off + 1);
      println("\tldaa %d,x", off);
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab %d,x", off + 2);
      println("\torab %d,x", off + 3);
      println("\tnegb");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    gen_expr(node->args);
      println("\tldab @long+1");
      println("\tldaa @long");
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab @long+2");
      println("\torab @long+3");
      println("\tnegb");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
    return true;
  }
  return false;
}

//
// isinf(x):
//
bool builtin_isinf(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "isinf")
  && node->args && !node->args->next) {
    double  fval;

    if (!is_flonum(node->args->ty)) {
      error_tok(node->args->tok, "a non-floating point value as an argument");
    }
    if (is_flonum_constant(node->args, &fval)) {
      println("\tclra");
      if (isinf(fval)) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      return true;
    }
    Node   *base;
    int64_t off = 0;

    if ((base = find_base_var(node->args,&off))) {
      println("\tldab _%s+%ld", base->var->name, off+1);
      if (off == 0) {
        println("\tldaa _%s", base->var->name);
      } else {
        println("\tldaa _%s+%ld", base->var->name, off);
      }
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab _%s+%ld", base->var->name, off+2);
      println("\torab _%s+%ld", base->var->name, off+3);
      println("\tsubb #1");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    if (test_addr_x(node->args)) {
      int off = gen_addr_x(node->args);
      println("\tldab %d,x", off + 1);
      println("\tldaa %d,x", off);
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab %d,x", off + 2);
      println("\torab %d,x", off + 3);
      println("\tsubb #1");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    gen_expr(node->args);
      println("\tldab @long+1");
      println("\tldaa @long");
      println("\taslb");
      println("\trola");
      println("\tadda #1");
      char *thru = new_label("L_thru_%d");
      println("\tbne %s", thru);
      println("\torab @long+2");
      println("\torab @long+3");
      println("\tsubb #1");
      println("%s:", thru);
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
    return true;
  }
  return false;
}

//
// isfinite(x):
//
bool builtin_isfinite(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "isfinite")
  && node->args && !node->args->next) {
    double  fval;

    if (!is_flonum(node->args->ty)) {
      error_tok(node->args->tok, "a non-floating point value as an argument");
    }
    if (is_flonum_constant(node->args, &fval)) {
      println("\tclra");
      if (isfinite(fval)) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      return true;
    }
    Node   *base;
    int64_t off = 0;

    if ((base = find_base_var(node->args,&off))) {
      println("\tldab _%s+%ld", base->var->name, off+1);
      if (off == 0) {
        println("\tldaa _%s", base->var->name);
      } else {
        println("\tldaa _%s+%ld", base->var->name, off);
      }
      println("\taslb");
      println("\trola");
      println("\tinca");
      println("\tnega");
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    if (test_addr_x(node->args)) {
      int off = gen_addr_x(node->args);
      println("\tldab %d,x", off + 1);
      println("\tldaa %d,x", off);
      println("\taslb");
      println("\trola");
      println("\tinca");
      println("\tnega");
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
      return true;
    }
    gen_expr(node->args);
      println("\tldab @long+1");
      println("\tldaa @long");
      println("\taslb");
      println("\trola");
      println("\tinca");
      println("\tnega");
      println("\tldab #0");
      println("\trolb");
      println("\tclra");
    return true;
  }
  return false;
}

//
// fabsf(x):
//
bool builtin_fabsf(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "fabsf")
  && node->args && !node->args->next) {
    if (!is_flonum(node->args->ty)) {
      error_tok(node->args->tok, "a non-floating point value as an argument");
    }
    gen_expr(node->args);
    println("\tldab @long");
    println("\tandb #$7f");
    println("\tstab @long");
    return true;
  }
  return false;
}

//
// copysignf(x, y):
//
bool builtin_copysignf(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "copysignf")
  && node->args && node->args->next) {
    if (!is_flonum(node->args->ty)) {
      return false;
    }
    if (!is_flonum(node->args->next->ty)) {
      return false;
    }
    Node   *base;
    int64_t off = 0;

    if ((base = find_base_var(node->args->next,&off))) {
      gen_expr(node->args);
      if (off == 0) {
        println("\tldab _%s", base->var->name);
      } else {
        println("\tldab _%s+%ld", base->var->name, off);
      }
      println("\tldaa @long");
      println("\tasla");
      println("\taslb");
      println("\trora");
      println("\tstaa @long");
      return true;
    }
    if (test_addr_x(node->args->next)) {
      gen_expr(node->args);
      int off = gen_addr_x(node->args->next);
      println("\tldab %d,x", off);
      println("\tldaa @long");
      println("\tasla");
      println("\taslb");
      println("\trora");
      println("\tstaa @long");
      return true;
    }
  }
  return false;
}
