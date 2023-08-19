CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
SANFLAGS := -fsanitize=undefined,alignment,address

test: eval.c test.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: sanitize
sanitize: eval.c test.c
	clang $(CFLAGS) $(SANFLAGS) -o test $^
