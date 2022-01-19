#!/usr/local/bin/python3
# -*- coding:utf-8 -*-
import sys
import urllib.parse

input = sys.stdin.read()

print(urllib.parse.unquote_plus(input))