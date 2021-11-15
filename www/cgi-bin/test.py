#!/usr/local/bin/python3
# -*- coding:utf-8 -*-
import cgi
import os

print('Content-type: text/html; charset=UTF-8\r\n')
print('Hello, World!')
print(os.environ)
form = cgi.FieldStorage()

for key in form:
    value = form[key].value
    print('<p>%s: %s</p>' % (key, value))