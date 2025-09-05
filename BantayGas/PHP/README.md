# BantayGas PHP Backend

This directory contains the PHP backend for the BantayGas Gas Monitoring System.

## Directory Structure

```
PHP/
├── config/
│   └── database.php          # Database configuration
├── classes/
│   ├── User.php             # User management class
│   └── Sensor.php           # Sensor management class
├── api/
│   ├── login.php            # Login API endpoint
│   ├── logout.php           # Logout API endpoint
│   └── sensors.php          # Sensors API endpoint
├── setup_database.sql       # Database setup script
└── README.md               # This file
```

## Setup Instructions

### 1. Database Setup

1. Install MySQL/MariaDB
2. Create a database named `bantaygas_db`
3. Run the SQL script:
   ```sql
   mysql -u root -p bantaygas_db < setup_database.sql
   ```

### 2. Configuration

Update the database credentials in `config/database.php`:

```php
private $host = 'localhost';
private $db_name = 'bantaygas_db';
private $username = 'your_username';
private $password = 'your_password';
```

### 3. Web Server Setup

#### For Apache:
- Ensure mod_rewrite is enabled
- The `.htaccess` file in the root directory handles routing

#### For IIS:
- Install PHP for IIS
- Configure URL rewriting in `Web.config`

### 4. Default Login Credentials

After running the database setup, you can login with:
- **Email:** baguiojosh@gmail.com
- **Password:** password

## API Endpoints

### Authentication

#### POST /api/login
Login user
```json
{
  "email": "user@example.com",
  "password": "password"
}
```

#### POST /api/logout
Logout user

### Sensors

#### GET /api/sensors
Get all sensors

#### GET /api/sensors?id={id}
Get specific sensor

#### POST /api/sensors
Create new sensor
```json
{
  "sensor_name": "Gas Sensor 1",
  "sensor_type": "gas",
  "location": "Kitchen",
  "threshold_value": 50.00
}
```

#### PUT /api/sensors
Update sensor
```json
{
  "id": 1,
  "sensor_name": "Updated Sensor",
  "sensor_type": "gas",
  "location": "Kitchen",
  "status": "active",
  "threshold_value": 75.00
}
```

#### DELETE /api/sensors
Delete sensor
```json
{
  "id": 1
}
```

## Database Schema

### Users Table
- id (Primary Key)
- email (Unique)
- password (Hashed)
- first_name
- last_name
- role (admin, user, viewer)
- status (active, inactive, suspended)
- created_at, updated_at

### Sensors Table
- id (Primary Key)
- sensor_name
- sensor_type (gas, smoke, temperature, humidity)
- location
- floor_plan_id (Foreign Key)
- status (active, inactive, maintenance)
- last_reading
- threshold_value
- created_at, updated_at

### Floor Plans Table
- id (Primary Key)
- floor_plan_name
- floor_plan_image
- description
- created_by (Foreign Key to Users)
- created_at, updated_at

### Sensor Readings Table
- id (Primary Key)
- sensor_id (Foreign Key)
- reading_value
- reading_timestamp
- status (normal, warning, critical)

### Alerts Table
- id (Primary Key)
- sensor_id (Foreign Key)
- alert_type (warning, critical, maintenance)
- message
- status (active, acknowledged, resolved)
- created_at, resolved_at

## Security Features

- Password hashing using PHP's `password_hash()`
- SQL injection prevention using prepared statements
- CORS headers for API access
- Session management for authentication
- Input validation and sanitization

## Development Notes

- All API responses are in JSON format
- Error handling with appropriate HTTP status codes
- Database connection pooling
- Prepared statements for security
- RESTful API design principles
