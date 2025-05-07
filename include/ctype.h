// ctype.h for MC6800 C compiler
// Character classification and conversion functions

#ifndef _CTYPE_H_
#define _CTYPE_H_

// Return nonzero if c is a digit ('0'..'9')
int isdigit(int c);

// Return nonzero if c is a lowercase letter ('a'..'z')
int islower(int c);

// Return nonzero if c is an uppercase letter ('A'..'Z')
int isupper(int c);

// Return nonzero if c is a letter ('A'..'Z' or 'a'..'z')
int isalpha(int c);

// Return nonzero if c is a letter or digit
int isalnum(int c);

// Return nonzero if c is a hexadecimal digit ('0'..'9', 'A'..'F', 'a'..'f')
int isxdigit(int c);

// Return nonzero if c is a space, tab, newline, etc.
int isspace(int c);

// Return nonzero if c is a printable character
int isprint(int c);

// Return nonzero if c is a punctuation character
int ispunct(int c);

// Return nonzero if c is a control character
int iscntrl(int c);

// Return nonzero if c is a blank (space or tab)
int isblank(int c);

// Convert c to lowercase if possible
int tolower(int c);

// Convert c to uppercase if possible
int toupper(int c);

#endif // _CTYPE_H_
