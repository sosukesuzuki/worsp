C_FILES := $(wildcard *.c)
H_FILES := $(wildcard *.h)
FORMAT_FILES := $(C_FILES) $(H_FILES)
CLANG_FORMAT := clang-format

.PHONY: format
format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)
