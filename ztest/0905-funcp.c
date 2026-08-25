struct S { int a; int b; int c; };

typedef void     (*FuncPtr)(void);
typedef void     (**FuncPtr2)(void);
typedef float    (*fnf)(float);
typedef float    (*fnfi)(float, int);
typedef long     (*fnl)(long);
typedef float    (*fns)(struct S);
typedef struct S (*fnr)(float);
typedef int      (*fna)(int *);

int var;

static void bump(void) { var++; }
static void drop(void) { var--; }

static float f_id(float x)  { return x; }
static float f_dbl(float x) { return x + x; }
static float f_neg(float x) { return -x; }

static float fi_sub(float x, int n) { return x - n; }
static float fi_add(float x, int n) { return x + n; }

static long l_id(long x)  { return x; }
static long l_dbl(long x) { return x + x; }

static float s_sum(struct S s)  { return s.a + s.b + s.c; }
static float s_diff(struct S s) { return s.a - s.b - s.c; }

static struct S r_make(float x)
{
	struct S s;
	s.a = x;
	s.b = x + x;
	s.c = x + x + x;
	return s;
}

static struct S r_zero(float x)
{
	struct S s;
	s.a = 0;
	s.b = x;
	s.c = 0;
	return s;
}

static int a_sum(int *p)  { return p[0] + p[1] + p[2] + p[3]; }
static int a_head(int *p) { return p[0]; }

FuncPtr array[2] = { bump, drop };

static fnf  tf[4]  = { f_id, f_dbl, f_neg };
static fnfi tfi[4] = { fi_sub, fi_add };
static fnl  tl[4]  = { l_id, l_dbl };
static fns  ts[4]  = { s_sum, s_diff };
static fnr  tr[4]  = { r_make, r_zero };
static fna  ta[4]  = { a_sum, a_head };

int idx;
float gx;
long gl;
struct S gs;
struct S gr;
int arr[4];
float sink;

static void put_f(float x) { sink = x; }

int main(int argc, char **argv)
{
	FuncPtr func = bump;
	FuncPtr2 func2 = &func;

	var = 0;
	(*func)();
	func();
	(**func2)();
	(*func2)();
	if (var != 4) return 1;

	array[1]();
	idx = 1;
	array[idx]();
	if (var != 2) return 2;

	// 2.0 has an exact binary form, so == is safe here
	gx = 2.0f;

	if (tf[1](gx) != 4.0f) return 10;
	idx = 0;
	if (tf[idx](gx) != 2.0f) return 11;
	idx = 1;
	if (tf[idx](gx) != 4.0f) return 12;
	idx = 2;
	if (tf[idx](gx) != -2.0f) return 13;

	// the result of the call goes straight into another call
	idx = 1;
	sink = 0.0f;
	put_f(tf[idx](gx));
	if (sink != 4.0f) return 14;

	idx = 0;
	if (tfi[idx](gx, 1) != 1.0f) return 20;
	idx = 1;
	if (tfi[idx](gx, 3) != 5.0f) return 21;

	// the index and the second argument come from the same variable
	idx = 1;
	if (tfi[idx](gx, idx) != 3.0f) return 22;

	gl = 100000;
	idx = 0;
	if (tl[idx](gl) != 100000) return 30;
	idx = 1;
	if (tl[idx](gl) != 200000) return 31;

	gs.a = 10;
	gs.b = 3;
	gs.c = 2;
	idx = 0;
	if (ts[idx](gs) != 15.0f) return 40;
	idx = 1;
	if (ts[idx](gs) != 5.0f) return 41;
	if (gs.a != 10) return 42;
	if (gs.c != 2) return 43;

	idx = 0;
	gr = tr[idx](gx);
	if (gr.a != 2) return 50;
	if (gr.b != 4) return 51;
	if (gr.c != 6) return 52;
	idx = 1;
	gr = tr[idx](gx);
	if (gr.b != 2) return 53;
	if (gr.c != 0) return 54;

	arr[0] = 1;
	arr[1] = 2;
	arr[2] = 4;
	arr[3] = 8;
	idx = 0;
	if (ta[idx](arr) != 15) return 60;
	idx = 1;
	if (ta[idx](arr) != 1) return 61;

	// the caller must not damage its own arguments
	if (gx != 2.0f) return 70;
	if (gl != 100000) return 71;

	// a stack that does not balance shows up after many calls
	int i;
	int n = 0;
	float acc = 0.0f;
	i = 0;
	while (i < 10) {
		idx = 1;
		acc = acc + tf[idx](gx);
		idx = 0;
		gr = tr[idx](gx);
		n = n + gr.a + ta[idx](arr);
		i = i + 1;
	}
	if (acc != 40.0f) return 80;
	if (n != 170) return 81;
	if (i != 10) return 82;

	return 0;
}
