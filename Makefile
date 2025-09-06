# uni-json Makefile
#

#*  definitions
#**  programs
#
GCC :=		gcc
CC :=		$(GCC) -Iinclude
LD :=		$(GCC)
INST_D :=	install -m 0644
INST_X :=	install -m 0755

#**  files
#
SRCS :=		$(shell ls src/*.c)
OBJS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.o)))
DEPS :=		$(OBJS:.o=.d)
HDRS :=		$(addprefix include/, uni_json_parser.h uni_json_p_binding.h)

#**  library
#
V_MAJ :=	0
V_MIN :=	1

L_BASE :=	libuni-json.so
L_MAJ :=	$(L_BASE).$(V_MAJ)
LIB :=		$(L_MAJ).$(V_MIN)

#**  CFLAGS
#
CFLAGS :=	-g -W -Wall -Wno-pointer-sign -Wno-implicit-fallthrough

ifndef DEV
CFLAGS :=	-O2 $(CFLAGS)
endif

#**  installation
#
PREFIX :=	$(shell scripts/read-prefix ./PREFIX)

MULTI :=	$(shell gcc -print-multiarch)

TARGET :=	$(DESTDIR)$(PREFIX)
TARGET_LIB :=	$(TARGET)/lib/$(MULTI)
TARGET_INC :=	$(TARGET)/include

#**  lib version
#

#*  targets
#
.PHONY: all clean install deb

all: bin/$(L_MAJ) bin/$(L_BASE)
	$(MAKE) -C bindings

deb:
	fakeroot debian/rules binary

install: all
	$(INST_X) -d $(TARGET_INC) $(TARGET_LIB)
	$(INST_D) $(HDRS) $(TARGET_INC)
	$(INST_D) bin/$(LIB) $(TARGET_LIB)
	cd $(TARGET_LIB) && ln -sf $(LIB) $(L_MAJ) && ln -sf $(LIB) $(L_BASE)
	$(MAKE) -C bindings install

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
