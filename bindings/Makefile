# uni-json bindings Makefile
#

#*  macros
#
PRE_FILE :=	../PREFIX
PREFIX :=	$(shell ../scripts/read-prefix $(PRE_FILE))

BINDINGS :=	perl python

BUILDS :=	$(addprefix build., $(BINDINGS))
CLEANS :=	$(addprefix clean., $(BINDINGS))
INSTALLS :=	$(addprefix install., $(BINDINGS))
TESTS :=	$(addprefix test., $(BINDINGS))

#*  targets
#
.PHONY: all clean install test

all: $(BUILDS)
clean: $(CLEANS)

install: all $(INSTALLS)

test: all $(TESTS)

#*  Perl bindings
#
build.perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL $(PRE_FILE)
	cd perl && perl Makefile.PL PREFIX=$(PREFIX)

clean.perl:
	test -f perl/Makefile && $(MAKE) -C perl clean || true

%.perl:
	$(MAKE) -C perl $*

#*  Python bindings
#
build.python:
	$(MAKE) -C python

%.python:
	$(MAKE) -C python $* PREFIX=$(PREFIX)
