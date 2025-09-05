<?php
require_once 'config/database.php';

/**
 * Sensor Class for BantayGas
 * Handles gas sensor data and monitoring
 */
class Sensor {
    private $conn;
    private $table_name = "sensors";

    public $id;
    public $sensor_name;
    public $sensor_type;
    public $location;
    public $floor_plan_id;
    public $status;
    public $last_reading;
    public $threshold_value;
    public $created_at;
    public $updated_at;

    public function __construct($db) {
        $this->conn = $db;
    }

    /**
     * Create new sensor
     */
    public function create() {
        $query = "INSERT INTO " . $this->table_name . "
                  SET sensor_name=:sensor_name, sensor_type=:sensor_type, 
                      location=:location, floor_plan_id=:floor_plan_id, 
                      status=:status, threshold_value=:threshold_value";

        $stmt = $this->conn->prepare($query);

        $stmt->bindParam(":sensor_name", $this->sensor_name);
        $stmt->bindParam(":sensor_type", $this->sensor_type);
        $stmt->bindParam(":location", $this->location);
        $stmt->bindParam(":floor_plan_id", $this->floor_plan_id);
        $stmt->bindParam(":status", $this->status);
        $stmt->bindParam(":threshold_value", $this->threshold_value);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Get all sensors
     */
    public function getAllSensors() {
        $query = "SELECT s.*, fp.floor_plan_name
                  FROM " . $this->table_name . " s
                  LEFT JOIN floor_plans fp ON s.floor_plan_id = fp.id
                  ORDER BY s.created_at DESC";

        $stmt = $this->conn->prepare($query);
        $stmt->execute();

        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    /**
     * Get sensor by ID
     */
    public function getSensorById($id) {
        $query = "SELECT s.*, fp.floor_plan_name
                  FROM " . $this->table_name . " s
                  LEFT JOIN floor_plans fp ON s.floor_plan_id = fp.id
                  WHERE s.id = :id";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":id", $id);
        $stmt->execute();

        if($stmt->rowCount() > 0) {
            $row = $stmt->fetch(PDO::FETCH_ASSOC);
            $this->id = $row['id'];
            $this->sensor_name = $row['sensor_name'];
            $this->sensor_type = $row['sensor_type'];
            $this->location = $row['location'];
            $this->floor_plan_id = $row['floor_plan_id'];
            $this->status = $row['status'];
            $this->last_reading = $row['last_reading'];
            $this->threshold_value = $row['threshold_value'];
            $this->created_at = $row['created_at'];
            return true;
        }
        return false;
    }

    /**
     * Update sensor
     */
    public function update() {
        $query = "UPDATE " . $this->table_name . "
                  SET sensor_name=:sensor_name, sensor_type=:sensor_type, 
                      location=:location, floor_plan_id=:floor_plan_id, 
                      status=:status, threshold_value=:threshold_value, updated_at=NOW()
                  WHERE id=:id";

        $stmt = $this->conn->prepare($query);

        $stmt->bindParam(":sensor_name", $this->sensor_name);
        $stmt->bindParam(":sensor_type", $this->sensor_type);
        $stmt->bindParam(":location", $this->location);
        $stmt->bindParam(":floor_plan_id", $this->floor_plan_id);
        $stmt->bindParam(":status", $this->status);
        $stmt->bindParam(":threshold_value", $this->threshold_value);
        $stmt->bindParam(":id", $this->id);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Delete sensor
     */
    public function delete() {
        $query = "DELETE FROM " . $this->table_name . " WHERE id = :id";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":id", $this->id);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Update sensor reading
     */
    public function updateReading($sensor_id, $reading_value) {
        $query = "UPDATE " . $this->table_name . "
                  SET last_reading=:reading_value, updated_at=NOW()
                  WHERE id=:sensor_id";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":reading_value", $reading_value);
        $stmt->bindParam(":sensor_id", $sensor_id);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Get sensors by floor plan
     */
    public function getSensorsByFloorPlan($floor_plan_id) {
        $query = "SELECT * FROM " . $this->table_name . "
                  WHERE floor_plan_id = :floor_plan_id AND status = 'active'
                  ORDER BY sensor_name";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":floor_plan_id", $floor_plan_id);
        $stmt->execute();

        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }
}
?>
