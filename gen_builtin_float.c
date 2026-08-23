#include "chibicc.h"
#include <math.h>

//
// signbit(x):
//    return (x<0);
//
bool builtin_signbit(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "signbit")
  && node->args) {
    int64_t val;
    double  fval;

    if (is_integer_constant(node->args, &val)) {
      println("\tclra");
      if (val<0) {
        println("\tldab #1");
      }else{
        println("\tclrb");
      }
      return true;
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
    if (is_global_var(node->args)) {
      println("\tclra");
      println("\tldab _%s",node->args->var->name);
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
  }
  return false;
}

//
// copysignf(x, y):
//    return the value of x with the sign of y;
//
bool builtin_copysignf(Node *node)
{
  if (node->lhs->kind == ND_VAR
  && !strcmp(node->lhs->var->name, "copysignf")
  && node->args && node->args->next) {
    if (is_global_var(node->args->next)) {
      gen_expr(node->args);
      println("\tldab _%s", node->args->next->var->name);
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
