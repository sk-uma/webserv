with open("./status_code") as f:
    str = [s.strip() for s in f.readlines()]

html_format = """<html>
<head><title>{}</title></head>
<body>
<center><h1>{}</h1></center>
<hr><center>42Tokyo_webserv</center>
</body>
</html>"""

for s in str:
    code = s.split(':')
    html_str = html_format.format(code[0] + " " + code[1], code[0] + " " + code[1])
    with open("./error_file/" + code[0] + ".html", mode='w') as w:
        w.write(html_str)