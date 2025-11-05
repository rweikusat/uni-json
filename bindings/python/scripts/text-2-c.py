#!/usr/bin/python
#
# convert stdin to a C macro definition
#

#*  imports
#
from sys import argv, stdin
import re

#*  variables
#
xlates = {
    '\\' : '\\\\',
    '\n' : '\\n',
    '\t' : '\\t',
    '"' : '\\"'}

#*  functions
#
def xlate_map(m):
    return xlates[m.group(1)]

def xlate(pat, s):
    return re.sub(pat, xlate_map, s)

def pr_def(name):
    print(f'#define {name} \\')

def pr_xlate(pat, s):
    print(xlate(pat, s), end='')

def double_bs(s):
    if s == '\\':
        return '\\\\'
    return s

#*  main
#
pat=re.compile(f'({str.join("|", [double_bs(x) for x in xlates.keys()])})')
pr_def(argv[1])

pr_xlate(pat, stdin.readline())
for line in stdin:
    print('\\')
    pr_xlate(pat, line)

print('')
