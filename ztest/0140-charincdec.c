int main(int argc, char **argv)
{
	unsigned char uc=0;
	signed   char sc=0;
	unsigned char ua[10];
	signed   char sa[10];
	unsigned char *up;
	unsigned char *sp;
	int	i;

	if (uc++)
		return 1;
	if (--uc)
		return 2;
	++uc;
	if (--uc)
		return 3;
	--uc;
	if (++uc)
		return 4;
	uc++;
	if (--uc)
		return 5;
	uc--;
	if (++uc)
		return 6;


	if (sc++)
		return 21;
	if (--sc)
		return 22;
	++sc;
	if (--sc)
		return 23;
	--sc;
	if (++sc)
		return 24;
	sc++;
	if (--sc)
		return 25;
	sc--;
	if (++sc)
		return 26;


	for (i=0; i<10; i++) {
	   ua[i] = sa[i] = 0;
	}

	if (ua[1]++)
		return 41;
	if (--ua[1])
		return 42;
	++ua[1];
	if (--ua[1])
		return 43;
	--ua[1];
	if (++ua[1])
		return 44;
	ua[1]++;
	if (--ua[1])
		return 45;
	ua[1]--;
	if (++ua[1])
		return 46;


	if (sa[1]++)
		return 61;
	if (--sa[1])
		return 62;
	++sa[1];
	if (--sa[1])
		return 63;
	--sa[1];
	if (++sa[1])
		return 64;
	sa[1]++;
	if (--sa[1])
		return 65;
	sa[1]--;
	if (++sa[1])
		return 66;

	up = ua[0];
	sp = sa[0];
	*up++ = *sp++;

	return (sa[1]+=1)-1;
}
