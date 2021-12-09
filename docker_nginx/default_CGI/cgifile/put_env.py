#!/usr/bin/python3

import os
environ = os.environ

print("Content-Type: text/plain", end="\n\n")

for key, val in environ.items():
  print("{:25s} => {}".format(key, val))
