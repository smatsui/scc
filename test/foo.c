#include <stdio.h>

int foo1(){
  return 23;
}

int foo2(int a){
  printf("%d\n", a);
  return a;
}

int foo3(int a, int b){
  printf("%d\n", a+b);
  return a+b;
}
