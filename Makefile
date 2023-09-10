MAIN_SRC_FILES := worsp.c main.c
EXECUTABLE_MAIN := main
DEBUGGABLE_MAIN := debug_main

TEST_SRC_FILES := worsp.c test.c
EXECUTABLE_TEST := test

REPL_SRC_FILES := worsp.c repl.c
EXECUTABLE_REPL := repl

EXECUTABLE_SNAPSHOT_TEST := ./tests/test.sh

CC := gcc
CFLAGS := -Wall -Wextra

CLANG_FORMAT := clang-format
FORMAT_FILES := $(wildcard *.c) $(wildcard *.h)

LLDB := lldb

BASH := bash

.PHONY: format clean run-test run-repl run-main lldb-main

$(EXECUTABLE_MAIN): $(MAIN_SRC_FILES)
	$(CC) $(CFLAGS) $(MAIN_SRC_FILES) -o $(EXECUTABLE_MAIN) -lm

$(DEBUGGABLE_MAIN): $(MAIN_SRC_FILES)
	$(CC) $(CFLAGS) -g $(MAIN_SRC_FILES) -o $(DEBUGGABLE_MAIN) -lm

$(EXECUTABLE_TEST): $(TEST_SRC_FILES)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) -o $(EXECUTABLE_TEST) -lm

$(EXECUTABLE_REPL): $(REPL_SRC_FILES)
	$(CC) $(CFLAGS) $(REPL_SRC_FILES) -o $(EXECUTABLE_REPL) -lm

run-main: $(EXECUTABLE_MAIN)
	./$(EXECUTABLE_MAIN) $(WORSP_FILE)

lldb-main: $(DEBUGGABLE_MAIN)
	$(LLDB) $(DEBUGGABLE_MAIN) $(WORSP_FILE)

run-test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

check-snapshot: $(EXECUTABLE_MAIN)
	$(BASH) $(EXECUTABLE_SNAPSHOT_TEST)

update-snapshot: $(EXECUTABLE_MAIN)
	$(BASH) $(EXECUTABLE_SNAPSHOT_TEST) -u

run-repl: $(EXECUTABLE_REPL)
	./$(EXECUTABLE_REPL)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

clean:
	rm -f $(EXECUTABLE_MAIN) $(EXECUTABLE_TEST) $(EXECUTABLE_REPL)
