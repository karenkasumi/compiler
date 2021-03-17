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
assert 10 "a=10;b=10;b;"
echo OK