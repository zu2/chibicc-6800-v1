#include "common.h"

int main(int argc, char **argv)
{
	int	i;

	for (i=0; i<1000; i++){
		if(i==50)
			break;
	}

	cpu_counter();
	return (i!=50);
}
