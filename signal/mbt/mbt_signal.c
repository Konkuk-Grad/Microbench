#include "mbt_signal.h"

int add(int a, int b){
    printf("sub2(%d, %d): %d\n", a, b, sub(a, b));
    return a + b;
}