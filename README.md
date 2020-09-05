# eval

Arithmetic expression evalutaion in C/C++.

## Usage

```cpp
double eval(const char *expr);
```

Returns the result of `expr`.

```
double veval(const char *expr, const EvalValues *values);
```

Returns the result of `expr` with constants/variables from `values`.

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

- `EvalConst` Creates a constant values
- `EvalVar` Uses the value of the pointed to variable
- `EvalEnd` Terminates the array

```

## Features

- Operators: `+`, `-`, `*` and `/`
- Operator precedences
- Parentheses
- Variables and constants

## Requirements

- https://github.com/JaMo42/c-vector
