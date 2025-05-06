int var=1;

int main(int argc, char **argv)
{
	int foo=0;

	if (foo<0) {
	  var++;
	}
	if (var<0) {
	  return 1;
	}
	if (!var) {
	  return 2;
	}
	return var<0;
}
