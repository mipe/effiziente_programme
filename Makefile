
CFLAGS=

export CFLAGS

.PHONY: all
all: shortest-path

test: shortest-path
	tests/runtests
#	shortest-path <input-ref-littleendian|diff -u - output-ref
#	shortest-path <input-bench-littleendian|diff -u - output-bench

.PHONY: debug
debug: CFLAGS += -ggdb
debug: shortest-path 

.PHONY: shortest-path
shortest-path:
	make -C src
	mv src/shortest-path .

clean:
	make clean -C src
	rm -f shortest-path

