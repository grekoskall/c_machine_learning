# This makefile builds and manages the project.

PROGRAM_NAME = main
DATASET_X = ./Datasets/2013_camera_specs/
DATASET_W = ./Datasets/sigmod_medium_labelled_dataset.csv

# Compilation flags
CFLAGS  = -std=gnu99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -pthread
#CFLAGS += -Wextra
#CFLAGS += -pedantic
#CFLAGS += -Werror

# Test flags
TEST_FLAGS  = -std=gnu99
TEST_FLAGS += -Wall
TEST_FLAGS += -I./test

# Valgrind flags
VFLAGS  = --quiet
VFLAGS += --tool=memcheck
VFLAGS += --leak-check=full
VFLAGS += --error-exitcode=1

SOURCE := $(shell find ./src -name '*.c' -not -name 'main.c')
MAIN := 'src/main.c'
UNITY := $(shell find ./test/Unity/src -name '*.c')
TESTS := $(shell find ./test -name '*.c' -not -path '*Unity*')
SHELL := /bin/bash

.PHONY: usage
usage:
	@echo "b|uild      Build the executable"
	@echo "c|lean      Remove any temporary products"
	@echo "cl|obber    Remove any generated files"
	@echo "m|emcheck   Check for memory leaks"
	@echo "r|un        Run 'build', then run the resulting executable with default arguments"
	@echo "t|est       Run the unit tests"

.PHONY: memcheck
memcheck: build/test
ifeq ( , $(shell which valgrind))
	$(error Please install valgrind to run this command)
else
	@valgrind $(VFLAGS) ./build/test
	@echo "Memory check passed"
endif
.PHONY: m
m: memcheck

.PHONY: build
build: $(SOURCE)
	mkdir -p build
	$(CC) $(CFLAGS) $(SOURCE) $(MAIN) -o ./build/$(PROGRAM_NAME) -lm
	@echo " "
	@echo "---- Run: ./build/main -x [dataset X folder] -w [dataset W file.csv] ----"
.PHONY: b
b: build

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM tmp* ./output/*.txt

.PHONY: ccd
c: clean

.PHONY: clobber
clobber: clean
	rm -rf build/
.PHONY: cl
cl: clobber

.PHONY: run
run: clean build
	time ./build/$(PROGRAM_NAME) -w $(DATASET_W) -x $(DATASET_X)
.PHONY: r
r: run

.PHONY: test
test: build/test
	./build/test
PHONY: t
t: test

build/test: $(TESTS) build
	$(CC) $(CFLAGS) $(TESTS) $(SOURCE) $(UNITY) -o build/test -lm
