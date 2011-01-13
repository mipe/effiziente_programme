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
gprof: CFLAGS  += -pg
gprof: LDFLAGS += -pg
gprof: debug

.PHONY: tests
test: clean all
	$(MAKE) test -C tests

.PHONY: clean
clean:
	$(MAKE) clean -C src
	rm -f shortest-path

.PHONY: clobber
clobber: clean
	$(MAKE) clobber -C tests

.PHONY: compare
compare:
	python tests/compare.py tests/logs/perf*


