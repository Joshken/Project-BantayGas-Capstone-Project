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

// Check if user has admin privileges for certain operations
$user_role = $_SESSION['user_role'] ?? 'user';
$is_admin = ($user_role === 'admin');

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
            // Get user profile or all users (admin only)
            if (isset($_GET['id'])) {
                $user_id = $_GET['id'];
                
                // Users can only view their own profile unless they're admin
                if (!$is_admin && $user_id != $_SESSION['user_id']) {
                    http_response_code(403);
                    echo json_encode(array(
                        'status' => 'error',
                        'message' => 'Access denied'
                    ));
                    exit;
                }
                
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
                if (!$is_admin) {
                    http_response_code(403);
                    echo json_encode(array(
                        'status' => 'error',
                        'message' => 'Admin access required'
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

        case 'POST':
            // Create new user (admin only)
            if (!$is_admin) {
                http_response_code(403);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Admin access required'
                ));
                exit;
            }
            
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['email']) || !isset($input['password']) || 
                !isset($input['first_name']) || !isset($input['last_name'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Required fields missing'
                ));
                exit;
            }

            $user->email = $input['email'];
            $user->password = $input['password'];
            $user->first_name = $input['first_name'];
            $user->last_name = $input['last_name'];
            $user->role = $input['role'] ?? 'user';
            $user->status = $input['status'] ?? 'active';

            if ($user->create()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'User created successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to create user'
                ));
            }
            break;

        case 'PUT':
            // Update user profile
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['id'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'User ID is required'
                ));
                exit;
            }
            
            $user_id = $input['id'];
            
            // Users can only update their own profile unless they're admin
            if (!$is_admin && $user_id != $_SESSION['user_id']) {
                http_response_code(403);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Access denied'
                ));
                exit;
            }
            
            $user->id = $user_id;
            $user->first_name = $input['first_name'];
            $user->last_name = $input['last_name'];
            
            // Only admins can change role and status
            if ($is_admin) {
                $user->role = $input['role'] ?? $user->role;
                $user->status = $input['status'] ?? $user->status;
            }

            if ($user->updateProfile()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'User updated successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to update user'
                ));
            }
            break;

        case 'DELETE':
            // Delete user (admin only)
            if (!$is_admin) {
                http_response_code(403);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Admin access required'
                ));
                exit;
            }
            
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['id'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'User ID is required'
                ));
                exit;
            }
            
            $user->id = $input['id'];
            
            // Prevent admin from deleting themselves
            if ($user->id == $_SESSION['user_id']) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Cannot delete your own account'
                ));
                exit;
            }

            if ($user->delete()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'User deleted successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to delete user'
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

