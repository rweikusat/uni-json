# uni-json Python bindings Makefile
#

SO_PATH :=	$(dir $(shell find build -name '*.so'))

.PHONY: all clean test

all: .built

clean:
	git clean -dfx

.built: src/parser.c
	pip wheel --no-build-isolation . && touch $@

test: all
	PYTHONPATH=$(SO_PATH) python -m unittest discover -s t
