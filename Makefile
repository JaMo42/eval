CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
SANFLAGS = -fsanitize=undefined,alignment,address

test: eval.c test.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: sanitize
sanitize: eval.c test.c
	clang $(CFLAGS) $(SANFLAGS) -o test $^
