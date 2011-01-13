# SRC MAKEFILE

CC      = gcc
CFLAGS += -Wall -std=gnu99 -O3 -DPERF

.PHONY: all
all: shortest-path

.PHONY: debug
debug: CFLAGS += -ggdb -DPERF
debug: shortest-path

shortest-path: shortest-path.o perf.o -lpapi

shortest-path.o: shortest-path.c
perf.o:          perf.c perf.h

.PHONY: clean
clean:
	rm -f shortest-path shortest-path.o perf.o

