// stdio.h for MC6800 C compiler
// Standard I/O function prototypes

#ifndef _STDIO_H_
#define _STDIO_H_

// File type (dummy struct definition)
//typedef struct _FILE FILE;

// Standard streams
//extern FILE *stdin;
//extern FILE *stdout;
//extern FILE *stderr;

// Buffer size
#define BUFSIZ 128

// End-of-file indicator
#define EOF (-1)

#define NULL ((void *)0)

// Character input/output
int putchar(int c);
//int getchar(void);

// String input/output
int puts(const char *s);
//char *gets(char *s);

// Formatted output
int printf(const char *format, ...);

// File operations (implement as needed)
//FILE *fopen(const char *filename, const char *mode);
//int fclose(FILE *stream);
//int fputc(int c, FILE *stream);
//int fgetc(FILE *stream);
//int fputs(const char *s, FILE *stream);
//char *fgets(char *s, int n, FILE *stream);

// File positioning
//int fseek(FILE *stream, long offset, int whence);
//long ftell(FILE *stream);
//void rewind(FILE *stream);

// Error handling
//void clearerr(FILE *stream);
//int feof(FILE *stream);
//int ferror(FILE *stream);

#endif // _STDIO_H_
