int main(int argc, char **argv)
{
	int	i;

  while (1) {         // optimizer check
    i = 0;
    break;
  }
  while (0) {         // optimizer check
    i = 0;
  }

	while (i<1000){
		if(i==50)
			break;
		i++;
	}

	return (i!=50);
}
