#include "chibicc.h"

void load_bitfield(Node *node)
{
  int off;
  Member *mem = node->member;

  if (!mem->is_bitfield) {
    assert(0);
  }
  if (can_load_x(node->ty) && test_addr_x(node)) {
    off = gen_addr_x(node, false);
    load_x(ty_uint, off);
  } else {
    gen_addr(node);
    load(ty_uint);
  }
  println("; bitfield mem->bit_width=%d, mem->bit_offset=%d, %s, %s %d",
          mem->bit_width, mem->bit_offset, (mem->ty->is_unsigned ? "u" : "i"),
          __FILE__, __LINE__);
  //    println(";  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);
  gen_shr(ty_uint, mem->bit_offset);
  unsigned int mask = (unsigned int)(1L << mem->bit_width) - 1;
  and_i(mask & 0xffff);
  if (!mem->ty->is_unsigned && mem->bit_width != 16) {
    char *label = new_jump_label();
    if (mem->bit_width <= 8) {
      println("\tbitb #<$%04x", (int)(1L << (mem->bit_width - 1)));
    } else {
      println("\tbita #<$%04x", (int)(1L << (mem->bit_width - 9)));
    }
    println("\tbeq %s", label);
    if (mem->bit_width <= 8) {
      if (mem->bit_width != 8) {
        println("\torab #<$%04x", (~mask) & 0xffff);
      }
      println("\toraa #$ff");
    } else {
      println("\toraa #>$%04x", (~mask) & 0xffff);
    }
    println("%s:", label);
  }
  return;
}

void assign_to_bitfield(Node *node)
{
  int64_t val;

  if (!node->lhs->kind == ND_MEMBER || !node->lhs->member->is_bitfield) {
    assert(0);
  }
  gen_addr(node->lhs);
  push();
  gen_expr(node->rhs);
  popx();
  if (!node->retval_unused) {
    push();
  }

  // If the lhs is a bitfield, we need to read the current value
  // from memory and merge it with a new value.
  Member *mem = node->lhs->member;
  println("; bitfieled mem->ty->size=%d, mem->bit_width=%d, "
          "mem->bit_offset=%d, %s, %s %d",
          mem->ty->size, mem->bit_width, mem->bit_offset,
          (mem->ty ? (mem->ty->is_unsigned ? "u" : "i") : "null"), __FILE__,
          __LINE__);
  and_i((unsigned short)(1L << mem->bit_width) - 1);
  gen_shl(ty_uint, mem->bit_offset);
  uint16_t mask = ((1L << mem->bit_width) - 1) << mem->bit_offset;
  println("\teorb 1,x");
  println("\teora 0,x");
  and_i(mask);
  println("\teorb 1,x");
  println("\teora 0,x");
  store_x(ty_uint, 0);
  if (!node->retval_unused) {
    pop();
  }
  return;
}
