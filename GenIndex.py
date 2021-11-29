#!/usr/local/bin/python3
import os
import glob

env = os.environ
cwd = (env["REQUEST_URI"])
files = sorted(os.listdir("www" + cwd))

str = """
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Directory listing for {}</title>
</head>
<body>
<h1>Directory listing for {}</h1>
<hr>
<ul>
""".format(cwd, cwd)

for file in files:
    if os.path.isdir(os.getcwd() + "/www/" + cwd + "/" + file):
        str += '<li><a href="{}/">{}/</a></li>\n'.format(cwd + "/" + file, file)
    else:
        str += '<li><a href="{}">{}</a></li>\n'.format(cwd + "/" + file, file)

str += """
</ul>
<hr>
</body>
</html>
"""

print(str)
