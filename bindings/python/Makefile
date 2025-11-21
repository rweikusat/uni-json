# uni-json Python bindings Makefile
#

#*  macros
#
SO_PATH :=	$(dir $(shell find build -name '*.so'))

SRCS :=		$(shell ls src/*.c)
DEPS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.d)))
DOCS :=		$(addprefix src/, MODULE.h PARSE_JSON.h JSON_SER.h)

T2C :=		scripts/text-2-c.py

#*  targets
#
.PHONY: all clean install test

all: .built

clean:
	rm -rf build
	rm -f *.whl
	rm -rf *.egg-info
	rm -f .built

install: all
	pip install --root install *.whl
	cd install/usr/local && find | cpio -dp $(DESTDIR)$(PREFIX)

.built: $(DEPS)
	pip wheel -v --no-build-isolation . && touch $@

test: all
	LD_LIBRARY_PATH=../../bin PYTHONPATH=$(SO_PATH) python3 -m unittest discover -s t -v

$(DEPS): $(DOCS)

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	gcc -I ../../include -MM $< | perl -pe 's|$*\.o|$@|' >$@

src/%.h: doc/%
	$(T2C) $* <$< >$@
