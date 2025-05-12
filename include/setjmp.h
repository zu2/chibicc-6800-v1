typedef unsigned char jmp_buf[9]; // CC,A,B,IX,PC,SP

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
