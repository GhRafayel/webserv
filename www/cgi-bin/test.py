#!/usr/bin/python3

import os
import sys

# HTTP headers
print("Content-Type: text/html; charset=UTF-8")
print()  # Important empty line

# HTML content
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Python CGI Test</title>
    <style>
        body { font-family: Arial; padding: 20px; background: #f0f0f0; }
        h1 { color: #333; }
        pre { background: #fff; padding: 10px; border: 1px solid #ddd; }
        .success { color: green; font-weight: bold; }
    </style>
</head>
<body>
    <h1>✅ Python CGI Աշխատում Է!</h1>
""")

# Show Python version
print(f"<p><strong>Python version:</strong> {sys.version}</p>")

# Show environment variables
print("<h2>Environment Variables:</h2>")
print("<pre>")
for key in ["REQUEST_METHOD", "QUERY_STRING", "SCRIPT_FILENAME", "CONTENT_TYPE"]:
    if key in os.environ:
        print(f"{key} = {os.environ[key]}")
print("</pre>")

# Show GET/POST data
import cgi
form = cgi.FieldStorage()
if form:
    print("<h2>Form Data:</h2>")
    print("<pre>")
    for key in form.keys():
        print(f"{key} = {form[key].value}")
    print("</pre>")

print("</body></html>")
