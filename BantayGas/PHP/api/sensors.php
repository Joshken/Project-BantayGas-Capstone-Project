<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
header('Access-Control-Allow-Headers: Content-Type');

require_once '../classes/Sensor.php';
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

    // Initialize Sensor class
    $sensor = new Sensor($db);

    $method = $_SERVER['REQUEST_METHOD'];

    switch ($method) {
        case 'GET':
            // Get all sensors or specific sensor
            if (isset($_GET['id'])) {
                $sensor_id = $_GET['id'];
                if ($sensor->getSensorById($sensor_id)) {
                    echo json_encode(array(
                        'status' => 'success',
                        'data' => array(
                            'id' => $sensor->id,
                            'sensor_name' => $sensor->sensor_name,
                            'sensor_type' => $sensor->sensor_type,
                            'location' => $sensor->location,
                            'floor_plan_id' => $sensor->floor_plan_id,
                            'status' => $sensor->status,
                            'last_reading' => $sensor->last_reading,
                            'threshold_value' => $sensor->threshold_value,
                            'created_at' => $sensor->created_at
                        )
                    ));
                } else {
                    http_response_code(404);
                    echo json_encode(array(
                        'status' => 'error',
                        'message' => 'Sensor not found'
                    ));
                }
            } else {
                $sensors = $sensor->getAllSensors();
                echo json_encode(array(
                    'status' => 'success',
                    'data' => $sensors
                ));
            }
            break;

        case 'POST':
            // Create new sensor
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['sensor_name']) || !isset($input['sensor_type']) || 
                !isset($input['location']) || !isset($input['threshold_value'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Required fields missing'
                ));
                exit;
            }

            $sensor->sensor_name = $input['sensor_name'];
            $sensor->sensor_type = $input['sensor_type'];
            $sensor->location = $input['location'];
            $sensor->floor_plan_id = isset($input['floor_plan_id']) ? $input['floor_plan_id'] : null;
            $sensor->status = isset($input['status']) ? $input['status'] : 'active';
            $sensor->threshold_value = $input['threshold_value'];

            if ($sensor->create()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Sensor created successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to create sensor'
                ));
            }
            break;

        case 'PUT':
            // Update sensor
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['id'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Sensor ID is required'
                ));
                exit;
            }

            $sensor->id = $input['id'];
            $sensor->sensor_name = $input['sensor_name'];
            $sensor->sensor_type = $input['sensor_type'];
            $sensor->location = $input['location'];
            $sensor->floor_plan_id = isset($input['floor_plan_id']) ? $input['floor_plan_id'] : null;
            $sensor->status = $input['status'];
            $sensor->threshold_value = $input['threshold_value'];

            if ($sensor->update()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Sensor updated successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to update sensor'
                ));
            }
            break;

        case 'DELETE':
            // Delete sensor
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['id'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Sensor ID is required'
                ));
                exit;
            }

            $sensor->id = $input['id'];

            if ($sensor->delete()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Sensor deleted successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to delete sensor'
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
