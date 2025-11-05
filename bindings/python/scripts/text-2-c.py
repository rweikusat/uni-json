#!/usr/bin/python
#
# convert a text file to a C macrp definition
#

#*  imports
#
import sys

#*  variables
#
xlates = [
    ['\n', '\\n'],
    ['\t', '\\t'],
    ['"', '\\"']]

#*  functions
#
def xlate(s):
    for xl in xlates:
        s = s.replace(xl[0], xl[1])
    return str.format('"{}"', s)

def pr_C_no_nl(line):
    print(xlate(line), end='')

#*  main
#
print(str.format('#define {} \\', sys.argv[1]))

pr_C_no_nl(sys.stdin.readline())
for line in sys.stdin:
    print('\\')
    pr_C_no_nl(line)

print('')
