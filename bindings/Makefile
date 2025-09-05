# uni-json bindings Makefile
#

.PHONY: perl install

perl: perl/Makefile
	$(MAKE) -C perl

perl/Makefile: perl/Makefile.PL
	cd perl && perl Makefile.PL

install: perl
	$(MAKE) -C perl install
