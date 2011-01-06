
all: test

test:
	mkdir -p logs
	./runtests 2>/dev/null
	python compare.py logs/*

.PHONY: clobber
clobber:
	rm logs/*

