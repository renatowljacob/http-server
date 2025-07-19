BINARY := http

# Gets the Operating system name
OS := $(shell uname -s)

# Default shell
SHELL := bash

# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test

# Source code file extension
SRCEXT := c

# Defines the C Compiler
CC := gcc

# Defines the language standards for GCC
STD := -std=c11 # See man gcc for more options

# Specifies to GCC the required warnings
WARNS := -Wall -Wextra -Wformat-overflow -Wuse-after-free=1 -Wstrict-prototypes -Wshadow -Wconversion

# Debug flags
DEBUG := -O0 $(WARNS) -fsanitize=undefined,address,leak -fmax-errors=1 -gdwarf -ggdb

# Release flags
RELEASE := -O3

# Test libraries
TEST_LIBS := -l cmocka -L /usr/lib

# Tests binary file
TEST_BINARY := $(BINARY)_test_runner

# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))

#
# COMPILATION RULES
#

default: release

# Help message
help:
	@echo "Sample HTTP Server"
	@echo
	@echo "Target rules:"
	@echo "    debug    - Compiles and generates binary file with debug flags"
	@echo "    release  - Compiles and generates binary file with optimizations"
	@echo "    tests    - Compiles with cmocka and run tests binary file"
	@echo "    valgrind - Runs binary file using valgrind tool"
	@echo "    clean    - Clean the project by removing binaries"
	@echo "    help     - Prints a help message with target rules"

# Rule for link and generate the binary file
debug: $(OBJECTS)
	$(CC) -o $(BINDIR)/$(BINARY) $+ $(DEBUG) $(STD)

# Rule for link and generate the binary file
release: $(OBJECTS)
	$(CC) -o $(BINDIR)/$(BINARY) $+ $(RELEASE) $(STD)

# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) -c $^ -o $@ $(DEBUG) $(STD)

# Rule for run valgrind tool
valgrind: release
	valgrind \
		--track-origins=yes \
		--leak-check=full \
		--leak-resolution=high \
		--log-file=$(LOGDIR)/$@.log \
		$(BINDIR)/$(BINARY)
	@echo -en "\nCheck the log file: $(LOGDIR)/$@.log\n"

# Compile tests and run the test binary
tests:
	$(CC) $(TESTDIR)/main.c -o $(BINDIR)/$(TEST_BINARY) $(DEBUG) $(STD) $(TEST_LIBS)
	@which ldconfig && ldconfig -C /tmp/ld.so.cache || true # caching the library linking
	@echo -en "Running tests: ";
	./$(BINDIR)/$(TEST_BINARY)

# Rule for cleaning the project
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;
