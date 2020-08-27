/*
Copyright (c) 2020 Jakob Mohrbacher

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <ctype.h>
#include "eval.h"
#include "vector.h"

static int
precedence(char op) {
  switch (op) {
  case '+':
  case '-':
    return 0;
  case '*':
  case '/':
    return 1;
  }
  return -1;
}

static void
eval_execute(double *numbers, char *operators) {
  double n2 = vector_pop(numbers);
  double n1 = vector_pop(numbers);
  char op = vector_pop(operators);
  switch (op) {
  case '+': vector_push(numbers, n1 + n2); break;
  case '-': vector_push(numbers, n1 - n2); break;
  case '*': vector_push(numbers, n1 * n2); break;
  case '/': vector_push(numbers, n1 / n2); break;
  }
}

static double
eval_internal(const char *expr) {
  double *numbers = vector_create(double, 3);
  char *operators = vector_create(char, 2);
  double ret = 0.0;

  int depth = 1;
  int prev = 1;
  char ch, *end;
  for (const char *p = expr; *p && depth > 0; ++p) {
  repeat:
    ch = *p;

    if (isspace(ch)) {
    }
    else if (isdigit(ch) || ch == '.' || (ch == '-' && prev)) {
      vector_push(numbers, strtod(p, &end));
      p = end - 1;
      prev = 0;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      if (vector_size(operators) == 0) {
        vector_push(operators, ch);
        prev = 1;
      }
      else if (precedence(ch) > precedence(*vector_back(operators))) {
        vector_push(operators, ch);
        prev = 1;
      }
      else {
        eval_execute(numbers, operators);
        goto repeat;
      }
    }
    else if (ch == '(') {
      vector_push(numbers, eval_internal(p+1));
      // Skip to end of this parenthesis
      int ldepth = 1;
      while (ldepth > 0) {
        ++p;
        if (*p == '(') { ++ldepth; }
        else if (*p == ')') { --ldepth; }
      }
      prev = 0;
    }
    else if (ch == ')') {
      --depth;
    }
    else {
      fprintf(stderr, "eval: invalid character in expression -- %c (%d)\n",
        ch, (int)(p - expr));
      goto end;
    }
  }

  while (vector_size(operators) > 0)
    eval_execute(numbers, operators);
  ret = numbers[0];

end:
  vector_free(numbers);
  vector_free(operators);
  return ret;
}

double
eval(const char *expr) {
  return eval_internal(expr);
}

