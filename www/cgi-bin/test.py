#!/usr/bin/python3

import os
import sys
from urllib.parse import parse_qs

print("Content-Type: text/html; charset=UTF-8\r\n")
#print()  # must be empty line

print("""
<!DOCTYPE html>
<html>
<head>
    <title>Python CGI Debug (no cgi module)</title>
    <style>
        body { font-family: Arial; padding: 20px; background: #f0f0f0; }
        h1 { color: #333; }
        pre { background: #fff; padding: 10px; border: 1px solid #ddd; }
    </style>
</head>
<body>
    <h1>✅ Python CGI Debug (no cgi module)</h1>
""")

# Python version
print(f"<p><strong>Python version:</strong> {sys.version}</p>")

# Environment Variables
print("<h2>Environment Variables:</h2>")
print("<pre>")
for key in sorted(os.environ.keys()):
    print(f"{key} = {os.environ[key]}")
print("</pre>")

# GET Data
query_string = os.environ.get("QUERY_STRING", "")
get_data = parse_qs(query_string)
print("<h2>GET Dat:</h2>")
print("<pre>")
if get_data:
    for key, values in get_data.items():
        print(f"{key} = {values[0]}")
else:
    print("No GET data")
print("</pre>")

# POST Data
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
post_data = {}
if content_length > 0:
    post_bytes = sys.stdin.read(content_length)
    post_data = parse_qs(post_bytes)
print("<h2>POST Data:</h2>")
print("<pre>")
if post_data:
    for key, values in post_data.items():
        print(f"{key} = {values[0]}")
else:
    print("No POST data")
print("</pre>")

print("</body></html>")