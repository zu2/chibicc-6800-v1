//
// ix_tracking.c
//

#include "chibicc.h"

IX_Type	IX_Dest = IX_None;
int IX_PTR_off = -1;
char *IX_EXT_var = "";
char *IX_IMM_str = "";
int   IX_IMM_val = -1;

void IX_invalidate(void)
{
  IX_Dest = IX_None;
}

IX_State IX_save(void)
{
  IX_State s;

  s.dest    = IX_Dest;
  s.ptr_off = IX_PTR_off;
  s.ext_var = IX_EXT_var;
  s.imm_str = IX_IMM_str;
  s.imm_val = IX_IMM_val;

  return s;
}

void IX_restore(IX_State s)
{
  IX_Dest    = s.dest;
  IX_PTR_off = s.ptr_off;
  IX_EXT_var = s.ext_var;
  IX_IMM_str = s.imm_str;
  IX_IMM_val = s.imm_val;
}

//
// IX (X register) tracking.
//
// X on MC6800:
//   There is one index register, so every off,X access goes through it:
//   locals via bp, pointer dereferences, struct members, array elements.
//   X is also the only 16-bit register, and the only place a pointer can be
//   held as a single value.
//
//   What X can do by itself is load, store, step by one, and a compare that
//   is only good for equality.  Arithmetic happens in A:B, and there is no
//   transfer between the two: a value worked out in A:B reaches X by way of
//   memory (see tfr_dx()).
//
// Why track it:
//   MC6800 has no frame pointer register, so the frame pointer lives in
//   memory as @bp and every local access starts with ldx @bp.  bp does not
//   change within a function, so one load can cover a run of accesses (tsx
//   when the frame allows it - see ldx_bp()).
//
//   Pointers work the same way one level out.  In p->x = 0; p->y = 0; the
//   second statement wants the X the first one already set up.  IX_BP,
//   IX_PTR and IX_EXT cover these cases.
//
//   A and B are not tracked.  Arithmetic happens there, so nearly every
//   instruction overwrites one of them; nothing stays put long enough to be
//   worth remembering.
//
//   When in doubt, invalidate.  Forgetting to emits wrong code; doing it
//   needlessly only costs size and speed.
//
// Why so few hooks are enough:
//   IX_Dest has to describe the actual contents of X wherever it is
//   consulted.  A write to memory can leave it stale, and on MC6800 there
//   are only two ways to write:
//
//     through X - the address has to be in X, so anything we were tracking
//       was discarded back when that address was loaded.
//     extended, to a symbol - X is untouched, so nothing notices on its own.
//
//   Only the second needs a hook.  That is what invalidate_EXT() is.
//
// So, when emitting code:
//
//   anything that changes X      -> IX_invalidate()
//   extended store to _sym       -> invalidate_EXT(), whatever the width
//   indexed store, runtime call  -> nothing to do
//
// Never tracked: arrays and aggregates (no contents, only an address), and
// numeric absolute addresses.
//
