#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" != "$expected" ]; then
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
echo OK
