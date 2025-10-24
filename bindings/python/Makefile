# uni-json Python bindings Makefile
#

.PHONY: all clean

all: .built

clean:
	git clean -dfx

.built: src/parser.c
	pip wheel --no-build-isolation . && touch $@
