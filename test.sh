#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./scc "$input" > tmp.s
  cc -c -o tmp.o tmp.s
  cc -c -o foo.o test/foo.c
  cc -o tmp tmp.o foo.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 "main(){0;}"
try 42 "main(){42;}"
try 21 'main(){5+20-4;}'
try 41 'main(){ 12 + 34  - 5; }'
try 47 "main(){5+6*7;}"
try 30 "main(){33-9/3;}"
try 15 "main(){5*(9-6);}"
try 4 "main(){(3+5)/2;}"
try 2 "main(){a=2;}"
try 3 "main(){b=1+2;}"
try 3 "main(){a=2; b=1+2;}"
try 4 "main(){a=2; b=1+2; c=(3+5)/2;}"
try 3 "main(){a=b=(12+3)/5;}"
try 6 "main(){a=b=3; a+b;}"
try 5 "main(){abc=2; abc+3;}"
try 5 "main(){abc1=2; def2=3; abc1+def2;}"
try 1 "main(){0 == 0;}"
try 0 "main(){0 != 0;}"
try 0 "main(){1 == 0;}"
try 1 "main(){a=b=1; a==b;}"
try 1 "main(){a=1; b=2; a+2==b+1;}"
try 0 "main(){a=1; b=2; a+2==b+2;}"
try 1 "main(){a=1; b=2; a+2!=b+2;}"
try 1 "main(){a=1; b=2; a<b;}"
try 0 "main(){a=1; b=1; a<b;}"
try 1 "main(){a=2; b=1; a>b;}"
try 0 "main(){a=1; b=1; a>b;}"
try 1 "main(){a=1; b=1; a<=b;}"
try 1 "main(){a=1; b=1; a>=b;}"
try 1 "main(){1&1;}"
try 0 "main(){1&0;}"
try 0 "main(){0|0;}"
try 1 "main(){1|0;}"
try 0 "main(){1^1;}"
try 1 "main(){1^0;}"
try 1 "main(){0==0 && 1==1;}"
try 0 "main(){0==0 && 1!=1;}"
try 1 "main(){0==0 || 1!=1;}"
try 0 "main(){0==1 || 1!=1;}"
try 5 "main(){3; if(1==1) 5;}"
try 0 "main(){3; if(1!=1) 5;}"
try 4 "main(){3; if(1!=1) 5; 4;}"
try 1 "main(){if(1==1) 1; else 2;}"
try 2 "main(){if(1!=1) 1; else 2;}"
try 100 "main(){a=0; while(a<100) a=a+1;}"
try 200 "main(){for(a=0; a<200; a=a+1) a;}"
try 23 "main(){foo1();}"
try 3 "main(){foo2(3);}"
try 3 "main(){foo3(1, 2);}"
try 2 "main(){bar();} bar(){2;}"
try 5 "main(){bar();} bar(){a=2;b=3; a+b;}"
echo OK
