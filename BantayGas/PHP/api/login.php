<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: POST');
header('Access-Control-Allow-Headers: Content-Type');

require_once '../classes/User.php';
require_once '../config/database.php';

// Only allow POST requests
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(array('message' => 'Method not allowed'));
    exit;
}

// Get JSON input
$input = json_decode(file_get_contents('php://input'), true);

// Validate input
if (!isset($input['email']) || !isset($input['password'])) {
    http_response_code(400);
    echo json_encode(array(
        'status' => 'error',
        'message' => 'Email and password are required'
    ));
    exit;
}

$email = trim($input['email']);
$password = $input['password'];

// Validate email format
if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
    http_response_code(400);
    echo json_encode(array(
        'status' => 'error',
        'message' => 'Invalid email format'
    ));
    exit;
}

try {
    // Initialize database connection
    $database = new Database();
    $db = $database->getConnection();

    if ($db === null) {
        http_response_code(500);
        echo json_encode(array(
            'status' => 'error',
            'message' => 'Database connection failed'
        ));
        exit;
    }

    // Initialize User class
    $user = new User($db);

    // Attempt login
    if ($user->login($email, $password)) {
        // Start session
        session_start();
        
        // Store user data in session
        $_SESSION['user_id'] = $user->id;
        $_SESSION['user_email'] = $user->email;
        $_SESSION['user_name'] = $user->first_name . ' ' . $user->last_name;
        $_SESSION['user_role'] = $user->role;
        $_SESSION['is_logged_in'] = true;

        // Return success response
        echo json_encode(array(
            'status' => 'success',
            'message' => 'Login successful',
            'user' => array(
                'id' => $user->id,
                'email' => $user->email,
                'name' => $user->first_name . ' ' . $user->last_name,
                'role' => $user->role
            )
        ));
    } else {
        http_response_code(401);
        echo json_encode(array(
            'status' => 'error',
            'message' => 'Invalid email or password'
        ));
    }

} catch (Exception $e) {
    http_response_code(500);
    echo json_encode(array(
        'status' => 'error',
        'message' => 'Server error: ' . $e->getMessage()
    ));
}
?>
