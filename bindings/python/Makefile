# uni-json Python bindings Makefile
#


#*  macros
#
SO_PATH :=	$(dir $(shell find build -name '*.so'))

SRCS :=		$(shell ls src/*.c)
DEPS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.d)))
DOCS :=		$(addprefix src/, MODULE.h PARSE_JSON.h)

T2C :=		scripts/text-2-c.py

#*  targets
#
.PHONY: all clean test

all: .built

clean:
	git clean -dfx

.built: $(DEPS)
	pip wheel -v --no-build-isolation . && touch $@

test: all
	LD_LIBRARY_PATH=../../bin PYTHONPATH=$(SO_PATH) python -m unittest discover -s t -v

$(DEPS): $(DOCS)

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	gcc -I ../../include -MM $< | perl -pe 's|$*\.o|$@|' >$@

src/%.h: doc/%
	$(T2C) $* <$< >$@
