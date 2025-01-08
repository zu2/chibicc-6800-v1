int main(int argc, char **argv)
{
	int	i;

	i = 0;
	while (i<1000){
		if(i==50)
			break;
		i++;
	}

	return (i!=50);
}
