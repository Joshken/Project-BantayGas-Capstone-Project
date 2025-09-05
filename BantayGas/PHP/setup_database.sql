-- BantayGas Database Setup
-- Gas Monitoring System Database Schema

CREATE DATABASE IF NOT EXISTS bantaygas_db;
USE bantaygas_db;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    role ENUM('admin', 'user', 'viewer') DEFAULT 'user',
    status ENUM('active', 'inactive', 'suspended') DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Floor plans table
CREATE TABLE IF NOT EXISTS floor_plans (
    id INT AUTO_INCREMENT PRIMARY KEY,
    floor_plan_name VARCHAR(255) NOT NULL,
    floor_plan_image VARCHAR(500),
    description TEXT,
    created_by INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (created_by) REFERENCES users(id) ON DELETE SET NULL
);

-- Sensors table
CREATE TABLE IF NOT EXISTS sensors (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_name VARCHAR(255) NOT NULL,
    sensor_type ENUM('gas', 'smoke', 'temperature', 'humidity') DEFAULT 'gas',
    location VARCHAR(255) NOT NULL,
    floor_plan_id INT,
    status ENUM('active', 'inactive', 'maintenance') DEFAULT 'active',
    last_reading DECIMAL(10,2),
    threshold_value DECIMAL(10,2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (floor_plan_id) REFERENCES floor_plans(id) ON DELETE SET NULL
);

-- Sensor readings table
CREATE TABLE IF NOT EXISTS sensor_readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_id INT NOT NULL,
    reading_value DECIMAL(10,2) NOT NULL,
    reading_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status ENUM('normal', 'warning', 'critical') DEFAULT 'normal',
    FOREIGN KEY (sensor_id) REFERENCES sensors(id) ON DELETE CASCADE
);

-- Alerts table
CREATE TABLE IF NOT EXISTS alerts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_id INT NOT NULL,
    alert_type ENUM('warning', 'critical', 'maintenance') NOT NULL,
    message TEXT NOT NULL,
    status ENUM('active', 'acknowledged', 'resolved') DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    resolved_at TIMESTAMP NULL,
    FOREIGN KEY (sensor_id) REFERENCES sensors(id) ON DELETE CASCADE
);

-- Insert default admin user
INSERT INTO users (email, password, first_name, last_name, role, status) 
VALUES ('baguiojosh@gmail.com', '$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2.uheWG/igi', 'Josh', 'Baguio', 'admin', 'active')
ON DUPLICATE KEY UPDATE email=email;

-- Insert sample floor plan
INSERT INTO floor_plans (floor_plan_name, description, created_by) 
VALUES ('Main Building - Ground Floor', 'Main building ground floor layout', 1)
ON DUPLICATE KEY UPDATE floor_plan_name=floor_plan_name;

-- Insert sample sensors
INSERT INTO sensors (sensor_name, sensor_type, location, floor_plan_id, threshold_value, status) 
VALUES 
('Gas Sensor 1', 'gas', 'Kitchen Area', 1, 50.00, 'active'),
('Gas Sensor 2', 'gas', 'Boiler Room', 1, 75.00, 'active'),
('Smoke Detector 1', 'smoke', 'Main Hallway', 1, 100.00, 'active'),
('Temperature Sensor 1', 'temperature', 'Server Room', 1, 30.00, 'active')
ON DUPLICATE KEY UPDATE sensor_name=sensor_name;

-- Create indexes for better performance
CREATE INDEX idx_sensors_floor_plan ON sensors(floor_plan_id);
CREATE INDEX idx_sensors_status ON sensors(status);
CREATE INDEX idx_sensor_readings_sensor_id ON sensor_readings(sensor_id);
CREATE INDEX idx_sensor_readings_timestamp ON sensor_readings(reading_timestamp);
CREATE INDEX idx_alerts_sensor_id ON alerts(sensor_id);
CREATE INDEX idx_alerts_status ON alerts(status);
CREATE INDEX idx_alerts_created_at ON alerts(created_at);
