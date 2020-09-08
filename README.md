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
void eval_assign(const char *expression, const EvalValue *values);
```

Performs assignment to a variable from `values`.

Format of `expression`: `val = expr` (whitespace does not matter), where `val` recieves the result of evaluating `expr`.

### The `values` array

Example:

```cpp
const EvalValues values[] = {
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

## Requirements

- https://github.com/JaMo42/c-vector
