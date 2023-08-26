TEST_SRC_FILES := worsp.c test.c
EXECUTABLE_TEST := test

REPL_SRC_FILES := worsp.c repl.c
EXECUTABLE_REPL := repl

CC := gcc
CFLAGS := -Wall -Wextra -lm

FORMAT_FILES := $(wildcard *.c) $(wildcard *.h)
CLANG_FORMAT := clang-format

.PHONY: format test clean

$(EXECUTABLE_TEST): $(TEST_SRC_FILES)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) -o $(EXECUTABLE_TEST)

$(EXECUTABLE_REPL): $(REPL_SRC_FILES)
	$(CC) $(CFLAGS) $(REPL_SRC_FILES) -o $(EXECUTABLE_REPL)

run-test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

run-repl: $(EXECUTABLE_REPL)
	./$(EXECUTABLE_REPL)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)
