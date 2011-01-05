
CC      = gcc
CFLAGS += -DPERF -Wall -O -std=gnu99

.PHONY: all
all: shortest-path

.PHONY: debug
debug:
	mv src/shortest-path .

shortest-path: shortest-path.o perf.o -lpapi

shortest-path.o: shortest-path.c
perf.o:          perf.c perf.h

.PHONY: clean
clean:
	rm -f shortest-path shortest-path.o perf.o
