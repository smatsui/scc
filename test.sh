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

try 0 "0;"
try 42 "42;"
try 21 '5+20-4;'
try 41 ' 12 + 34  - 5;'
try 47 "5+6*7;"
try 30 "33-9/3;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 2 "a=2;"
try 3 "b=1+2;"
try 3 "a=2; b=1+2;"
try 4 "a=2; b=1+2; c=(3+5)/2;"
try 3 "a=b=(12+3)/5;"
try 6 "a=b=3; a+b;"
try 5 "abc=2; abc+3;"
try 5 "abc1=2; def2=3; abc1+def2;"
try 1 "0 == 0;"
try 0 "0 != 0;"
try 0 "1 == 0;"
try 1 "a=b=1; a==b;"
try 1 "a=1; b=2; a+2==b+1;"
try 0 "a=1; b=2; a+2==b+2;"
try 1 "a=1; b=2; a+2!=b+2;"
try 1 "a=1; b=2; a<b;"
try 0 "a=1; b=1; a<b;"
try 1 "a=2; b=1; a>b;"
try 0 "a=1; b=1; a>b;"
try 1 "a=1; b=1; a<=b;"
try 1 "a=1; b=1; a>=b;"
try 1 "1&1;"
try 0 "1&0;"
try 0 "0|0;"
try 1 "1|0;"
try 0 "1^1;"
try 1 "1^0;"
try 1 "0==0 && 1==1;"
try 0 "0==0 && 1!=1;"
try 1 "0==0 || 1!=1;"
try 0 "0==1 || 1!=1;"
try 5 "3; if(1==1) 5;"
try 0 "3; if(1!=1) 5;"
try 4 "3; if(1!=1) 5; 4;"
try 1 "if(1==1) 1; else 2;"
try 2 "if(1!=1) 1; else 2;"
try 100 "a=0; while(a<100) a=a+1;"
try 200 "for(a=0; a<200; a=a+1) a;"
try 23 "foo1();"
try 3 "foo2(3);"
try 3 "foo3(1, 2);"
echo OK
