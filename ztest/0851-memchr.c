#include	"common.h"

extern	void	*memchr(const void *s, int c, int n);
extern	int	strlen(const char *s);

int	main(int argc, char **argv)
{
	char	*s = "Hello, World";
	int	len = strlen(s);

	if (memchr(s,'H',len) != s)
	  return 1;

	if (memchr(s,',',len) != s+5)
	  return 2;

	if (memchr(s,'d',len) != s+11)
	  return 3;

	if (memchr(s,'z',len) != 0)
	  return 4;

	return	0;
}

