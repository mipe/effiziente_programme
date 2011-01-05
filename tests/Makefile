
all: test

test:
	mkdir -p logs
	./runtests 2>/dev/null
	../shortest-path <input-ref-littleendian   | diff -u - xoutput-ref
	../shortest-path <input-bench-littleendian | diff -u - xoutput-bench

