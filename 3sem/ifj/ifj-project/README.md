#IFJ

## Project structure

All source files are in the root directory, all `.c` files not beginning with `test-`
are compiled as modules and linked into binary `compiler`. These are where you implement
the different modules. Each `.c` file beginning with `test-` is built into a separate
test binary `test-<name>.test`. This is where you put your tests (one `main` function per
`test-<name>.c` file).

### Makefile commands

- `make all` builds the compiler
- `make run` builds and runs the compiler
- `make testall` builds and runs all tests
- `make test-<name>` builds and runs test `test-<name>`
- `make clean` cleans all build files

### Formatting

There is a `.clang-format` file in the project, please use it to format all the code
before commiting it to avoid large commit diffs because of different autoformatters.

You can automate this by putting the following code to `.git/hooks/pre-commit`:

```bash
for FILE in $(git diff --cached --name-only | grep -v '\.md' | grep -v '\.ini')
do
        clang-format -i "$FILE"
done
```
