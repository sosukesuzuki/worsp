# worsp

Worsp is a minimal LISP interpreter written in C.

## Development

### `make format`

Run clang-format for all `*.c` and `*.h` files.

### `make main` and `make run-main`

Build and run main program. It execute worsp program from a file.

```
make run-main WORSP_FILE="./tmp/fact.wsp"
```

### `make lldb-main`

Build and run main program via lldb.

```
make lldb-main WORSP_FILE="./tmp/fact.wsp"
```

### `make test` and `make run-test`

Build `test.c` and run it.

### `make repl` and `make run-repl`

Build `repl.c` and run it.
