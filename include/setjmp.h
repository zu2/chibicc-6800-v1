typedef unsigned char jmp_buf[6]; // PC,SP,@bp

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
