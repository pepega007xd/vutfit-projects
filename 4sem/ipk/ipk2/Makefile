TARGET := ipk24chat-server # name of final binary

SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

CC := gcc

# stolen from https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-1
# and https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-2
CFLAGS := -std=gnu11 -Wall -Wextra -pedantic -g -O2 -fsanitize=address \
	-Wformat-security -Wduplicated-cond -Wfloat-equal -Wshadow -Wconversion \
	-Wjump-misses-init -Wlogical-not-parentheses -Wnull-dereference \
	-fsanitize=undefined -fno-omit-frame-pointer -D_FORTIFY_SOURCE=1

LDFLAGS := -flto

.PHONY: all run testall zip clean selftest test

all: $(OBJECTS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

run: all
	@./$(TARGET)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

zip:
	@rm -f xbrabl04.zip
	@zip -q xbrabl04.zip Makefile $(SOURCES) $(HEADERS) README.md LICENSE CHANGELOG.md

clean:
	@rm -f *.o xbrabl04.zip compile_commands.json
	@rm -f $(TARGET)

# recompile project from scratch and generate compile_commands.json
bear: clean
	@bear -- $(MAKE)

selftest: # for debugging Makefile
	@echo "SOURCES: $(SOURCES)"
	@echo "HEADERS: $(HEADERS)"
	@echo "OBJECTS: $(OBJECTS)"
