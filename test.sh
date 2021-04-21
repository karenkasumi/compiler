#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./C "$input" > compile.s
  cc -o compile compile.s
  ./compile
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 1 '10==10;'
assert 1 "1!=0;"
assert 10 "a=10;b=10;b;"
assert 0 "if(1==1)return 0;"
assert 1 "if(1==2)return 0;else return 1;"
assert 0 "i=0;while(i==1)return 1;i;"
assert 10 "i=0;while(i!=10)i=i+1;i;"
assert 2 "a=0;for(i=1;i!=3;i=i+1)a=a+1;a;"
assert 2 "{a=1;b=1;}a+b;"
echo OK