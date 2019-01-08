#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./scc "$input" > tmp.s
  gcc -o tmp tmp.s
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
echo OK
