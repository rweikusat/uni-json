#!/usr/bin/python
#
# convert stdin to a C macro definition
#

#*  imports
#
from sys import argv, stdin

#*  variables
#
xlates = (
    ('\n', '\\n'),
    ('\t', '\\t'),
    ('"', '\\"'))

#*  functions
#
def xlate(s):
    for xl in xlates:
        s = s.replace(xl[0], xl[1])
    return f'"{s}"'

def pr_def(name):
    print(f'#define {name} \\')

def pr_xlate(s):
    print(xlate(s), end='')

#*  main
#
pr_def(argv[1])

pr_xlate(stdin.readline())
for line in stdin:
    print('\\')
    pr_xlate(line)

print('')
