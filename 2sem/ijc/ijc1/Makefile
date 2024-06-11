# Makefile
# Řešení IJC-DU1, příklad a), 2022-03-04 
# Autor: Tomáš Brablec
# Přeloženo: gcc 10.2.1
# přidal jsem modul s testem a steg-encode

CC = gcc
CFLAGS = -g -std=c11 -pedantic -Wall -Wextra
# CFLAGS += -fsanitize=address
# CFLAGS += -m32
LDFLAGS = -lm
INLINE = -DUSE_INLINE

.PHONY: all run clean zip
.SILENT:

all: steg-decode primes primes-i

test: bitset.h test.c error.o steg-decode steg-encode test.sh
	$(CC) $(CFLAGS) -o test test.c error.o 
	$(CC) $(CFLAGS) $(INLINE) -o test-i test.c error.o 
	./test.sh

steg-encode: ppm.o steg-encode.c utf8_check.c eratosthenes.c
	$(CC) $(CFLAGS) -o steg-encode steg-encode.c ppm.o error.o $(LDFLAGS) 

steg-decode: ppm.o steg-decode.c utf8_check.c eratosthenes.c
	$(CC) $(CFLAGS) -o steg-decode steg-decode.c ppm.o error.o $(LDFLAGS) 

run: primes primes-i
	ulimit -s 50000; ./primes; ./primes-i

primes: error.o primes.c bitset.h eratosthenes.c
	$(CC) $(CFLAGS) -o primes primes.c error.o $(LDFLAGS) 

primes-i: error.o primes.c bitset.h eratosthenes.c
	$(CC) $(CFLAGS) $(INLINE) -o primes-i primes.c error.o $(LDFLAGS) 

ppm.o: error.o ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

clean:
	rm -f *.o primes primes-i test test-i steg-decode steg-encode xbrabl04.zip

zip:
	zip xbrabl04.zip *.c *.h Makefile test.sh
