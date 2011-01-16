# MAIN MAKEFILE

# name of program
PROG = shortest-path

CFLAGS  = -DPERF
LDFLAGS = -lpapi

export CFLAGS
export LDFLAGS

# build program
.PHONY: all
all:
	$(MAKE) all   -C src
	$(MAKE) __move__

.PHONY: debug
debug:
	$(MAKE) debug -C src
	$(MAKE) __move__

.PHONY: gprof
gprof:
	$(MAKE) gprof -C src
	$(MAKE) __move__

# move binaries
.PHONY: __move__
__move__:
	mv src/$(PROG)    .
	mv src/$(PROG)-O0 .
	mv src/$(PROG)-O3 .

# testing
.PHONY: test
test: clean all
	$(MAKE) test -C tests

.PHONY: compare
compare:
	python tests/compare.py tests/logs/log_*

# cleanup
.PHONY: clean
clean:
	$(MAKE) clean -C src
	-rm $(PROG) $(PROG)-O0 $(PROG)-O3

.PHONY: clobber
clobber: clean
	$(MAKE) clobber -C tests


