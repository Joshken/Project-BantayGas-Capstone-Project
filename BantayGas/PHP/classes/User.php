<?php
require_once 'config/database.php';

/**
 * User Class for BantayGas
 * Handles user authentication and management
 */
class User {
    private $conn;
    private $table_name = "users";

    public $id;
    public $email;
    public $password;
    public $first_name;
    public $last_name;
    public $role;
    public $status;
    public $created_at;
    public $updated_at;

    public function __construct($db) {
        $this->conn = $db;
    }

    /**
     * Create new user
     */
    public function create() {
        $query = "INSERT INTO " . $this->table_name . "
                  SET email=:email, password=:password, first_name=:first_name, 
                      last_name=:last_name, role=:role, status=:status";

        $stmt = $this->conn->prepare($query);

        // Hash password
        $this->password = password_hash($this->password, PASSWORD_DEFAULT);

        // Bind values
        $stmt->bindParam(":email", $this->email);
        $stmt->bindParam(":password", $this->password);
        $stmt->bindParam(":first_name", $this->first_name);
        $stmt->bindParam(":last_name", $this->last_name);
        $stmt->bindParam(":role", $this->role);
        $stmt->bindParam(":status", $this->status);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Login user
     */
    public function login($email, $password) {
        $query = "SELECT id, email, password, first_name, last_name, role, status
                  FROM " . $this->table_name . "
                  WHERE email = :email AND status = 'active'";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":email", $email);
        $stmt->execute();

        if($stmt->rowCount() > 0) {
            $row = $stmt->fetch(PDO::FETCH_ASSOC);
            
            if(password_verify($password, $row['password'])) {
                $this->id = $row['id'];
                $this->email = $row['email'];
                $this->first_name = $row['first_name'];
                $this->last_name = $row['last_name'];
                $this->role = $row['role'];
                $this->status = $row['status'];
                return true;
            }
        }
        return false;
    }

    /**
     * Get user by ID
     */
    public function getUserById($id) {
        $query = "SELECT id, email, first_name, last_name, role, status, created_at
                  FROM " . $this->table_name . "
                  WHERE id = :id";

        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(":id", $id);
        $stmt->execute();

        if($stmt->rowCount() > 0) {
            $row = $stmt->fetch(PDO::FETCH_ASSOC);
            $this->id = $row['id'];
            $this->email = $row['email'];
            $this->first_name = $row['first_name'];
            $this->last_name = $row['last_name'];
            $this->role = $row['role'];
            $this->status = $row['status'];
            $this->created_at = $row['created_at'];
            return true;
        }
        return false;
    }

    /**
     * Update user profile
     */
    public function updateProfile() {
        $query = "UPDATE " . $this->table_name . "
                  SET first_name=:first_name, last_name=:last_name, updated_at=NOW()
                  WHERE id=:id";

        $stmt = $this->conn->prepare($query);

        $stmt->bindParam(":first_name", $this->first_name);
        $stmt->bindParam(":last_name", $this->last_name);
        $stmt->bindParam(":id", $this->id);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Change password
     */
    public function changePassword($new_password) {
        $query = "UPDATE " . $this->table_name . "
                  SET password=:password, updated_at=NOW()
                  WHERE id=:id";

        $stmt = $this->conn->prepare($query);

        $hashed_password = password_hash($new_password, PASSWORD_DEFAULT);
        $stmt->bindParam(":password", $hashed_password);
        $stmt->bindParam(":id", $this->id);

        if($stmt->execute()) {
            return true;
        }
        return false;
    }

    /**
     * Get all users (for admin)
     */
    public function getAllUsers() {
        $query = "SELECT id, email, first_name, last_name, role, status, created_at
                  FROM " . $this->table_name . "
                  ORDER BY created_at DESC";

        $stmt = $this->conn->prepare($query);
        $stmt->execute();

        return $stmt->fetchAll(PDO::FETCH_ASSOC);
    }

    /**
     * Delete user
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
     * Update user profile (including role and status for admin)
     */
    public function updateProfile() {
        $query = "UPDATE " . $this->table_name . "
                  SET first_name=:first_name, last_name=:last_name, updated_at=NOW()";
        
        // Add role and status updates if they exist
        if (isset($this->role)) {
            $query .= ", role=:role";
        }
        if (isset($this->status)) {
            $query .= ", status=:status";
        }
        
        $query .= " WHERE id=:id";

        $stmt = $this->conn->prepare($query);

        $stmt->bindParam(":first_name", $this->first_name);
        $stmt->bindParam(":last_name", $this->last_name);
        $stmt->bindParam(":id", $this->id);
        
        if (isset($this->role)) {
            $stmt->bindParam(":role", $this->role);
        }
        if (isset($this->status)) {
            $stmt->bindParam(":status", $this->status);
        }

        if($stmt->execute()) {
            return true;
        }
        return false;
    }
}
?>
