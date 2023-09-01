MAIN_SRC_FILES := worsp.c main.c
EXECUTABLE_MAIN := main

TEST_SRC_FILES := worsp.c test.c
EXECUTABLE_TEST := test

REPL_SRC_FILES := worsp.c repl.c
EXECUTABLE_REPL := repl

CC := gcc
CFLAGS := -Wall -Wextra

CLANG_FORMAT := clang-format
FORMAT_FILES := $(wildcard *.c) $(wildcard *.h)

LLDB := lldb

.PHONY: format clean run-test run-repl run-main

$(EXECUTABLE_MAIN): $(MAIN_SRC_FILES)
	$(CC) $(CFLAGS) $(MAIN_SRC_FILES) -o $(EXECUTABLE_MAIN) -lm

$(EXECUTABLE_TEST): $(TEST_SRC_FILES)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) -o $(EXECUTABLE_TEST) -lm

$(EXECUTABLE_REPL): $(REPL_SRC_FILES)
	$(CC) $(CFLAGS) $(REPL_SRC_FILES) -o $(EXECUTABLE_REPL) -lm

run-main: $(EXECUTABLE_MAIN)
	./$(EXECUTABLE_MAIN) $(WORSP_FILE)

lldb-main: $(EXECUTABLE_MAIN)
	$(LLDB) $(EXECUTABLE_MAIN) $(WORSP_FILE)

run-test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

run-repl: $(EXECUTABLE_REPL)
	./$(EXECUTABLE_REPL)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

clean:
	rm -f $(EXECUTABLE_MAIN) $(EXECUTABLE_TEST) $(EXECUTABLE_REPL)
