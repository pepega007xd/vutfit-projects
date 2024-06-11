CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g -fsanitize=address
FILES=hashtable.c test.c test_util.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f test
