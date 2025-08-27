# uni-json Makefile
#

#*  definitions
#**  programs
#
GCC :=		gcc
CC :=		$(GCC) -Iinclude
LD :=		$(GCC)

#**  files
#
SRCS :=		$(shell ls src/*.c)
OBJS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.o)))
DEPS :=		$(OBJS:.o=.d)

#**  library
#
V_MAJ :=	0
V_MIN :=	1

L_BASE :=	libuni-json.so
L_MAJ :=	$(L_BASE).$(V_MAJ)
LIB :=		$(L_MAJ).$(V_MIN)

#**  CFLAGS
#
CFLAGS :=	-g -W -Wall

ifdef FINAL
CFLAGS :=	-O2 $(CFLAGS)
endif

#**  lib version
#

#*  targets
#
.PHONY: all clean

all: bin/$(L_MAJ) bin/$(L_BASE)
	$(MAKE) -C bindings

clean:
	-rm tmp/*o
	-rm bin/*

bin/$(LIB): $(OBJS)

bin/$(L_MAJ) bin/$(L_BASE): bin/$(LIB)
	ln -sf $(notdir $^) $@

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	$(CC) -MM $< | perl -pe 's|$*\.o|$@|' >$@

tmp/%.o: src/%.c tmp/%.d
	$(CC) $(CFLAGS) -c -o $@ -fpic $<

bin/%:
	$(LD) -shared -o $@ -Wl,-soname -Wl,$(notdir $(basename $@)) $^
