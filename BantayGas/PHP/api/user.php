<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
header('Access-Control-Allow-Headers: Content-Type');

require_once '../classes/User.php';
require_once '../config/database.php';

// Check if user is logged in
session_start();
if (!isset($_SESSION['is_logged_in']) || $_SESSION['is_logged_in'] !== true) {
    http_response_code(401);
    echo json_encode(array(
        'status' => 'error',
        'message' => 'Unauthorized access'
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

    $method = $_SERVER['REQUEST_METHOD'];

    switch ($method) {
        case 'GET':
            // Get current user info
            if (isset($_GET['profile'])) {
                $user_id = $_SESSION['user_id'];
                if ($user->getUserById($user_id)) {
                    echo json_encode(array(
                        'status' => 'success',
                        'data' => array(
                            'id' => $user->id,
                            'email' => $user->email,
                            'first_name' => $user->first_name,
                            'last_name' => $user->last_name,
                            'role' => $user->role,
                            'status' => $user->status,
                            'created_at' => $user->created_at
                        )
                    ));
                } else {
                    http_response_code(404);
                    echo json_encode(array(
                        'status' => 'error',
                        'message' => 'User not found'
                    ));
                }
            } else {
                // Get all users (admin only)
                if ($_SESSION['user_role'] !== 'admin') {
                    http_response_code(403);
                    echo json_encode(array(
                        'status' => 'error',
                        'message' => 'Access denied. Admin privileges required.'
                    ));
                    exit;
                }

                $users = $user->getAllUsers();
                echo json_encode(array(
                    'status' => 'success',
                    'data' => $users
                ));
            }
            break;

        case 'PUT':
            // Update user profile
            $input = json_decode(file_get_contents('php://input'), true);
            
            $user->id = $_SESSION['user_id'];
            $user->first_name = $input['first_name'];
            $user->last_name = $input['last_name'];

            if ($user->updateProfile()) {
                // Update session data
                $_SESSION['user_name'] = $user->first_name . ' ' . $user->last_name;
                
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Profile updated successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to update profile'
                ));
            }
            break;

        case 'POST':
            // Change password
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['new_password'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'New password is required'
                ));
                exit;
            }

            $user->id = $_SESSION['user_id'];
            if ($user->changePassword($input['new_password'])) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Password changed successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to change password'
                ));
            }
            break;

        default:
            http_response_code(405);
            echo json_encode(array(
                'status' => 'error',
                'message' => 'Method not allowed'
            ));
            break;
    }

} catch (Exception $e) {
    http_response_code(500);
    echo json_encode(array(
        'status' => 'error',
        'message' => 'Server error: ' . $e->getMessage()
    ));
}
?>
