<?php
header("Content-Type: text/plain");
echo "PHP CGI Environment Check\n";
echo "========================\n";
echo "Gateway Interface: 	" . $_SERVER['GATEWAY_INTERFACE'] . "\n";
echo "Request Method:	 	" . $_SERVER['REQUEST_METHOD'] . "\n";
echo "Server Protocol:   	" . $_SERVER['SERVER_PROTOCOL'] . "\n";
echo "Script Filename:   	" . $_SERVER['SCRIPT_FILENAME'] . "\n";

echo "GATEWAY_INTERFACE:	" . $_SERVER['GATEWAY_INTERFACE'] . "\n";
echo "REQUEST_METHOD:		" . $_SERVER['REQUEST_METHOD'] . "\n";
echo "QUERY_STRING:			" . $_SERVER['QUERY_STRING'] . "\n";
echo "CONTENT_TYPE:			" . $_SERVER['CONTENT_TYPE'] . "\n";
echo "CONTENT_LENGHT:		" . $_SERVER['CONTENT_LENGHT'] . "\n";
echo "SCRIPT_FILENAME:		" . $_SERVER['SCRIPT_FILENAME'] . "\n";
echo "SCRIPT_NAME:			" . $_SERVER['SCRIPT_NAME'] . "\n";
echo "REQUEST_URI:			" . $_SERVER['REQUEST_URI'] . "\n";
echo "REMOTE_ADDR:			" . $_SERVER['REMOTE_ADDR'] . "\n";
echo "REMOTE_PORT:			" . $_SERVER['REMOTE_PORT'] . "\n";
echo "PATH_INFO:			" . $_SERVER['PATH_INFO'] . "\n";
echo "HTTP_USER_AGENT:		" . $_SERVER['HTTP_USER_AGENT'] . "\n";
echo "HTTP_HOST:			" . $_SERVER['HTTP_HOST'] . "\n";
echo "HTTP_ACCEPT_LANGUAGE:	" . $_SERVER['HTTP_ACCEPT_LANGUAGE'] . "\n";
echo "HTTP_COOKIE:			" . $_SERVER['HTTP_COOKIE'] . "\n";
echo "SERVER_NAME:			" . $_SERVER['SERVER_NAME'] . "\n";
echo "SERVER_PORT:			" . $_SERVER['SERVER_PORT'] . "\n";
echo "SERVER_SOFTWARE:		" . $_SERVER['SERVER_SOFTWARE'] . "\n";
echo "SERVER_PROTOCOL:		" . $_SERVER['SERVER_PROTOCOL'] . "\n";
echo "DOCUMENT_ROOT:		" . $_SERVER['DOCUMENT_ROOT'] . "\n";

echo "PHP Version:       " . phpversion() . "\n";
?>