import socket
import time

target_ip = "127.0.0.1"
target_port = 4242
buffer_size = 4098


requests = '''GET / HTTP/1.1
Host: python
Transfer-Encoding: chunked

'''

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((target_ip, target_port))
    res = s.send(requests.replace('\n', '\r\n').encode())
    response = s.recv(buffer_size)
    print(response.decode('utf-8'))
