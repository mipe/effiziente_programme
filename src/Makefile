# SRC MAKEFILE

CC      = gcc
CFLAGS += -Wall -std=gnu99

.PHONY: all
all: CFLAGS += -O0
all: shortest-path

.PHONY: debug
debug: CFLAGS += -O0 -ggdb -DPERF
debug: shortest-path

shortest-path: shortest-path.o perf.o -lpapi

shortest-path.o: shortest-path.c
perf.o:          perf.c perf.h

.PHONY: clean
clean:
	rm -f shortest-path shortest-path.o perf.o

