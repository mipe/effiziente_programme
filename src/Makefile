# SRC MAKEFILE

PROG = shortest-path

CC      = gcc
CFLAGS += -Wall -std=gnu99

.PHONY: all
all:   $(PROG) $(PROG)-O0 $(PROG)-O3

.PHONY: debug
debug: CFLAGS += -ggdb
debug: all

.PHONY: gprof
gprof: CFLAGS  += -pg
gprof: LDFLAGS += -pg
gprof: all

# build without optimization
$(PROG)-O0: CFLAGS += -O0
$(PROG)-O0: $(PROG)-O0.o perf.o

# build with full optimization
$(PROG)-O3: CFLAGS += -O3
$(PROG)-O3: $(PROG)-O3.o perf.o

# build default
$(PROG): $(PROG).o perf.o 

$(PROG)-O0.c:
	cp $(PROG).c $(PROG)-O0.c
$(PROG)-O3.c:
	cp $(PROG).c $(PROG)-O3.c

perf.o:    perf.c perf.h

# cleanup
.PHONY: clean
clean:
	-rm $(PROG)    $(PROG).o
	-rm $(PROG)-O0 $(PROG)-O0.o $(PROG)-O0.c
	-rm $(PROG)-O3 $(PROG)-O3.o $(PROG)-O3.c
	-rm perf.o


