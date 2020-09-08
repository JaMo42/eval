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
#ifndef EVAL_H
#define EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

enum EvalValueType { EVAL_VAR, EVAL_CONST };

typedef struct {
  const char *name;
  union {
    double *variable;
    double constant;
  };
  enum EvalValueType type;
} EvalValue;

#define EvalVar(n, v) { .name = n, .variable = v, .type = EVAL_VAR }
#define EvalConst(n, c) { .name = n, .constant = c, .type = EVAL_CONST }
#define EvalEnd { .name = NULL, .variable = NULL, .type = EVAL_VAR }

/**
 * @brief Parses simple arithmetic expression.
 */
double
eval(const char *expression);

/**
 * @brief Parses arithmetic expression with constants and varaibles.
 */
double
eval_v(const char *expression, const EvalValue *values);

/**
 * @brief Parses arithmetic expression with assignment.
 * Expected format: "val = expr" (whitespace does not matter).
 */
int
eval_assign(const char *assignment, const EvalValue *values);

#ifdef __cplusplus
}
#endif

#endif
