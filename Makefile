# MAIN MAKEFILE

USER=
CFLAGS=
LDFLAGS=

export CFLAGS
export LDFLAGS
export USER

.PHONY: all
all:
	$(MAKE) all   -C src
	mv src/shortest-path .

.PHONY: debug
debug:
	$(MAKE) debug -C src
	mv src/shortest-path .

.PHONY: gprof
gprof: CFLAGS  += -pg -O0
gprof: LDFLAGS += -pg -O0
gprof: debug

.PHONY: tests
test:
	$(MAKE) test -C tests

.PHONY: clean
clean:
	$(MAKE) clean -C src
	rm -f shortest-path

.PHONY: clobber
clobber: clean
	$(MAKE) clobber -C tests

