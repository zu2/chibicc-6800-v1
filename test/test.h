#define ASSERT(x, y) assert(x, y, #y)

void assert(int expected, int actual, char *code);
int printf(char *fmt, ...);
int sprintf(char *buf, char *fmt, ...);
int vsprintf(char *buf, char *fmt, void *ap);
int strcmp(char *p, char *q);
int strncmp(char *p, char *q, long n);
int memcmp(char *p, char *q, long n);
void exit(int n);
int vsprintf();
long strlen(char *s);
void *memcpy(void *dest, void *src, long n);
void *memset(void *s, int c, long n);

extern	void print(int x);
extern	void putchar(int ch);

void putstr(unsigned char *s)
{
	while (*s) {
		putchar(*s);
		s++;
	}
}
void assert(int expected, int actual, char *code) {
  if (expected == actual) {
	putstr(code);putstr(" => ");print(actual);
//    printf("%s => %d\n", code, actual);
  } else {
	putstr(code);putstr(" => ");print(expected);
	putstr(" expected but got ");print(actual);
//    printf("%s => %d expected but got %d\n", code, expected, actual);
//    exit(1);
  }
}

#define	printf(x)	putstr(x)
