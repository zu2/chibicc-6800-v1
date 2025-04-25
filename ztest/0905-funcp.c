typedef void (*FuncPtr)(void);
FuncPtr array[1];

void func(void) {
	array[0]();
}

int main(int argc, char **argv)
{
	return 0;
}
