# TESTS MAKEFILE

all: test

test:
	mkdir -p logs
	./runtests `whoami` 2>/dev/null
	python compare.py logs/perf_log*

.PHONY: clobber
clobber:
	-rm logs/*

