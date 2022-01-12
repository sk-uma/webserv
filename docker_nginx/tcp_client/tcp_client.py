import socket

target_ip = "127.0.0.1"
target_port = 80
buffer_size = 4098


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((target_ip,target_port))
    requests = b'''GET / HTTP/1.1\r
Host: localhost\r
Content-Length: 5\r
\r
hello'''
    res = s.send(requests)
    response = s.recv(buffer_size)
    s.shutdown(socket.SHUT_RDWR)
    s.close()
    print(response.decode('utf-8'))
