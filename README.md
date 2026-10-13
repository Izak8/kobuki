# Kobuki Robot
The source code for the Kobuki firmware lives here.

## Getting Started

To clone the repo, ensure that you include the `--recursive` option in order to
fetch the included submodules (located at `extern`). To build the code manually
you can type:

`cmake -S . -B build/Debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake -DCMAKE_BUILD_TYPE=Debug`

This configures the CMake build system. You can then build the code by typing:

`cmake --build build/Debug`

This produces an output file `build/Debug/app/kobuki_main/kobuki_main.elf`
which can be flashed to the NUCELO board.

## Contribution Guidelines
Development takes place in branches other than `main`. When working on a feature create a new branch
(e.g. `git checkout -b dev/uart-wrapper`). Make sure to branch from the latest `main` ref. Features
that are complete should be merged to `main` via a pull request. The pull request title and
description should follow the [conventional commits](https://www.conventionalcommits.org/en/v1.0.0/)
standard. The `main` branch should have a linear commit history. To achieve this, pull requests are
merged by squashing.

### Commit Types
Each commit is prefixed with one of the following types:

- **feat** (a change which adds new functionality)
- **fix** (a change which fixes a bug)
- **build** (a change which modifies the build system)
- **docs** (a change which modifies the documentation)
- **repo** (a change which modifies repository configuration)
- **style** (a change which modifies code style or formatting)

A commit which is considered a breaking change (not backwards compatible) must have a `!` suffixed
to the end of the commit type (e.g. `feat!: uart wrapper functions now use scoped-enum args`).
Commits which introduce breaking changes must have a footer in the extended description which
details the nature of the breaking change. NOTE: A commit message may have an extended description
by adding a blank line followed by any text.

e.g. 
```
feat!: uart wrapper functions now use scoped-enum args

This change was made to make the code more type-safe and avoid issues which can't be discovered
at compile-time.

BREAKING-CHANGE: Code dependent on the UART wrapper will need to change the arguments to the new
types.
```
