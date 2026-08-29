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
