int main(int argc, char **argv)
{
	int	i;

  do {
	  i = 0;
  } while (0);    // optimizer check

  do {
    if (i==50)
      break;
    i++;
  } while (1);    // optimizer check

  i = 0;
	do {
		if(i==50)
			break;
		i++;
	} while (i<1000);

	return (i!=50);
}
