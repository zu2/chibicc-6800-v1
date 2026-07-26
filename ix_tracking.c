//
//
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
