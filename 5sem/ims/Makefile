TARGET := ims
ARCHIVE := 02_xbrabl04_xstarh04.tar.gz

SOURCES := $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

CC := g++

# stolen from https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-1
# and https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-2
CFLAGS := -std=c++20 -Wall -Wextra -pedantic -g -O2 \
	-Wformat-security -Wduplicated-cond -Wfloat-equal -Wshadow -Wconversion \
	-Wlogical-not-parentheses -Wnull-dereference \
	-fsanitize=undefined -fno-omit-frame-pointer -D_FORTIFY_SOURCE=1

LDFLAGS := -flto

SIMLIB_LINK_ARGS := -lsimlib

.PHONY: all run pack clean bear selftest

all: $(OBJECTS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(SIMLIB_LINK_ARGS) $(OBJECTS) -o $(TARGET)

run: all
	@ASAN_OPTIONS=detect_leaks=0 ./$(TARGET)

%.o: %.cpp $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

pack: clean
	@tar cf $(ARCHIVE) Makefile $(SOURCES) $(HEADERS) documentation.pdf

clean:
	@rm -rf *.o $(ARCHIVE) $(TARGET) compile_commands.json .cache

# recompile project from scratch and generate compile_commands.json
bear: clean
	@bear -- $(MAKE)

selftest: # for debugging Makefile
	@echo "SOURCES: $(SOURCES)"
	@echo "HEADERS: $(HEADERS)"
	@echo "OBJECTS: $(OBJECTS)"

