<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');
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
            // Get monitoring dashboard data
            $floor_plan_id = $_GET['floor_plan_id'] ?? null;
            
            if ($floor_plan_id) {
                // Get sensors for specific floor plan
                $sensors = $sensor->getSensorsByFloorPlan($floor_plan_id);
            } else {
                // Get all active sensors
                $sensors = $sensor->getAllSensors();
                $sensors = array_filter($sensors, function($s) {
                    return $s['status'] === 'active';
                });
            }
            
            // Get floor plans
            $floor_plans_query = "SELECT * FROM floor_plans ORDER BY floor_plan_name";
            $stmt = $db->prepare($floor_plans_query);
            $stmt->execute();
            $floor_plans = $stmt->fetchAll(PDO::FETCH_ASSOC);
            
            // Get recent alerts
            $alerts_query = "SELECT a.*, s.sensor_name, s.location 
                           FROM alerts a 
                           JOIN sensors s ON a.sensor_id = s.id 
                           WHERE a.status = 'active' 
                           ORDER BY a.created_at DESC 
                           LIMIT 10";
            $stmt = $db->prepare($alerts_query);
            $stmt->execute();
            $alerts = $stmt->fetchAll(PDO::FETCH_ASSOC);
            
            // Calculate statistics
            $total_sensors = count($sensors);
            $critical_sensors = count(array_filter($sensors, function($s) {
                return $s['last_reading'] > $s['threshold_value'];
            }));
            $active_alerts = count($alerts);
            
            echo json_encode(array(
                'status' => 'success',
                'data' => array(
                    'statistics' => array(
                        'total_sensors' => $total_sensors,
                        'critical_sensors' => $critical_sensors,
                        'active_alerts' => $active_alerts,
                        'normal_sensors' => $total_sensors - $critical_sensors
                    ),
                    'sensors' => $sensors,
                    'floor_plans' => $floor_plans,
                    'alerts' => $alerts
                )
            ));
            break;

        case 'POST':
            // Update sensor reading (simulate real-time data)
            $input = json_decode(file_get_contents('php://input'), true);
            
            if (!isset($input['sensor_id']) || !isset($input['reading_value'])) {
                http_response_code(400);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Sensor ID and reading value are required'
                ));
                exit;
            }
            
            $sensor_id = $input['sensor_id'];
            $reading_value = $input['reading_value'];
            
            // Update sensor reading
            if ($sensor->updateReading($sensor_id, $reading_value)) {
                // Insert reading into sensor_readings table
                $insert_query = "INSERT INTO sensor_readings (sensor_id, reading_value, status) 
                                VALUES (:sensor_id, :reading_value, 
                                        CASE WHEN :reading_value > (SELECT threshold_value FROM sensors WHERE id = :sensor_id) 
                                        THEN 'critical' ELSE 'normal' END)";
                $stmt = $db->prepare($insert_query);
                $stmt->bindParam(':sensor_id', $sensor_id);
                $stmt->bindParam(':reading_value', $reading_value);
                $stmt->execute();
                
                // Check if alert should be created
                $threshold_query = "SELECT threshold_value FROM sensors WHERE id = :sensor_id";
                $stmt = $db->prepare($threshold_query);
                $stmt->bindParam(':sensor_id', $sensor_id);
                $stmt->execute();
                $threshold = $stmt->fetchColumn();
                
                if ($reading_value > $threshold) {
                    // Create alert
                    $alert_query = "INSERT INTO alerts (sensor_id, alert_type, message, status) 
                                   VALUES (:sensor_id, 'critical', 
                                          CONCAT('Sensor reading (', :reading_value, ') exceeds threshold (', :threshold, ')'), 
                                          'active')";
                    $stmt = $db->prepare($alert_query);
                    $stmt->bindParam(':sensor_id', $sensor_id);
                    $stmt->bindParam(':reading_value', $reading_value);
                    $stmt->bindParam(':threshold', $threshold);
                    $stmt->execute();
                }
                
                echo json_encode(array(
                    'status' => 'success',
                    'message' => 'Sensor reading updated successfully'
                ));
            } else {
                http_response_code(500);
                echo json_encode(array(
                    'status' => 'error',
                    'message' => 'Failed to update sensor reading'
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