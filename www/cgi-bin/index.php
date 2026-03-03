<?php
// Webserv test CGI script
// Tests: basic response, query string, POST body, environment variables

header("Content-Type: text/html; charset=UTF-8");

$method = $_SERVER["REQUEST_METHOD"] ?? "UNKNOWN";
$query  = $_SERVER["QUERY_STRING"] ?? "";
$uri    = $_SERVER["REQUEST_URI"] ?? "";
?>
<!DOCTYPE html>
<html>
<head><title>CGI Test - webserv</title></head>
<body>
<h1>PHP CGI Test Page</h1>

<h2>Request Info</h2>
<ul>
  <li><strong>Method:</strong> <?php echo htmlspecialchars($method); ?></li>
  <li><strong>URI:</strong> <?php echo htmlspecialchars($uri); ?></li>
  <li><strong>Query String:</strong> <?php echo htmlspecialchars($query); ?></li>
</ul>

<?php if ($method === "GET" && !empty($_GET)): ?>
<h2>GET Parameters</h2>
<ul>
<?php foreach ($_GET as $k => $v): ?>
  <li><strong><?php echo htmlspecialchars($k); ?>:</strong> <?php echo htmlspecialchars($v); ?></li>
<?php endforeach; ?>
</ul>
<?php endif; ?>

<?php if ($method === "POST"): ?>
<h2>POST Body</h2>
<?php
  $raw = file_get_contents("php://stdin");
  parse_str($raw, $post);
?>
<ul>
<?php foreach ($post as $k => $v): ?>
  <li><strong><?php echo htmlspecialchars($k); ?>:</strong> <?php echo htmlspecialchars($v); ?></li>
<?php endforeach; ?>
</ul>
<?php if (empty($post)): ?>
  <p>Raw body: <?php echo htmlspecialchars($raw); ?></p>
<?php endif; ?>
<?php endif; ?>

<h2>CGI Environment Variables</h2>
<ul>
<?php
$env_vars = [
  "SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL",
  "REQUEST_METHOD", "REQUEST_URI", "QUERY_STRING",
  "CONTENT_TYPE", "CONTENT_LENGTH",
  "HTTP_HOST", "HTTP_USER_AGENT", "HTTP_ACCEPT",
  "GATEWAY_INTERFACE", "PATH_INFO", "SCRIPT_NAME",
];
foreach ($env_vars as $var) {
  $val = getenv($var);
  if ($val !== false) {
    echo "  <li><strong>" . htmlspecialchars($var) . ":</strong> " . htmlspecialchars($val) . "</li>
";
  }
}
?>
</ul>

<h2>Status</h2>
<p style="color: green; font-weight: bold;">PHP CGI is working correctly!</p>
</body>
</html>