CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm -fsanitize=address -g
FILES=btree.c ../btree.c stack.c ../test_util.c ../test.c ../exa/exa.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -DEXA=1 -o $@ $(FILES)

clean:
	rm -f test
