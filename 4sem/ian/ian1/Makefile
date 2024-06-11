TARGET := ian-proj1# name of final binary
MAIN := main.c # name of entrypoint

SOURCES := $(shell find ./*.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

CC := gcc

# stolen from https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-1
# and https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-2
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g -O2 \
        -Wformat-security -Wduplicated-cond -Wfloat-equal -Wshadow -Wconversion \
        -Wjump-misses-init -Wlogical-not-parentheses -Wnull-dereference \
        -fsanitize=undefined -fno-omit-frame-pointer -D_FORTIFY_SOURCE=1

LDFLAGS := -flto -lelf

.PHONY: all clean

all: $(OBJECTS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(TARGET).tar.gz

pack:
	cd .. && tar cf ian/$(TARGET).tar.gz ian/*.c ian/*.h ian/Makefile
