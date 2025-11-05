#!/usr/bin/python
#
# convert a text file to a C macrp definition
#

#*  imports
#
import sys

#*  functions
#
def to_C(line):
    return str.format('"{}"', line.replace('\n', '\\n').replace('\t', '\\t').replace('"', '\\"'))

def pr_C_no_nl(line):
    print(to_C(line), end='')

#*  main
#
print(str.format('#define {} \\', sys.argv[1]))

pr_C_no_nl(sys.stdin.readline())
for line in sys.stdin:
    print('\\')
    pr_C_no_nl(line)

print('')
