# Filename: Makefile
# Author: Tomas Brablec (xbrabl04)
# Faculty: FIT BUT
# Date: 2023-03-28
#
#Compiler: gcc-10.2.1-6

CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -pedantic -fPIC

# debug flags
# CFLAGS+=-g -fsanitize=address
# LDFLAGS=-fsanitize=address

# test flags
# CFLAGS+=-DSTATISTICS

# tryout flags
# CFLAGS+=-DHASHTEST

HTAB_DEPS=$(patsubst %.c,%.o,$(wildcard htab_*.c))

.PHONY=all clean test

all: tail wordcount wordcount-dynamic libhtab.a libhtab.so 

tail: tail.c
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

wordcount-cc: wordcount-.cc
	g++ -O2 -o $@ $^

wordcount: wordcount.c libhtab.a io.o
	$(CC) $(CFLAGS) -o $@ -L. $(LDFLAGS) $^

wordcount-dynamic: wordcount.c libhtab.so io.o
	$(CC) $(CFLAGS) -o $@ -L. $(LDFLAGS) $^

libhtab.a: $(HTAB_DEPS)
	ar rcs $@ $^

libhtab.so: $(HTAB_DEPS)
	$(CC) $(CFLAGS) -shared -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a *.so tail wordcount wordcount-dynamic wordcount-cc test

zip:
	zip xbrabl04.zip *.c *.cc *.h test.sh Makefile

test: test.c all wordcount-cc
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $< libhtab.a
	bash test.sh
