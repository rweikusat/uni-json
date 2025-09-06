# uni-json bindings Makefile
#

PRE_FILE :=	../PREFIX

.PHONY: all perl install clean

all: perl

install: perl
	$(MAKE) -C perl install

clean: clean-perl

perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL $(PRE_FILE)
	cd perl && perl Makefile.PL INSTALL_BASE=$(shell ../scripts/read-prefix $(PRE_FILE))


clean-perl:
	test -f perl/Makefile && $(MAKE) -C perl clean || true
