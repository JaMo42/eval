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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eval.h"

struct stack {
    union stack_item {
        char op;
        double number;
    } values[4];
    int size;
};

static inline union stack_item
stk_top(struct stack *stk) {
  return stk->values[stk->size - 1];
}

static inline void
stk_push_num(struct stack *stk, double number) {
  stk->values[stk->size++].number = number;
}

static inline void
stk_push_op(struct stack *stk, char ch) {
  stk->values[stk->size++].op = ch;
}

static inline union stack_item
stk_pop(struct stack *stk) {
    return stk->values[--stk->size];
}

static inline int
stk_size(struct stack *stk) {
    return stk->size;
}

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
eval_execute(struct stack *numbers, struct stack *operators) {
  double n2 = stk_pop(numbers).number;
  double n1 = stk_pop(numbers).number;
  char op = stk_pop(operators).op;
  switch (op) {
  case '+': stk_push_num(numbers, n1 + n2); break;
  case '-': stk_push_num(numbers, n1 - n2); break;
  case '*': stk_push_num(numbers, n1 * n2); break;
  case '/': stk_push_num(numbers, n1 / n2); break;
  }
}

static double
eval_internal(const char *expr, const EvalValue *values) {
  struct stack numbers;
  struct stack operators;
  numbers.size = operators.size = 0;
  double ret = 0.0;

  int previous_token_was_operator = 1;
  char ch, *end;
  for (const char *p = expr; *p; ++p) {
  repeat:
    ch = *p;

    if (isspace(ch)) {
    }
    else if (isdigit(ch) || ch == '.'
             || (ch == '-' && previous_token_was_operator)) {
      stk_push_num(&numbers, strtod(p, &end));
      p = end - 1;
      previous_token_was_operator = 0;
    }
    else if ((isalpha(ch) || ch == '_') && values) {
      const char *begin = p;
      while (isalnum(*p) || *p == '_') {
        ++p;
      }
      const EvalValue *v;
      for (v = values; v->name; ++v) {
        if (!strncmp(v->name, begin, p - begin)) {
          stk_push_num(&numbers, v->type ? v->v.constant : *(v->v.variable));
          previous_token_was_operator = 0;
          break;
        }
      }
      if (!v->name) {
        fprintf(
          stderr,
          "eval_v: value does not exist `%.*s'\n",
          (int)(p - begin),
          begin
        );
        goto end;
      }
      --p;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
      if (stk_size(&operators) == 0) {
        stk_push_op(&operators, ch);
        previous_token_was_operator = 1;
      }
      else if (precedence(ch) > precedence(stk_top(&operators).op)) {
        stk_push_op(&operators, ch);
        previous_token_was_operator = 1;
      }
      else {
        eval_execute(&numbers, &operators);
        goto repeat;
      }
    }
    else if (ch == '(') {
      // How to disambiguate something like "48/2(9+3)"
#ifndef EVAL_DISAMBIG_ALT
      // "48/(2 * (9+3))"
      if (!previous_token_was_operator)
        stk_push_op(&operators, '*');
#else
      // "(48/2)(9+3)"
      if (!prev) {
        if(stk_size(&numbers) > 1 && stk_size(&operators) > 0)
          eval_execute(&numbers, &operators);
        stk_push_op(&operators, '*');
      }
#endif
      stk_push_num(&numbers, eval_internal(p+1, values));
      // Skip to end of this parenthesis
      int depth = 1;
      while (depth > 0) {
        ++p;
        if (*p == '(') { ++depth; }
        else if (*p == ')') { --depth; }
      }
      previous_token_was_operator = 0;
    }
    else if (ch == ')') {
      break;
    }
    else {
      fprintf(
        stderr,
        "eval: invalid character in expression -- %c (at index %d)\n",
        ch,
        (int)(p - expr)
      );
      goto end;
    }
  }

  while (stk_size(&operators) > 0)
    eval_execute(&numbers, &operators);
  ret = numbers.values[0].number;

end:
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
        fprintf(
          stderr,
          "eval_assign: target value does not exist `%.*s'\n",
          (int)(p - name),
          name
        );
        return 0;
      } else if (target->type == EVAL_CONST) {
        fprintf(
          stderr,
          "eval_assign: target value is a constant `%.*s'\n",
          (int)(p - name),
          name
        );
        return 0;
      }
    }
    else {
      fprintf(
        stderr,
        "eval_assign: unexpected character in expression -- %c (%d)\n",
        *p,
        (int)(p - expr)
      );
      return 0;
    }
  }
  *target->v.variable = eval_internal(eq_pos + 1, values);
  return 1;
}

