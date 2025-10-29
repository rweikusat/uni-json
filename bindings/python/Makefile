# uni-json Python bindings Makefile
#


#*  macros
#
SO_PATH :=	$(dir $(shell find build -name '*.so'))

SRCS :=		$(shell ls src/*.c)
DEPS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.d)))

#*  targets
#
.PHONY: all clean test

all: .built

clean:
	git clean -dfx

.built: tmp/parser.d
	pip wheel -v --no-build-isolation . && touch $@

test: all
	LD_LIBRARY_PATH=../../bin PYTHONPATH=$(SO_PATH) python -m unittest discover -s t -v

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	$(CC) -I ../../include -MM $< | perl -pe 's|$*\.o|$@|' >$@
