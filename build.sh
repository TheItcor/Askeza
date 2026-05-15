#!/bin/bash

# I think complex build systems are unnecessary. Because there are few files.
# The bash-script solves all problems:


# Compiler: Clang

# Find all files.c in src/
C_FILES=$(find src -name "*.c")

# Find all headers.h in includes/
H_FILES=$(find include -name "*.h")

# Compile
clang $C_FILES -Iinclude -o ask -lm
