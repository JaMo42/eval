#!/usr/bin/env python3
import random
import subprocess

OPERATORS = [" + ", " - ", " * ", " / "]
DIGITS = "0123456789"

def generate_number():
  n = 0
  while n == 0:
    n = random.randint(-100, 100)
  return str(n)

def generate_expression():
  length = random.randrange(3, 10)
  expr = []

  n_parens = 0
  p_paren = 0.2

  for i in range(length):
    if random.random() < p_paren:
      expr.append('(')
      n_parens += 1

    expr.append(generate_number())

    if random.random() < (p_paren * n_parens):
      expr.append(')')
      n_parens -= 1

    expr.append(OPERATORS[random.randrange(4)])

  expr.pop()

  for i in range(n_parens):
    expr.append(')')

  return ''.join(expr)

if __name__ == "__main__":
  print("RESULT == CONTROL")
  for i in range(100):
    expr = generate_expression()
    control = eval(expr)
    proc = subprocess.run(["./test", f"'{expr}'"], stdout=subprocess.PIPE)
    result = proc.stdout.decode('utf-8')
    print(f"{result} == {control}")
    try:
      assert(float(control) - float(result) < 1e-10)
    except AssertionError as e:
      print("FAILED:")
      print(expr)
      exit(1)
