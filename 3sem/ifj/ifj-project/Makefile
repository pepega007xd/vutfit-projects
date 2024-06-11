TARGET := compiler # name of compiler binary
MAIN := main.c # name of entrypoint
TEST_PREFIX = test- # prefix of test source files (eg. "test-tokenizer.c")

SOURCES := $(shell find ./*.c | grep -v $(TEST_PREFIX))
HEADERS := $(shell find ./*.h | grep -v $(TEST_PREFIX))
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
# all modules except main function, for compiling test binaries
MODULES := $(patsubst %.c,%.o,$(shell find ./*.c | grep -v $(TEST_PREFIX) | grep -v $(MAIN)))

TEST_SOURCES := $(wildcard test-*.c)
TEST_EXECUTABLES := $(patsubst %.c,%.test,$(TEST_SOURCES)) # .test extension is because of .gitignore

CC := gcc

# stolen from https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-1
# and https://blogs.oracle.com/linux/post/making-code-more-secure-with-gcc-part-2
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g -O2 \
	-Wformat-security -Wduplicated-cond -Wfloat-equal -Wshadow -Wconversion \
	-Wjump-misses-init -Wlogical-not-parentheses -Wnull-dereference \
	-fsanitize=undefined -fno-omit-frame-pointer -D_FORTIFY_SOURCE=1

LDFLAGS := -flto

.PHONY: all run testall zip clean selftest

all: $(OBJECTS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

run: all
	@./$(TARGET)

testall: all zip $(TEST_EXECUTABLES)
	@chmod +x ic23int is_it_ok.sh && \
	python3 compiler_tests.py && \
	rm -rf testdir && \
	./is_it_ok.sh xkralo07.zip testdir && \
	rm -rf testdir && \
	cd havel_tests && ./test.sh && \
	echo "All tests have passed"
	

test-%.test: test-%.o $(MODULES) # we don't know what other header files are included in each test
	@$(CC) $(CFLAGS) $(LDFLAGS) $+ -o $@
	@echo "TESTING: $@"; ./$@

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

zip:
	@rm -f xkralo07.zip
	@zip -q xkralo07.zip Makefile $(SOURCES) $(HEADERS) rozdeleni rozsireni dokumentace.pdf

clean:
	rm -f *.o xkralo07.zip
	rm -f $(TARGET) $(TEST_EXECUTABLES)

selftest: # for debugging Makefile
	@echo "SOURCES: $(SOURCES)"
	@echo "HEADERS: $(HEADERS)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "MODULES: $(MODULES)"
	@echo 
	@echo "TEST_SOURCES: $(TEST_SOURCES)"
	@echo "TEST_EXECUTABLES: $(TEST_EXECUTABLES)"
