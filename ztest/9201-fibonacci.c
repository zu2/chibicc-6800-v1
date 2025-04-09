#include "common.h"

int fibonacci(int n){
    if(n <= 1){
        return n;
    }
    return fibonacci(n-1)+fibonacci(n-2);
} 

int	fibo[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711 };

int main(int argc,char **argv)
{
    if (fibonacci(0)!=0)
      return -1;

    for (int i=1; i<23; i++){
      print(i);
//    print(fibo[i]);
      if (fibonacci(i)!=fibo[i])
        return i;
    }

    return 0;
}
