int main(int argc, char **argv)
{
	unsigned int ui=0;
	signed   int si=0;
	unsigned int ua[10];
	signed   int sa[10];
	unsigned int *up;
	unsigned int *sp;
	int	i;
	
	if (ui++)
		return 1;
	if (--ui)
		return 2;
	++ui;
	if (--ui)
		return 3;
	--ui;
	if (++ui)
		return 4;
	ui++;
	if (--ui)
		return 5;
	ui--;
	if (++ui)
		return 6;


	if (si++)
		return 21;
	if (--si)
		return 22;
	++si;
	if (--si)
		return 23;
	--si;
	if (++si)
		return 24;
	si++;
	if (--si)
		return 25;
	si--;
	if (++si)
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
