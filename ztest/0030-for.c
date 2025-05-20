#include "common.h"

int main(int argc, char **argv)
{
	int	i;

  for (;;) {                // optimizer check
    i = 0;
    break;
  }
  for (;1;) {                // optimizer check
    i = 0;
    break;
  }
  for (;0;) {                // optimizer check
  }
	for (i=0; i<1000; i++){
		if(i==50)
			break;
	}

	cpu_counter();
	return (i!=50);
}
