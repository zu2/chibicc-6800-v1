typedef void (*FuncPtr)(void);
typedef void (**FuncPtr2)(void);

int var;
static void sub(void) { var++; }

int main(void) {
	FuncPtr func = sub;
	FuncPtr2 func2 = &func;

	(*func)();
	func();
	(**func2)();
	(*func2)();
	return 0;
}
