# uni-json bindings Makefile
#

PRE_FILE :=	../PREFIX

.PHONY: perl install

perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL $(PRE_FILE)
	cd perl && perl Makefile.PL INSTALL_BASE=$(shell ../scripts/read-prefix $(PRE_FILE))

install: perl
	$(MAKE) -C perl install
