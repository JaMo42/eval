#include <stdio.h>
#include <string.h>
#include <float.h>
#include "eval.h"

int main(int argc, const char **argv) {
  if (argc != 2) { return 1; }
  char *arg = (char *)argv[1] + 1;
  arg[strlen(arg)-1] = 0;
  printf("%.*f", DBL_DECIMAL_DIG, eval(arg));
  return 0;
}

