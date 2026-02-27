<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html>
<html>
<head>
    <title>Success!</title>
    <style>
        body { 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            font-family: Arial;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            text-align: center;
            margin: 0;
        }
        .box {
            background: rgba(255,255,255,0.1);
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        h1 { font-size: 3em; margin: 0; }
        .info { margin-top: 20px; font-size: 0.9em; opacity: 0.8; }
    </style>
</head>
<body>
    <div class="box">
        <h1>✅ SUCCESS!</h1>
        <p>Your PHP CGI is working perfectly!</p>
        <p class="info">
            PHP <?php echo phpversion(); ?> • 
            <?php echo $_SERVER['REQUEST_METHOD']; ?> • 
            <?php echo date('Y-m-d H:i:s'); ?>
        </p>
        <?php if (!empty($_GET)): ?>
            <p>GET Data: <?php echo htmlspecialchars(json_encode($_GET)); ?></p>
        <?php endif; ?>
    </div>
</body>
</html>
