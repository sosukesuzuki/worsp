TEST_SRC_FILES := worsp.c test.c
EXECUTABLE_TEST := test

CC := gcc
CFLAGS := -Wall -Wextra

FORMAT_FILES := $(wildcard *.c) $(wildcard *.h)
CLANG_FORMAT := clang-format

.PHONY: format test clean

$(EXECUTABLE_TEST): $(TEST_SRC_FILES)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) -o $(EXECUTABLE_TEST)

run-test: $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)
