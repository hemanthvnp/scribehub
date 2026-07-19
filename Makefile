# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Isrc -g
LDFLAGS = -lncurses

# Targets
TARGET = scribehub
TEST_TARGET = run_tests

# Source Files
SRC = src/buffer.c src/ui.c src/main.c
OBJ = $(SRC:.c=.o)

TEST_SRC = tests/test_buffer.c src/buffer.c
TEST_OBJ = $(TEST_SRC:.c=.o)

.PHONY: all clean test format

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_TARGET)
	./$(TEST_TARGET)

format:
	clang-format -i src/*.c src/*.h tests/*.c 2>/dev/null || true

clean:
	rm -f src/*.o tests/*.o $(TARGET) $(TEST_TARGET)
