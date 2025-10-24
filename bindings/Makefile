# uni-json bindings Makefile
#

PRE_FILE :=	../PREFIX

.PHONY: all perl install clean

all: perl python

install: perl
	$(MAKE) -C perl install

clean: clean-perl clean-python

#*  Perl bindings
#
perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL $(PRE_FILE)
	cd perl && perl Makefile.PL PREFIX=$(shell ../scripts/read-prefix $(PRE_FILE))

clean-perl:
	test -f perl/Makefile && $(MAKE) -C perl clean || true

#*  Python bindings
#
python: python/.built

clean-python:
	cd python && git clean -dfx

python/.built: python/src/parser.c
	( cd python && pip wheel --no-build-isolation . ) && touch $@
