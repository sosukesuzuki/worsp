TEST_SRC_FILES := worsp.c test.c
EXECUTABLE_TEST := test

REPL_SRC_FILES := worsp.c repl.c
EXECUTABLE_REPL := repl

CC := gcc
CFLAGS := -Wall -Wextra

FORMAT_FILES := $(wildcard *.c) $(wildcard *.h)
CLANG_FORMAT := clang-format

.PHONY: format clean run-test

$(EXECUTABLE_TEST): $(TEST_SRC_FILES)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) -o $(EXECUTABLE_TEST) -lm

$(EXECUTABLE_REPL): $(REPL_SRC_FILES)
	$(CC) $(CFLAGS) $(REPL_SRC_FILES) -o $(EXECUTABLE_REPL) -lm

run-test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

run-repl: $(EXECUTABLE_REPL)
	./$(EXECUTABLE_REPL)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

clean:
	rm -f $(EXECUTABLE_TEST) $(EXECUTABLE_REPL)
