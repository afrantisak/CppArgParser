CppArgParser
============

Python-style C++ Command Line Argument Parser

### Simplifying Assumptions
1. optional arguments always begin with `--`; ex: `foo --help` or `foo --count 1` or `foo --count=1`
2. required arguments are always positional; ex: `foo 12 13`
3. `--help` is built-in, you do not need to add it, and it always documents every parameter.