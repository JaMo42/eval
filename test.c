#include <stdio.h>
#include <string.h>
#include <float.h>
#include "eval.h"

#if __STDC_VERSION__ == 199901L
#  define DBL_DECIMAL_DIG 17
#endif

int main(int argc, const char **argv) {
  if (argc != 2) { return 1; }
  char *arg = (char *)argv[1] + 1;
  arg[strlen(arg)-1] = 0;
  printf("%.*f", DBL_DECIMAL_DIG, eval(arg));
  return 0;
}

