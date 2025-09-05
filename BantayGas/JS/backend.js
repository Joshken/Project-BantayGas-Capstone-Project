/**
 * Backend Integration for BantayGas
 * Handles all API calls and data management
 */

class BantayGasAPI {
    constructor() {
        this.baseURL = '/api';
        this.isLoggedIn = sessionStorage.getItem('isLoggedIn') === 'true';
    }

    /**
     * Make API request
     */
    async makeRequest(endpoint, options = {}) {
        const url = `${this.baseURL}${endpoint}`;
        const defaultOptions = {
            headers: {
                'Content-Type': 'application/json',
            },
        };

        const config = { ...defaultOptions, ...options };

        try {
            const response = await fetch(url, config);
            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.message || 'API request failed');
            }

            return data;
        } catch (error) {
            console.error('API Error:', error);
            throw error;
        }
    }

    /**
     * Authentication methods
     */
    async login(email, password) {
        const response = await this.makeRequest('/login.php', {
            method: 'POST',
            body: JSON.stringify({ email, password })
        });

        if (response.status === 'success') {
            sessionStorage.setItem('isLoggedIn', 'true');
            sessionStorage.setItem('userName', response.user.name);
            sessionStorage.setItem('userRole', response.user.role);
            sessionStorage.setItem('userId', response.user.id);
        }

        return response;
    }

    async logout() {
        try {
            await this.makeRequest('/logout.php', {
                method: 'POST'
            });
        } catch (error) {
            console.error('Logout error:', error);
        } finally {
            sessionStorage.clear();
            window.location.href = 'login.html';
        }
    }

    /**
     * User management methods
     */
    async getUserProfile() {
        return await this.makeRequest('/user.php?profile=true');
    }

    async getAllUsers() {
        return await this.makeRequest('/user.php');
    }

    async updateProfile(firstName, lastName) {
        return await this.makeRequest('/user.php', {
            method: 'PUT',
            body: JSON.stringify({ first_name: firstName, last_name: lastName })
        });
    }

    async changePassword(newPassword) {
        return await this.makeRequest('/user.php', {
            method: 'POST',
            body: JSON.stringify({ new_password: newPassword })
        });
    }

    /**
     * Sensor management methods
     */
    async getAllSensors() {
        return await this.makeRequest('/sensors.php');
    }

    async getSensorById(id) {
        return await this.makeRequest(`/sensors.php?id=${id}`);
    }

    async createSensor(sensorData) {
        return await this.makeRequest('/sensors.php', {
            method: 'POST',
            body: JSON.stringify(sensorData)
        });
    }

    async updateSensor(sensorData) {
        return await this.makeRequest('/sensors.php', {
            method: 'PUT',
            body: JSON.stringify(sensorData)
        });
    }

    async deleteSensor(id) {
        return await this.makeRequest('/sensors.php', {
            method: 'DELETE',
            body: JSON.stringify({ id })
        });
    }

    /**
     * Monitoring methods
     */
    async getMonitoringData(floorPlanId = null) {
        const endpoint = floorPlanId ? `/monitoring.php?floor_plan_id=${floorPlanId}` : '/monitoring.php';
        return await this.makeRequest(endpoint);
    }

    async updateSensorReading(sensorId, readingValue) {
        return await this.makeRequest('/monitoring.php', {
            method: 'POST',
            body: JSON.stringify({ sensor_id: sensorId, reading_value: readingValue })
        });
    }

    /**
     * Alert methods
     */
    async getAllAlerts() {
        return await this.makeRequest('/alerts.php');
    }

    async createAlert(alertData) {
        return await this.makeRequest('/alerts.php', {
            method: 'POST',
            body: JSON.stringify(alertData)
        });
    }

    async updateAlertStatus(alertId, status) {
        return await this.makeRequest('/alerts.php', {
            method: 'PUT',
            body: JSON.stringify({ alert_id: alertId, status })
        });
    }
}

// Initialize API instance
const api = new BantayGasAPI();

/**
 * Navigation and UI Management
 */
class NavigationManager {
    constructor() {
        this.currentPage = this.getCurrentPage();
        this.init();
    }

    getCurrentPage() {
        const path = window.location.pathname;
        const page = path.split('/').pop().replace('.html', '');
        return page;
    }

    init() {
        this.setActiveNavigation();
        this.setupNavigationHandlers();
        this.loadUserProfile();
    }

    setActiveNavigation() {
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.classList.remove('active');
            const href = item.getAttribute('href');
            if (href && href.includes(this.currentPage)) {
                item.classList.add('active');
            }
        });
    }

    setupNavigationHandlers() {
        // Handle navigation clicks
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                const href = item.getAttribute('href');
                if (href && href !== '#') {
                    window.location.href = href;
                }
            });
        });

        // Handle logout
        const logoutBtn = document.querySelector('.logout-btn');
        if (logoutBtn) {
            logoutBtn.addEventListener('click', (e) => {
                e.preventDefault();
                if (confirm('Are you sure you want to logout?')) {
                    api.logout();
                }
            });
        }
    }

    async loadUserProfile() {
        try {
            const response = await api.getUserProfile();
            if (response.status === 'success') {
                const user = response.data;
                this.updateUserInfo(user);
            }
        } catch (error) {
            console.error('Failed to load user profile:', error);
            // Fallback to session storage
            this.updateUserInfoFromSession();
        }
    }

    updateUserInfo(user) {
        const nameElement = document.querySelector('.profile .name');
        const roleElement = document.querySelector('.profile .role');
        
        if (nameElement) {
            nameElement.textContent = `${user.first_name} ${user.last_name}`;
        }
        
        if (roleElement) {
            roleElement.textContent = user.role.charAt(0).toUpperCase() + user.role.slice(1);
        }
    }

    updateUserInfoFromSession() {
        const nameElement = document.querySelector('.profile .name');
        const roleElement = document.querySelector('.profile .role');
        
        if (nameElement) {
            nameElement.textContent = sessionStorage.getItem('userName') || 'Josh Baguio';
        }
        
        if (roleElement) {
            roleElement.textContent = sessionStorage.getItem('userRole') || 'Admin';
        }
    }
}

/**
 * Monitoring Dashboard Manager
 */
class MonitoringManager {
    constructor() {
        this.sensors = [];
        this.init();
    }

    async init() {
        await this.loadMonitoringData();
        this.setupFloorSelector();
        this.startRealTimeUpdates();
    }

    async loadMonitoringData(floorPlanId = null) {
        try {
            const response = await api.getMonitoringData(floorPlanId);
            if (response.status === 'success') {
                this.sensors = response.data;
                this.renderSensors();
            }
        } catch (error) {
            console.error('Failed to load monitoring data:', error);
            this.showError('Failed to load monitoring data');
        }
    }

    renderSensors() {
        const floorPlanContainer = document.querySelector('.floor-plan');
        if (!floorPlanContainer) return;

        // Clear existing sensor indicators
        const existingSensors = floorPlanContainer.querySelectorAll('.sensor');
        existingSensors.forEach(sensor => sensor.remove());

        // Render new sensors
        this.sensors.forEach(sensor => {
            const sensorElement = this.createSensorElement(sensor);
            floorPlanContainer.appendChild(sensorElement);
        });
    }

    createSensorElement(sensor) {
        const sensorDiv = document.createElement('div');
        sensorDiv.className = `sensor sensor-${sensor.status}`;
        sensorDiv.style.top = sensor.position.top;
        sensorDiv.style.left = sensor.position.left;

        sensorDiv.innerHTML = `
            <div class="sensor-pin"></div>
            <div class="sensor-info">
                <div class="sensor-name">${sensor.sensor_name}</div>
                <div class="sensor-type">${sensor.sensor_type.toUpperCase()}</div>
                <div class="sensor-value">${sensor.current_reading} ${this.getUnit(sensor.sensor_type)}</div>
                <div class="sensor-status ${sensor.status}">Status: ${sensor.status.charAt(0).toUpperCase() + sensor.status.slice(1)}</div>
            </div>
        `;

        return sensorDiv;
    }

    getUnit(sensorType) {
        switch (sensorType) {
            case 'gas': return 'ppm';
            case 'smoke': return '%';
            case 'temperature': return '°C';
            case 'humidity': return '%';
            default: return '';
        }
    }

    setupFloorSelector() {
        const floorSelect = document.getElementById('floorSelect');
        if (floorSelect) {
            floorSelect.addEventListener('change', (e) => {
                const floorPlanId = e.target.value;
                this.loadMonitoringData(floorPlanId);
            });
        }
    }

    startRealTimeUpdates() {
        // Update sensor data every 30 seconds
        setInterval(() => {
            this.loadMonitoringData();
        }, 30000);
    }

    showError(message) {
        console.error(message);
        // You can implement a toast notification here
    }
}

/**
 * Initialize the application
 */
document.addEventListener('DOMContentLoaded', function() {
    // Check authentication
    if (sessionStorage.getItem('isLoggedIn') !== 'true') {
        const currentPage = window.location.pathname;
        const allowedPages = ['/login.html', '/sign-up.html', '/forgot-password.html', '/forgot-confirmation.html', '/enter-new-password.html', '/password-update-forgot.html', '/account-created.html', '/email-otp-verify.html'];
        
        if (!allowedPages.some(page => currentPage.includes(page))) {
            window.location.href = 'login.html';
            return;
        }
    }

    // Initialize navigation
    new NavigationManager();

    // Initialize monitoring if on monitoring page
    if (window.location.pathname.includes('monitoring.html')) {
        new MonitoringManager();
    }
});

// Export for use in other scripts
window.BantayGasAPI = BantayGasAPI;
window.NavigationManager = NavigationManager;
window.MonitoringManager = MonitoringManager;
