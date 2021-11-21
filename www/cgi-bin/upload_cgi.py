#!/usr/local/bin/python3

import os
import cgi
import cgitb

cgitb.enable()
upload_path = 'www/upload_file/'
form = cgi.FieldStorage()

file_name = form["upfile"].filename
path = upload_path + file_name
if os.path.exists(path):
    body = file_name + ' already exists.'
else:
    uploaded_file = open(path, 'wb')
    item = form['upfile']
    while True:
        chunk = item.file.read(1000)
        if not chunk:
            break
        uploaded_file.write(chunk)
    uploaded_file.close()
    body = file_name + ' has been uploaded.'

#print("""<!DOCTYPE html>
#<html>
#<head>
#<title>Welcome to nginx!</title>
#<style>
#    body {
#        width: 35em;
#        margin: 0 auto;
#        font-family: Tahoma, Verdana, Arial, sans-serif;
#    }
#</style>
#</head>
#<body>
#<h1>Upload Result</h1>
#<p>{}</p>
#</body>
#</html>""".format(body))
print(body)
print(os.environ)
