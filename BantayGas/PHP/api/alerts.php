<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT');
header('Access-Control-Allow-Headers: Content-Type');

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
            // Get alerts
            $status = $_GET['status'] ?? 'all';
            $limit = $_GET['limit'] ?? 50;
            
            $query = "SELECT a.*, s.sensor_name, s.location, s.sensor_type, s.threshold_value, s.last_reading
                     FROM alerts a 
                     JOIN sensors s ON a.sensor_id = s.id";
            
            if ($status !== 'all') {
                $query .= " WHERE a.status = :status";
            }
            
            $query .= " ORDER BY a.created_at DESC LIMIT :limit";
            
            $stmt = $db->prepare($query);
            
            if ($status !== 'all') {
                $stmt->bindParam(':status', $status);
            }
            $stmt->bindParam(':limit', $limit, PDO::PARAM_INT);
            $stmt->execute();
            
            $alerts = $stmt->fetchAll(PDO::FETCH_ASSOC);
            
            // Get alert statistics
            $stats_query = "SELECT 
                           COUNT(*) as total,
                           SUM(CASE WHEN status = 'active' THEN 1 ELSE 0 END) as active,
                           SUM(CASE WHEN status = 'acknowledged' THEN 1 ELSE 0 END) as acknowledged,
                           SUM(CASE WHEN status = 'resolved' THEN 1 ELSE 0 END) as resolved,
                           SUM(CASE WHEN alert_type = 'critical' THEN 1 ELSE 0 END) as critical,
                           SUM(CASE WHEN alert_type = 'warning' THEN 1 ELSE 0 END) as warning
                           FROM alerts";
            $stmt = $db->prepare($stats_query);
            $stmt->execute();
            $stats = $stmt->fetch(PDO::FETCH_ASSOC);
            
            echo json_encode(array(
                'status' => 'success',
                'data' => array(
                    'alerts' => $alerts,
                    'statistics' => $stats
                )
            ));
            break;

        case 'POST':
            // Create new alert
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['sensor_id']) || !isset($input['alert_type']) || !isset($input['message'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Sensor ID, alert type, and message are required'
                ));
                exit;
            }
            
            $sensor_id = $input['sensor_id'];
            $alert_type = $input['alert_type'];
            $message = $input['message'];
            $status = $input['status'] ?? 'active';
            
            $query = "INSERT INTO alerts (sensor_id, alert_type, message, status) 
                     VALUES (:sensor_id, :alert_type, :message, :status)";
            
            $stmt = $db->prepare($query);
            $stmt->bindParam(':sensor_id', $sensor_id);
            $stmt->bindParam(':alert_type', $alert_type);
            $stmt->bindParam(':message', $message);
            $stmt->bindParam(':status', $status);
            
            if ($stmt->execute()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Alert created successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to create alert'
                ));
            }
            break;

        case 'PUT':
            // Update alert status
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['id']) || !isset($input['status'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Alert ID and status are required'
                ));
                exit;
            }
            
            $alert_id = $input['id'];
            $status = $input['status'];
            
            $query = "UPDATE alerts SET status = :status";
            
            if ($status === 'resolved') {
                $query .= ", resolved_at = NOW()";
            } else {
                $query .= ", resolved_at = NULL";
            }
            
            $query .= " WHERE id = :id";
            
            $stmt = $db->prepare($query);
            $stmt->bindParam(':status', $status);
            $stmt->bindParam(':id', $alert_id);
            
            if ($stmt->execute()) {
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Alert updated successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to update alert'
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