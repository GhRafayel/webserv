#!/usr/bin/python3
import time
import sys

t = 20
time.sleep(int(t))

print("Content-Type: text/html\r\n")
print("<p>Slept", t, "seconds</p>")