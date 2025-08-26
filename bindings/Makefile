# uni-json bindings Makefile
#

.PHONY: all perl

all: perl

perl: perl/Makefile
	make -C perl

perl/Makefile: perl/Makefile.PL
	cd perl && perl Makefile.PL
