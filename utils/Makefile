SRC := read_file.c test.c rw_all.c
INC := read_file.h common.h log.h

CC := clang
CFLAGS += -Wall -Werror

all: test

test: $(SRC) $(INC)
	$(CC) $(CFLAGS) $(SRC) -o $@
