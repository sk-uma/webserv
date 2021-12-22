#!/usr/bin/python3

import sys

print("Content-Type: text/plain", end="\n\n")

print("argv:")
print(*sys.argv, sep="\n")
