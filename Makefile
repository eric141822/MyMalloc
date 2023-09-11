CC=gcc
CCFLAGS=-Wall -Wundef -Wshadow -Wpointer-arith -Wcast-align \
	-pedantic -O3

EXECUTABLES=build \
build/main.out \

all: $(EXECUTABLES)

build/main.out: src/mymalloc.c src/main.c
	$(CC) $(CCFLAGS) $^ -o $@

build:
	mkdir -p build

.PHONY: clean all

clean:
	$(RM) -r build/*.out
	rmdir build