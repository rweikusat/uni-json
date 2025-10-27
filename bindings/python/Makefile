# uni-json Python bindings Makefile
#

.PHONY: all clean test

all: .built

clean:
	git clean -dfx

.built: src/parser.c
	pip wheel --no-build-isolation . && touch $@

test: all
	PYTHONPATH=$(dir $(shell find build -name '*.so')) python -m unittest discover -s t
