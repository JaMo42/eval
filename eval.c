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
eval_internal(const char *expr, const EvalValue *values) {
  double *numbers = vector_create(double, 4);
  char *operators = vector_create(char, 3);
  double ret = 0.0;

  int prev = 1;  // Previous token was an operator
  char ch, *end;
  for (const char *p = expr; *p; ++p) {
  repeat:
    ch = *p;

    if (isspace(ch)) {
    }
    else if (isdigit(ch) || ch == '.' || (ch == '-' && prev)) {
      vector_push(numbers, strtod(p, &end));
      p = end - 1;
      prev = 0;
    }
    else if ((isalpha(ch) || ch == '_') && values) {
      const char *begin = p;
      while (isalnum(*p) || *p == '_') {
        ++p;
      }
      const EvalValue *v;
      for (v = values; v->name; ++v) {
        if (!strncmp(v->name, begin, p - begin)) {
          vector_push(numbers, v->type ? v->constant : *(v->variable));
          prev = 0;
          break;
        }
      }
      if (!v->name) {
        fprintf(stderr, "eval_v: value does not exist `%.*s'\n", (int)(p - begin), begin);
        goto end;
      }
      --p;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      if (vector_size(operators) == 0) {
        vector_push(operators, ch);
        prev = 1;
      }
      else if (precedence(ch) > precedence(vector_back(operators))) {
        vector_push(operators, ch);
        prev = 1;
      }
      else {
        eval_execute(numbers, operators);
        goto repeat;
      }
    }
    else if (ch == '(') {
      // How to disambiguate something like "48/2(9+3)"
#ifndef EVAL_DISAMBIG_ALT
      // "48/(2 * (9+3))"
      if (!prev)
        vector_push(operators, '*');
#else
      // "(48/2)(9+3)"
      if (!prev) {
        if(vector__size(numbers) > 1 && vector__size(operators) > 0)
          eval_execute(numbers, operators);
        vector_push(operators, '*');
      }
#endif
      vector_push(numbers, eval_internal(p+1, values));
      // Skip to end of this parenthesis
      int depth = 1;
      while (depth > 0) {
        ++p;
        if (*p == '(') { ++depth; }
        else if (*p == ')') { --depth; }
      }
      prev = 0;
    }
    else if (ch == ')') {
      break;
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
  return eval_internal(expr, NULL);
}

static int
eval_check_values(const char *caller, const EvalValue *values) {
  int err = 0;
  for (const EvalValue *v = values; v->name != NULL; ++v) {
    if (!(isalpha(v->name[0]) || v->name[0] == '-')) {
      goto error;
    } else {
    }
    for (const char *p = v->name+1; *p; ++p) {
      if (!(isalnum(*p) || *p == '_')) {
        goto error;
      }
    }
    continue;
error:
    fprintf(stderr, "%s: invalid name `%s'\n", caller, v->name);
    err = 1;
  }
  return err;
}

double
eval_v(const char *expr, const EvalValue *values) {
  if (eval_check_values("eval_v", values)) {
    return 0.0;
  }
  return eval_internal(expr, values);
}

int
eval_assign(const char *expr, const EvalValue *values) {
  if (eval_check_values("eval_assign", values)) {
    return 0;
  }
  const char *name;
  const EvalValue *target = NULL;
  const char *eq_pos = strchr(expr, '=');
  if (eq_pos == NULL) {
    fprintf(stderr, "eval_assign: no assignment\n");
    return 0;
  }

  for (const char *p = expr; p < eq_pos; ++p) {
    if (isspace(*p)) {
    }
    else if (isalpha(*p) || *p == '_') {
      if (target != NULL) {
        fprintf(stderr, "eval_assign: multiple target values\n");
        return 0;
      }
      name = p;
      while (isalnum(*p) || *p == '_') {
        ++p;
      }
      for (target = values; target->name; ++target) {
        if (!strncmp(target->name, name, p - name)) {
          break;
        }
      }
      if (!target->name) {
        fprintf(stderr, "eval_assign: target value does not exist `%.*s'\n", (int)(p - name), name);
        return 0;
      } else if (target->type == EVAL_CONST) {
        fprintf(stderr, "eval_assign: target value is a constant `%.*s'\n", (int)(p - name), name);
        return 0;
      }
    }
    else {
      fprintf(stderr, "eval_assign: unexpected character in expression -- %c (%d)\n", *p, (int)(p - expr));
      return 0;
    }
  }
  *target->variable = eval_internal(eq_pos + 1, values);
  return 1;
}

