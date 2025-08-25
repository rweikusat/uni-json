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
LIB :=		libuni-json.so

#**  CFLAGS
#
CFLAGS :=	-g -W -Wall

ifdef FINAL
CFLAGS :=	-O2 $(CFLAGS)
endif

#**  lib version
#
V_MAJ :=	0

#*  targets
#
.PHONY: all clean

all: bin/$(LIB)

clean:
	-rm tmp/*o
	-rm bin/*

bin/$(LIB): $(OBJS)

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	$(CC) -MM $< | perl -pe 's|$*\.o|$@|' >$@

tmp/%.o: src/%.c tmp/%.d
	$(CC) $(CFLAGS) -c -o $@ -fpic $<

bin/%:
	$(LD) -shared -o $@ -Wl,-soname -Wl,$(notdir $@).$(V_MAJ) $^
