# eval

Arithmetic expression evalutaion in C/C++.

## Usage

```cpp
double eval(const char *expr);
```

Returns the result of `expr`.

```cpp
double eval_v(const char *expr, const EvalValues *values);
```

Returns the result of `expr`, with constants/variables from `values`.

```cpp
int eval_assign(const char *expression, const EvalValue *values);
```

Performs assignment to a variable from `values`.

Format of `expression`: `val = expr` (whitespace does not matter), where `val` recieves the result of evaluating `expr`.

### The `values` array

Example:

```cpp
const EvalValue values[] = {
  EvalConst("pi", 3.14159),
  EvalConst("e", 2.71828),
  EvalVar("argc", &argc),
  EvalEnd
};
```

- `EvalConst` Creates a constant value
- `EvalVar` Uses the value of the pointed to variable
- `EvalEnd` Terminates the array

## Features

- Operators: `+`, `-`, `*` and `/`
- Operator precedences
- Parentheses
- Variables and constants
- Assignment expressions
- Implicit multiplication with parentheses

## Operator precedence disambiguation

By default an expression like `48 / 2(9 + 3)` is interpreted as `48 / (2 * (9 + 3))`.
When compiling with `-DEVAL_DISAMBIG_ALT`, it gets changed to `(48 / 2) * (9 + 3)`.
