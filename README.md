# uesh

UNIX utilities and shell interpreter in C — sort, ls, tail, head, grep, replace, cp, kill, and a custom shell (UEsh), built for the Operating Systems course.

[![License: MIT](https://img.shields.io/badge/License-MIT-3da639.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-completed-6f42c1)

[![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![Make](https://img.shields.io/badge/Make-4495D5?logo=make&logoColor=white)](https://www.gnu.org/software/make/)

[Portuguese](README.pt.md) | English

## About

Implementation of 9 classic UNIX utilities and a custom shell interpreter for the Operating Systems course. All utilities share a common library (`lib/common.c`) for argument parsing and error handling. Developed natively for Linux.

## Utilities

### Shell

| Utility | Description |
| ------- | ----------- |
| **UEsh** | Shell interpreter with a custom prompt. Supports external utilities and built-in commands: `cd`, `pwd`, `echo`, `sleep`, `exit`. |

### Required

| Utility | Description |
| ------- | ----------- |
| **sort** | Sorts text files. Supports descending order (`-d`). Writes output to `FILE.sort`. |
| **ls** | Lists directory contents. Supports long format (`-l`), column layout (`-c`), and sort by name, size, or date (`-on`, `-os`, `-od`). |
| **grep** | Searches for strings in files. Supports case-insensitive search (`-i`), line numbers (`-n`), match count (`-c`), and inverted match (`-v`). |

### Optional

| Utility | Description |
| ------- | ----------- |
| **tail** | Prints the last N lines of a text file. Supports `-n`, `-E`, and a numeric count flag. |
| **head** | Prints the first N lines of a text file. Supports `-n`, `-E`, and a numeric count flag. |
| **replace** | Replaces a string in files, processing each file in a separate child process (`fork`). Supports `-c`, `-n`, `-i`. |
| **cp** | Copies a file to a destination. Supports interactive mode (`-i`). |
| **kill** | Terminates a process by PID via `SIGTERM`. |

## Requirements

| Tool | Minimum version |
| ---- | --------------- |
| GCC  | 9+              |
| Make | 4+              |

Linux only — native macOS execution is not supported.

## How to run

```bash
make          # compile all utilities
make run      # compile and launch UEsh
make valgrind # run UEsh under valgrind
make clean    # remove build artifacts
```

Binaries are placed in `bin/`. All utilities support `-h` for usage help.

## License

Distributed under the **MIT** license, © 2026 Nycolas Souza.

It is a permissive license: anyone can use, copy, modify, and distribute the code, including in commercial projects, as long as the copyright notice and license text are retained.

The full text is in [LICENSE](LICENSE).
