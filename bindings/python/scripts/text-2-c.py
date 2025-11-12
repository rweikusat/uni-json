#!/usr/bin/python3
#
# convert stdin to a C macro definition
#

#*  imports
#
from sys import argv, stdin
import re

#*  variables
#
XLATES = {
    '\\' : '\\\\',
    '\n' : '\\n',
    '\t' : '\\t',
    '"' : '\\"'}

XLATE_PAT = re.compile('(\\\\|\n|\t|")')

#*  functions
#
def xlate(s):
    return f'"{re.sub(XLATE_PAT, lambda m: XLATES[m.group(1)], s)}"'

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

print()
