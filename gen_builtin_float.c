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

    println("; ND_FUNCALL: builtin_signbit");
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
    // The MSB is the sign bit for all types.
    if (is_global_var(node->args)) {
      println("\tclra");
      println("\tldab _%s",node->args->var->name);
      println("\tandb #$80");
      return true;
    }
    if (test_addr_x(node->args)) {
      int off = gen_addr_x(node->args,false);
      println("\tclra");
      println("\tldab %d,x",off);
      println("\tandb #$80");
      return true;
    }
  }
  return false;
}
