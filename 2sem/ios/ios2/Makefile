CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
# CFLAGS += -g
# CFLAGS += -fsanitize=address
LDFLAGS = -pthread
# LDFLAGS += -g
# LDFLAGS += -fsanitize=address
.PHONY: all run clean zip
bin = proj2
srcs = $(wildcard *.c)
objs = $(patsubst %.c,%.o,$(srcs))
# headers = $(patsubst %.c,%.h,$(srcs))

$(bin): $(objs)

run: $(bin)
	./$(bin) 3 3 100 100 100
clean:
	rm -f *.o $(bin) $(bin).out proj2.zip

zip:
	zip proj2.zip *.c *.h Makefile
