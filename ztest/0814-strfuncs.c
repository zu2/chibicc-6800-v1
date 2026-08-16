#include <string.h>
#include <stdlib.h>

char buf[40];
char big[520];
char big2[320];

int main(int argc, char **argv)
{
	char *p;

	// strstr
	if (strstr("hello world", "world") == NULL) return 1;
	if (strcmp(strstr("hello world", "world"), "world")) return 2;
	if (strcmp(strstr("hello world", ""), "hello world")) return 3;
	if (strcmp(strstr("hello world", "hello"), "hello world")) return 4;
	if (strstr("hello world", "worlds") != NULL) return 5;
	if (strstr("", "a") != NULL) return 6;
	if (strcmp(strstr("", ""), "")) return 7;
	if (strcmp(strstr("aaab", "aab"), "aab")) return 8;
	if (strcmp(strstr("abcabd", "abd"), "abd")) return 9;
	if (strstr("abc", "abcd") != NULL) return 10;
	if (strcmp(strstr("abc", "c"), "c")) return 11;

	// strncat
	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "ab");
	strncat(buf, "cdef", 3);
	if (strcmp(buf, "abcde")) return 20;
	if (buf[6] != 'Z') return 21;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "ab");
	strncat(buf, "cd", 10);
	if (strcmp(buf, "abcd")) return 22;
	if (buf[5] != 'Z') return 23;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "ab");
	strncat(buf, "cd", 0);
	if (strcmp(buf, "ab")) return 24;
	if (buf[3] != 'Z') return 25;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "");
	strncat(buf, "cd", 0);
	if (buf[0] != 0) return 26;
	if (buf[1] != 'Z') return 27;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "");
	strncat(buf, "xy", 2);
	if (strcmp(buf, "xy")) return 28;
	if (buf[3] != 'Z') return 29;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "ab");
	strncat(buf, "", 3);
	if (strcmp(buf, "ab")) return 30;
	if (buf[3] != 'Z') return 31;

	memset(buf, 'Z', sizeof(buf));
	strcpy(buf, "ab");
	strncat(buf, "cd", 2);
	if (strcmp(buf, "abcd")) return 32;
	if (buf[5] != 'Z') return 33;

	if (strncat(buf, "", 1) != buf) return 34;

	// strdup
	p = strdup("hello");
	if (p == NULL) return 60;
	if (strcmp(p, "hello")) return 61;
	p[0] = 'H';
	if (strcmp(p, "Hello")) return 62;
	free(p);

	p = strdup("");
	if (p == NULL) return 63;
	if (p[0] != 0) return 64;
	free(p);

	// Longer than 256 bytes
	memset(big, 'a', 300);
	big[300] = 'Z';
	big[301] = 0;
	if (strlen(big) != 301) return 40;
	if (strstr(big, "aZ") != big + 299) return 41;
	if (strstr(big, "Zb") != NULL) return 42;

	memset(big2, 'a', 299);
	big2[299] = 'Z';
	big2[300] = 0;
	if (strstr(big, big2) != big + 1) return 43;
	big2[298] = 'Y';
	if (strstr(big, big2) != NULL) return 44;

	memset(big, 'b', 200);
	big[200] = 0;
	memset(big2, 'c', 300);
	big2[300] = 0;
	big[501] = 'Y';
	strncat(big, big2, 400);
	if (strlen(big) != 500) return 45;
	if (big[199] != 'b') return 46;
	if (big[200] != 'c') return 47;
	if (big[499] != 'c') return 48;
	if (big[500] != 0) return 49;
	if (big[501] != 'Y') return 50;

	memset(big, 'b', 10);
	big[10] = 0;
	big[311] = 'X';
	strncat(big, big2, 300);
	if (strlen(big) != 310) return 51;
	if (big[310] != 0) return 52;
	if (big[311] != 'X') return 53;

	memset(big, 'd', 400);
	big[400] = 0;
	p = strdup(big);
	if (p == NULL) return 54;
	if (strlen(p) != 400) return 55;
	if (strcmp(p, big)) return 56;
	free(p);

	return 0;
}
