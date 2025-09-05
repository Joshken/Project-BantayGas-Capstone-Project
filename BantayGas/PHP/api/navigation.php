<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');
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

    $method = $_SERVER['REQUEST_METHOD'];

    switch ($method) {
        case 'GET':
            // Get navigation menu based on user role
            $user_role = $_SESSION['user_role'] ?? 'user';
            
            $navigation = array(
                'status' => 'success',
                'data' => array(
                    'user' => array(
                        'id' => $_SESSION['user_id'],
                        'name' => $_SESSION['user_name'],
                        'email' => $_SESSION['user_email'],
                        'role' => $user_role
                    ),
                    'menu' => getNavigationMenu($user_role)
                )
            );
            
            echo json_encode($navigation);
            break;

        case 'POST':
            // Handle navigation actions (like updating active page)
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (isset($input['action'])) {
                switch ($input['action']) {
                    case 'update_active_page':
                        // Update user's last visited page
                        $page = $input['page'] ?? '';
                        $_SESSION['last_page'] = $page;
                        
                        echo json_encode(array(
                            'status' => 'success',
                            'message' => 'Active page updated'
                        ));
                        break;
                        
                    default:
                        http_response_code(400);
                        echo json_encode(array(
                            'status' => 'error',
                            'message' => 'Invalid action'
                        ));
                        break;
                }
            } else {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Action is required'
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

/**
 * Get navigation menu based on user role
 */
function getNavigationMenu($user_role) {
    $base_menu = array(
        array(
            'id' => 'monitoring',
            'title' => 'Monitoring',
            'icon' => '🏠',
            'url' => 'monitoring.html',
            'permission' => 'view_monitoring'
        ),
        array(
            'id' => 'sensors',
            'title' => 'Sensors',
            'icon' => '📡',
            'url' => 'sensors.html',
            'permission' => 'view_sensors'
        ),
        array(
            'id' => 'alerts',
            'title' => 'Alert Section',
            'icon' => '⚠️',
            'url' => 'alert_sections.html',
            'permission' => 'view_alerts'
        ),
        array(
            'id' => 'settings',
            'title' => 'Settings',
            'icon' => '⚙️',
            'url' => 'settings.html',
            'permission' => 'view_settings'
        )
    );

    // Add admin-only menu items
    if ($user_role === 'admin') {
        $admin_menu = array(
            array(
                'id' => 'staff_management',
                'title' => 'Staff Management',
                'icon' => '👥',
                'url' => 'user_management.html',
                'permission' => 'manage_users'
            )
        );
        
        // Insert staff management before settings
        array_splice($base_menu, -1, 0, $admin_menu);
    }

    return $base_menu;
}
?>

