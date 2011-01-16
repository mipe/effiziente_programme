# TESTS MAKEFILE

.PHONY: all
all: test

.PHONY: test
test:
	mkdir -p logs
	./runtests.sh
	python compare.py logs/log_*

.PHONY: clobber
clobber:
	-rm logs/*

