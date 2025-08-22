# uni-json Makefile
#

#*  definitions
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

bin/uni-json.so: tmp/uni_json_parser.o

tmp/uni_json_parser.o: src/uni_json_parser.c include/uni_json_parser.h include/uni_json_p_binding.h

#*  %-rules
#
tmp/%.o: src/%.c
	$(CC) -Iinclude $(CFLAGS) -c -o $@ -fpic $<
