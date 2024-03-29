#!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()
upload_path = 'www/upload_file/'
form = cgi.FieldStorage()

#import sys

#a = input()
#print(a)
#stdin_str = sys.stdin.read()
#print(stdin_str)

print( """<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
    body {
        width: 35em;
        margin: 0 auto;
        font-family: Tahoma, Verdana, Arial, sans-serif;
    }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>※これはダミーページです</p>
<p>If you see this page, the nginx web server is successfully installed and
working. Further configuration is required.</p>

<p>For online documentation and support please refer to 
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at 
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>""")

print(os.environ)
for key in form:
    value = form[key].value
    print('<p>%s: %s</p>' % (key, value))
