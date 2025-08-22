# uni-json Makefile
#

#*  targets
#
.PHONY: all

all: bin/uni-json.so

bin/uni-json.so: tmp/uni_json_parser.o

tmp/uni_json_parser.o: src/uni_json_parser.c include/uni_json_parser.h include/uni_json_p_binding.h

#*  %-rules
#
tmp/%.o: src/%.c
	$(CC) -Iinclude -g -c -o $@ -fpic $<
