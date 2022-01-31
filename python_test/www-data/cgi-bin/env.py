#!/usr/bin/env python3

import os

env = lambda name: name + ": " + str(os.getenv(name))

print(env('AUTH_TYPE'))
print(env('CONTENT_LENGTH'))
print(env('CONTENT_TYPE'))
print(env('GATEWAY_INTERFACE'))
print(env('PATH_INFO'))
print(env('PATH_TRANSLATED'))
print(env('QUERY_STRING'))
print(env('REMOTE_ADDR'))
print(env('REMOTE_HOST'))
print(env('REMOTE_IDENT'))
print(env('REMOTE_USER'))
print(env('REQUEST_METHOD'))
print(env('SCRIPT_NAME'))
print(env('SERVER_NAME'))
print(env('SERVER_PORT'))
print(env('SERVER_PROTOCOL'))
print(env('SERVER_SOFTWARE'))
