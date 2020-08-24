CC = gcc
CFLAGS = -Wall

test: eval.c test.c
	$(CC) $(CFLAGS) -o $@ $^

