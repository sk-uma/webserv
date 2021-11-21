#!/usr/local/bin/python3

import os
import cgi
import cgitb

cgitb.enable()
upload_path = 'www/upload_file/'
form = cgi.FieldStorage()

file_name = form["delfile"].value
path = upload_path + file_name
if os.path.exists(path):
    os.remove(path)
    body = file_name + ' has been removed.'
else:
    body = file_name + ' is not found.'

print(body)
print(os.environ)
