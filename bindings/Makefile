# uni-json bindings Makefile
#

#*  macros
#
PRE_FILE :=	../PREFIX
PREFIX :=	$(shell ../scripts/read-prefix $(PRE_FILE))

#*  targets
#
.PHONY: all perl python install clean

all: perl python

install: all
	$(MAKE) -C perl install

clean: clean-perl clean-python

#*  Perl bindings
#
perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL $(PRE_FILE)
	cd perl && perl Makefile.PL PREFIX=$(PREFIX)

clean-perl:
	test -f perl/Makefile && $(MAKE) -C perl clean || true

#*  Python bindings
#
python:
	$(MAKE) -C python

%-python:
	$(MAKE) -C python $*
