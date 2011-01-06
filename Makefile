

USER=
CFLAGS=

export CFLAGS
export USER

.PHONY: all
all: shortest-path

.PHONY: debug
debug: CFLAGS += -ggdb
debug: shortest-path 

.PHONY: tests
test: shortest-path
	$(MAKE) test -C tests

shortest-path:
	$(MAKE) all -C src
	mv src/shortest-path .

.PHONY: clean
clean:
	$(MAKE) clean -C src
	rm -f shortest-path

.PHONY: clobber
clobber: clean
	$(MAKE) clobber -C tests

