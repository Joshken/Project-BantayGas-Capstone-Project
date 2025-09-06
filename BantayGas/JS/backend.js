// Backend API Integration for BantayGas
class BantayGasAPI {
    constructor() {
        this.baseURL = 'PHP/api';
        this.isLoggedIn = sessionStorage.getItem('isLoggedIn') === 'true';
    }

    // Generic API call method
    async apiCall(endpoint, method = 'GET', data = null) {
        const url = `${this.baseURL}/${endpoint}`;
        const options = {
            method: method,
            headers: {
                'Content-Type': 'application/json',
            }
        };

        if (data && (method === 'POST' || method === 'PUT')) {
            options.body = JSON.stringify(data);
        }

        try {
            const response = await fetch(url, options);
            const result = await response.json();

            if (!response.ok) {
                throw new Error(result.message || 'API call failed');
            }

            return result;
        } catch (error) {
            console.error('API Error:', error);
            throw error;
        }
    }

    // Authentication methods
    async login(email, password) {
        try {
            const result = await this.apiCall('login.php', 'POST', {
                email: email,
                password: password
            });

            if (result.status === 'success') {
                // Store user data in session storage
                sessionStorage.setItem('isLoggedIn', 'true');
                sessionStorage.setItem('userName', result.data.user.first_name + ' ' + result.data.user.last_name);
                sessionStorage.setItem('userRole', result.data.user.role);
                sessionStorage.setItem('userId', result.data.user.id);
                sessionStorage.setItem('userEmail', result.data.user.email);
                
                this.isLoggedIn = true;
                return result;
            }
            return result;
        } catch (error) {
            throw error;
        }
    }

    async logout() {
        try {
            await this.apiCall('logout.php', 'POST');
            sessionStorage.clear();
            this.isLoggedIn = false;
            return { status: 'success' };
        } catch (error) {
            // Even if API call fails, clear local session
            sessionStorage.clear();
            this.isLoggedIn = false;
            throw error;
        }
    }

    // Navigation methods
    async getNavigationData() {
        try {
            return await this.apiCall('navigation.php');
        } catch (error) {
            throw error;
        }
    }

    async updateActivePage(page) {
        try {
            return await this.apiCall('navigation.php', 'POST', {
                action: 'update_active_page',
                page: page
            });
        } catch (error) {
            throw error;
        }
    }

    // User management methods
    async getAllUsers() {
        try {
            return await this.apiCall('users.php');
        } catch (error) {
            throw error;
        }
    }

    async getUserById(id) {
        try {
            return await this.apiCall(`users.php?id=${id}`);
        } catch (error) {
            throw error;
        }
    }

    async createUser(userData) {
        try {
            return await this.apiCall('users.php', 'POST', userData);
        } catch (error) {
            throw error;
        }
    }

    async updateUser(id, userData) {
        try {
            return await this.apiCall('users.php', 'PUT', { id, ...userData });
        } catch (error) {
            throw error;
        }
    }

    async deleteUser(id) {
        try {
            return await this.apiCall('users.php', 'DELETE', { id });
        } catch (error) {
            throw error;
        }
    }

    // Monitoring methods
    async getMonitoringData(floorPlanId = null) {
        try {
            const endpoint = floorPlanId ? `monitoring.php?floor_plan_id=${floorPlanId}` : 'monitoring.php';
            return await this.apiCall(endpoint);
        } catch (error) {
            throw error;
        }
    }

    async updateSensorReading(sensorId, readingValue) {
        try {
            return await this.apiCall('monitoring.php', 'POST', {
                sensor_id: sensorId,
                reading_value: readingValue
            });
        } catch (error) {
            throw error;
        }
    }

    // Sensor methods
    async getAllSensors() {
        try {
            return await this.apiCall('sensors.php');
        } catch (error) {
            throw error;
        }
    }

    async getSensorById(id) {
        try {
            return await this.apiCall(`sensors.php?id=${id}`);
        } catch (error) {
            throw error;
        }
    }

    async createSensor(sensorData) {
        try {
            return await this.apiCall('sensors.php', 'POST', sensorData);
        } catch (error) {
            throw error;
        }
    }

    async updateSensor(id, sensorData) {
        try {
            return await this.apiCall('sensors.php', 'PUT', { id, ...sensorData });
        } catch (error) {
            throw error;
        }
    }

    async deleteSensor(id) {
        try {
            return await this.apiCall('sensors.php', 'DELETE', { id });
        } catch (error) {
            throw error;
        }
    }

    // Alert methods
    async getAlerts(status = 'all', limit = 50) {
        try {
            return await this.apiCall(`alerts.php?status=${status}&limit=${limit}`);
        } catch (error) {
            throw error;
        }
    }

    async createAlert(alertData) {
        try {
            return await this.apiCall('alerts.php', 'POST', alertData);
        } catch (error) {
            throw error;
        }
    }

    async updateAlertStatus(id, status) {
        try {
            return await this.apiCall('alerts.php', 'PUT', { id, status });
        } catch (error) {
            throw error;
        }
    }
}

// Initialize global API instance
const api = new BantayGasAPI();

// Navigation Enhancement
class NavigationManager {
    constructor() {
        this.currentPage = this.getCurrentPage();
        this.init();
    }

    getCurrentPage() {
        const path = window.location.pathname;
        const filename = path.split('/').pop();
        return filename.replace('.html', '');
    }

    async init() {
        try {
            // Load navigation data from backend
            const navData = await api.getNavigationData();
            
            if (navData.status === 'success') {
                this.updateUserInfo(navData.data.user);
                this.updateNavigationMenu(navData.data.menu);
                this.setActiveNavigation();
            }
        } catch (error) {
            console.error('Failed to load navigation data:', error);
            // Fallback to static navigation
            this.setActiveNavigation();
        }

        // Add click handlers to navigation links
        this.addNavigationHandlers();
    }

    updateUserInfo(user) {
        const nameElement = document.querySelector('.profile .name');
        const roleElement = document.querySelector('.profile .role');
        
        if (nameElement) {
            nameElement.textContent = `${user.name}`;
        }
        
        if (roleElement) {
            roleElement.textContent = user.role;
        }
    }

    updateNavigationMenu(menu) {
        const navContainer = document.querySelector('.nav');
        if (!navContainer) return;

        // Clear existing navigation
        navContainer.innerHTML = '';

        // Add navigation items based on user permissions
        menu.forEach(item => {
            const navItem = document.createElement('a');
            navItem.className = 'nav-item';
            navItem.href = item.url;
            navItem.innerHTML = `
                <span class="icon">${item.icon}</span>
                <span>${item.title}</span>
            `;
            navContainer.appendChild(navItem);
        });
    }

    setActiveNavigation() {
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.classList.remove('active');
            if (item.href.includes(this.currentPage)) {
                item.classList.add('active');
            }
        });
    }

    addNavigationHandlers() {
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.addEventListener('click', async (e) => {
                const href = item.getAttribute('href');
                if (href && !href.startsWith('#')) {
                    // Update active page in backend
                    try {
                        await api.updateActivePage(this.currentPage);
                    } catch (error) {
                        console.error('Failed to update active page:', error);
                    }
                }
            });
        });
    }
}

// Enhanced Authentication Manager
class AuthManager {
    constructor() {
        this.init();
    }

    init() {
        this.checkAuthStatus();
        this.addLogoutHandler();
    }

    checkAuthStatus() {
        if (!api.isLoggedIn) {
            const currentPage = window.location.pathname;
            const allowedPages = ['/login.html', '/sign-up.html', '/forgot-password.html', 
                                '/forgot-confirmation.html', '/enter-new-password.html', 
                                '/password-update-forgot.html', '/account-created.html', 
                                '/email-otp-verify.html'];
            
            if (!allowedPages.some(page => currentPage.includes(page))) {
                window.location.href = 'login.html';
                return;
            }
        }
    }

    addLogoutHandler() {
        const logoutBtn = document.querySelector('.logout-btn');
        if (logoutBtn) {
            logoutBtn.addEventListener('click', async (e) => {
                e.preventDefault();
                if (confirm('Are you sure you want to logout?')) {
                    try {
                        await api.logout();
                        window.location.href = 'login.html';
                    } catch (error) {
                        console.error('Logout error:', error);
                        // Force redirect even if API call fails
                        window.location.href = 'login.html';
                    }
                }
            });
        }
    }
}

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    // Initialize authentication
    new AuthManager();
    
    // Initialize navigation (only if not on login/signup pages)
    const currentPage = window.location.pathname;
    const authPages = ['/login.html', '/sign-up.html', '/forgot-password.html', 
                      '/forgot-confirmation.html', '/enter-new-password.html', 
                      '/password-update-forgot.html', '/account-created.html', 
                      '/email-otp-verify.html'];
    
    if (!authPages.some(page => currentPage.includes(page))) {
        new NavigationManager();
    }
});

// Export for use in other scripts
window.BantayGasAPI = BantayGasAPI;
window.api = api;