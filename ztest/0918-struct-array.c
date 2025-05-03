typedef struct {
	int x;
	int array[1];
} S;

int func(S *s) {
	s->x++;
	return s->array[0];
}

int
main(int argc, char **argv)
{
	S	s;

	s.array[0] = 1;

	return func(&s)!=1;
}
