// Monitoring Page JavaScript
class MonitoringManager {
    constructor() {
        this.currentFloor = 'ground';
        this.sensors = [];
        this.alerts = [];
        this.init();
    }

    async init() {
        try {
            await this.loadMonitoringData();
            this.setupEventListeners();
            this.startRealTimeUpdates();
        } catch (error) {
            console.error('Failed to initialize monitoring:', error);
            this.showError('Failed to load monitoring data. Please refresh the page.');
        }
    }

    async loadMonitoringData() {
        try {
            const response = await api.getMonitoringData();
            
            if (response.status === 'success') {
                this.sensors = response.data.sensors;
                this.alerts = response.data.alerts;
                
                this.updateStatistics(response.data.statistics);
                this.updateSensorDisplay();
                this.updateAlertsDisplay();
                this.updateFloorPlans(response.data.floor_plans);
            }
        } catch (error) {
            console.error('Error loading monitoring data:', error);
            // Fallback to static data
            this.loadStaticData();
        }
    }

    loadStaticData() {
        // Fallback static data for when backend is not available
        this.sensors = [
            {
                id: 1,
                sensor_name: 'Sensor_001',
                sensor_type: 'CO',
                location: 'Kitchen',
                last_reading: 88,
                threshold_value: 50,
                status: 'critical'
            },
            {
                id: 2,
                sensor_name: 'Sensor_002',
                sensor_type: 'CO',
                location: 'Living Room',
                last_reading: 88,
                threshold_value: 50,
                status: 'critical'
            },
            {
                id: 3,
                sensor_name: 'Sensor_003',
                sensor_type: 'CO',
                location: 'Bedroom',
                last_reading: 88,
                threshold_value: 50,
                status: 'critical'
            }
        ];

        this.alerts = [
            {
                id: 1,
                sensor_name: 'Sensor_001',
                alert_type: 'critical',
                message: 'CO level exceeds threshold',
                status: 'active',
                created_at: new Date().toISOString()
            }
        ];

        this.updateStatistics({
            total_sensors: this.sensors.length,
            critical_sensors: this.sensors.filter(s => s.status === 'critical').length,
            active_alerts: this.alerts.filter(a => a.status === 'active').length,
            normal_sensors: this.sensors.filter(s => s.status === 'normal').length
        });

        this.updateSensorDisplay();
        this.updateAlertsDisplay();
    }

    updateStatistics(stats) {
        // Update statistics display
        const totalSensorsEl = document.querySelector('.stat-card:nth-child(1) .stat-number');
        const criticalSensorsEl = document.querySelector('.stat-card:nth-child(2) .stat-number');
        const activeAlertsEl = document.querySelector('.stat-card:nth-child(3) .stat-number');
        const normalSensorsEl = document.querySelector('.stat-card:nth-child(4) .stat-number');

        if (totalSensorsEl) totalSensorsEl.textContent = stats.total_sensors;
        if (criticalSensorsEl) criticalSensorsEl.textContent = stats.critical_sensors;
        if (activeAlertsEl) activeAlertsEl.textContent = stats.active_alerts;
        if (normalSensorsEl) normalSensorsEl.textContent = stats.normal_sensors;
    }

    updateSensorDisplay() {
        const sensorContainer = document.querySelector('.sensor-indicators');
        if (!sensorContainer) return;

        // Clear existing sensors
        sensorContainer.innerHTML = '';

        // Add sensors to the display
        this.sensors.forEach(sensor => {
            const sensorElement = this.createSensorElement(sensor);
            sensorContainer.appendChild(sensorElement);
        });
    }

    createSensorElement(sensor) {
        const sensorDiv = document.createElement('div');
        sensorDiv.className = `sensor sensor-${sensor.status}`;
        
        // Random positioning for demo (in real app, this would come from database)
        const positions = [
            { top: '15%', left: '75%' },
            { top: '45%', left: '20%' },
            { top: '75%', left: '80%' }
        ];
        const position = positions[Math.floor(Math.random() * positions.length)];
        
        sensorDiv.style.top = position.top;
        sensorDiv.style.left = position.left;

        sensorDiv.innerHTML = `
            <div class="sensor-pin"></div>
            <div class="sensor-info">
                <div class="sensor-name">${sensor.sensor_name}</div>
                <div class="sensor-type">${sensor.sensor_type}</div>
                <div class="sensor-value">${sensor.last_reading} ppm</div>
                <div class="sensor-status ${sensor.status}">Status: ${sensor.status.charAt(0).toUpperCase() + sensor.status.slice(1)}</div>
            </div>
        `;

        return sensorDiv;
    }

    updateAlertsDisplay() {
        const alertsContainer = document.querySelector('.alerts-section .alerts-list');
        if (!alertsContainer) return;

        // Clear existing alerts
        alertsContainer.innerHTML = '';

        if (this.alerts.length === 0) {
            alertsContainer.innerHTML = '<div class="no-alerts">No active alerts</div>';
            return;
        }

        // Add alerts to the display
        this.alerts.forEach(alert => {
            const alertElement = this.createAlertElement(alert);
            alertsContainer.appendChild(alertElement);
        });
    }

    createAlertElement(alert) {
        const alertDiv = document.createElement('div');
        alertDiv.className = `alert-item alert-${alert.alert_type}`;
        
        const timeAgo = this.getTimeAgo(alert.created_at);

        alertDiv.innerHTML = `
            <div class="alert-icon">
                ${alert.alert_type === 'critical' ? '🚨' : '⚠️'}
            </div>
            <div class="alert-content">
                <div class="alert-title">${alert.sensor_name}</div>
                <div class="alert-message">${alert.message}</div>
                <div class="alert-time">${timeAgo}</div>
            </div>
            <div class="alert-actions">
                <button class="btn-acknowledge" onclick="monitoringManager.acknowledgeAlert(${alert.id})">
                    Acknowledge
                </button>
            </div>
        `;

        return alertDiv;
    }

    updateFloorPlans(floorPlans) {
        const floorSelect = document.getElementById('floorSelect');
        if (!floorSelect || !floorPlans) return;

        // Clear existing options
        floorSelect.innerHTML = '';

        // Add floor plan options
        floorPlans.forEach(plan => {
            const option = document.createElement('option');
            option.value = plan.id;
            option.textContent = plan.floor_plan_name;
            floorSelect.appendChild(option);
        });
    }

    setupEventListeners() {
        // Floor selection
        const floorSelect = document.getElementById('floorSelect');
        if (floorSelect) {
            floorSelect.addEventListener('change', (e) => {
                this.currentFloor = e.target.value;
                this.loadMonitoringData();
            });
        }

        // Refresh button
        const refreshBtn = document.querySelector('.refresh-btn');
        if (refreshBtn) {
            refreshBtn.addEventListener('click', () => {
                this.loadMonitoringData();
            });
        }
    }

    async acknowledgeAlert(alertId) {
        try {
            await api.updateAlertStatus(alertId, 'acknowledged');
            
            // Update local alerts array
            const alert = this.alerts.find(a => a.id === alertId);
            if (alert) {
                alert.status = 'acknowledged';
                this.updateAlertsDisplay();
            }
            
            this.showSuccess('Alert acknowledged successfully');
        } catch (error) {
            console.error('Failed to acknowledge alert:', error);
            this.showError('Failed to acknowledge alert');
        }
    }

    startRealTimeUpdates() {
        // Simulate real-time updates every 30 seconds
        setInterval(() => {
            this.loadMonitoringData();
        }, 30000);
    }

    getTimeAgo(dateString) {
        const now = new Date();
        const date = new Date(dateString);
        const diffInSeconds = Math.floor((now - date) / 1000);

        if (diffInSeconds < 60) {
            return `${diffInSeconds} seconds ago`;
        } else if (diffInSeconds < 3600) {
            return `${Math.floor(diffInSeconds / 60)} minutes ago`;
        } else if (diffInSeconds < 86400) {
            return `${Math.floor(diffInSeconds / 3600)} hours ago`;
        } else {
            return `${Math.floor(diffInSeconds / 86400)} days ago`;
        }
    }

    showSuccess(message) {
        // Create success notification
        const notification = document.createElement('div');
        notification.className = 'notification success';
        notification.textContent = message;
        document.body.appendChild(notification);

        setTimeout(() => {
            notification.remove();
        }, 3000);
    }

    showError(message) {
        // Create error notification
        const notification = document.createElement('div');
        notification.className = 'notification error';
        notification.textContent = message;
        document.body.appendChild(notification);

        setTimeout(() => {
            notification.remove();
        }, 5000);
    }
}

// Initialize monitoring when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    window.monitoringManager = new MonitoringManager();
});
