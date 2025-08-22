# uni-json Makefile
#

#*  definitions
#**  programs
#
GCC :=		gcc
CC :=		$(GCC) -Iinclude

#**  files
#
SRCS :=		$(shell ls src/*.c)
OBJS :=		$(addprefix tmp/, $(notdir $(SRCS:.c=.o)))
DEPS :=		$(OBJS:.o=.d)

#**  CFLAGS
#
CFLAGS := -g -W -Wall

ifdef FINAL
CFLAGS := -O2 $(CFLAGS)
endif

#*  targets
#
.PHONY: all clean

all: bin/uni-json.so

clean:
	-rm tmp/*.o

bin/uni-json.so: $(OBJS)

include $(DEPS)

#*  %-rules
#
tmp/%.d: src/%.c
	$(CC) -MM $< | perl -pe 's|$*\.o|$@|' >$@

tmp/%.o: src/%.c tmp/%.d
	$(CC) $(CFLAGS) -c -o $@ -fpic $<
