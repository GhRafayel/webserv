<?php
header("Content-Type: text/html");
echo "<h1>CGI Handler Status: Active</h1>";
echo "<p>PHP is executing correctly via the CGI handler.</p>";
echo "<p>Current Server Time: " . date('Y-m-d H:i:s') . "</p>";
?>