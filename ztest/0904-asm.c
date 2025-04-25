int var=0;

void func(void) {
	if (var >= 0)
		while (++var < 10)
			asm("nop");
	else var = 0;
}

int main(int argc, char **argv)
{
	func();
	return 0;
}
