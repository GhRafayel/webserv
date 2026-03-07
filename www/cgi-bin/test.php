<?php
// Tell browser this is HTML
header('Content-Type: text/html; charset=UTF-8');

// Start HTML output
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head><title>PHP Test</title>";
echo "<style>
    body { font-family: Arial; padding: 20px; }
    pre { background: #f4f4f4; padding: 10px; border: 1px solid #ddd; }
    .success { color: green; font-weight: bold; }
</style>";
echo "</head>";
echo "<body>";

echo "<h1>🔧 PHP Debug Page</h1>";

// Show request method
echo "<h2>Request Method:</h2>";
echo "<pre>" . $_SERVER['REQUEST_METHOD'] . "</pre>";

// Show GET parameters
echo "<h2>GET Parameters:</h2>";
if (!empty($_GET)) {
    echo "<pre>";
    print_r($_GET);
    echo "</pre>";
    echo "<p class='success'>✅ GET parameters received!</p>";
} else {
    echo "<p>No GET parameters</p>";
}

// Show POST parameters
echo "<h2>POST Parameters:</h2>";
if (!empty($_POST)) {
    echo "<pre>";
    print_r($_POST);
    echo "</pre>";
    echo "<p class='success'>✅ POST parameters received!</p>";
} else {
    echo "<p>No POST parameters</p>";
}

// Show query string
echo "<h2>Query String:</h2>";
echo "<pre>" . ($_SERVER['QUERY_STRING'] ?: 'empty') . "</pre>";

echo "</body>";
echo "</html>";
?>
