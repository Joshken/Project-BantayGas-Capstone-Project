<?php
/**
 * Database Configuration for BantayGas
 * Gas Monitoring System
 */

class Database {
    private $host = 'localhost';
    private $db_name = 'bantaygas_db';
    private $username = 'root';
    private $password = '';
    private $conn;

    /**
     * Get database connection
     */
    public function getConnection() {
        $this->conn = null;

        try {
            $this->conn = new PDO(
                "mysql:host=" . $this->host . ";dbname=" . $this->db_name,
                $this->username,
                $this->password
            );
            $this->conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->conn->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
        } catch(PDOException $exception) {
            echo "Connection error: " . $exception->getMessage();
        }

        return $this->conn;
    }

    /**
     * Test database connection
     */
    public function testConnection() {
        try {
            $conn = $this->getConnection();
            if ($conn) {
                return array(
                    'status' => 'success',
                    'message' => 'Database connection successful'
                );
            }
        } catch(Exception $e) {
            return array(
                'status' => 'error',
                'message' => 'Database connection failed: ' . $e->getMessage()
            );
        }
    }
}
?>
