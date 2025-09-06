// Sensors Page JavaScript
class SensorsManager {
    constructor() {
        this.sensors = [];
        this.filteredSensors = [];
        this.currentFilter = 'all';
        this.init();
    }

    async init() {
        try {
            await this.loadSensors();
            this.setupEventListeners();
            this.renderSensors();
        } catch (error) {
            console.error('Failed to initialize sensors:', error);
            this.loadStaticSensors();
        }
    }

    async loadSensors() {
        try {
            const response = await api.getAllSensors();
            
            if (response.status === 'success') {
                this.sensors = response.data;
                this.filteredSensors = [...this.sensors];
                this.renderSensors();
            }
        } catch (error) {
            console.error('Error loading sensors:', error);
            throw error;
        }
    }

    loadStaticSensors() {
        // Fallback static data
        this.sensors = [
            {
                id: 1,
                sensor_name: 'Sensor_001',
                sensor_type: 'CO',
                location: 'Kitchen',
                status: 'active',
                last_reading: 88,
                threshold_value: 50,
                created_at: '2024-01-15T10:30:00Z'
            },
            {
                id: 2,
                sensor_name: 'Sensor_002',
                sensor_type: 'CO',
                location: 'Living Room',
                status: 'active',
                last_reading: 25,
                threshold_value: 50,
                created_at: '2024-01-15T10:30:00Z'
            },
            {
                id: 3,
                sensor_name: 'Sensor_003',
                sensor_type: 'CO',
                location: 'Bedroom',
                status: 'inactive',
                last_reading: 0,
                threshold_value: 50,
                created_at: '2024-01-15T10:30:00Z'
            }
        ];

        this.filteredSensors = [...this.sensors];
        this.renderSensors();
    }

    setupEventListeners() {
        // Filter buttons
        const filterButtons = document.querySelectorAll('.filter-btn');
        filterButtons.forEach(button => {
            button.addEventListener('click', (e) => {
                const filter = e.target.getAttribute('data-filter');
                this.applyFilter(filter);
            });
        });

        // Search input
        const searchInput = document.querySelector('.search-input');
        if (searchInput) {
            searchInput.addEventListener('input', (e) => {
                this.searchSensors(e.target.value);
            });
        }

        // Add sensor button
        const addSensorBtn = document.querySelector('.add-sensor-btn');
        if (addSensorBtn) {
            addSensorBtn.addEventListener('click', () => {
                this.showAddSensorModal();
            });
        }

        // Modal close buttons
        const modalCloseBtns = document.querySelectorAll('.modal-close');
        modalCloseBtns.forEach(btn => {
            btn.addEventListener('click', (e) => {
                const modal = e.target.closest('.modal');
                if (modal) {
                    modal.style.display = 'none';
                }
            });
        });

        // Add sensor form
        const addSensorForm = document.querySelector('#addSensorForm');
        if (addSensorForm) {
            addSensorForm.addEventListener('submit', (e) => {
                e.preventDefault();
                this.handleAddSensor(e.target);
            });
        }
    }

    applyFilter(filter) {
        this.currentFilter = filter;

        // Update filter buttons
        const filterButtons = document.querySelectorAll('.filter-btn');
        filterButtons.forEach(button => {
            button.classList.remove('active');
            if (button.getAttribute('data-filter') === filter) {
                button.classList.add('active');
            }
        });

        // Filter sensors
        if (filter === 'all') {
            this.filteredSensors = [...this.sensors];
        } else if (filter === 'active') {
            this.filteredSensors = this.sensors.filter(sensor => sensor.status === 'active');
        } else if (filter === 'inactive') {
            this.filteredSensors = this.sensors.filter(sensor => sensor.status === 'inactive');
        } else if (filter === 'critical') {
            this.filteredSensors = this.sensors.filter(sensor => 
                sensor.status === 'active' && sensor.last_reading > sensor.threshold_value
            );
        }

        this.renderSensors();
    }

    searchSensors(query) {
        if (!query.trim()) {
            this.applyFilter(this.currentFilter);
            return;
        }

        const searchTerm = query.toLowerCase();
        this.filteredSensors = this.sensors.filter(sensor => 
            sensor.sensor_name.toLowerCase().includes(searchTerm) ||
            sensor.location.toLowerCase().includes(searchTerm) ||
            sensor.sensor_type.toLowerCase().includes(searchTerm)
        );

        this.renderSensors();
    }

    renderSensors() {
        const sensorsContainer = document.querySelector('.sensors-grid');
        if (!sensorsContainer) return;

        // Clear existing sensors
        sensorsContainer.innerHTML = '';

        if (this.filteredSensors.length === 0) {
            sensorsContainer.innerHTML = `
                <div class="no-sensors">
                    <p>No sensors found matching your criteria.</p>
                </div>
            `;
            return;
        }

        // Render sensor cards
        this.filteredSensors.forEach(sensor => {
            const sensorCard = this.createSensorCard(sensor);
            sensorsContainer.appendChild(sensorCard);
        });
    }

    createSensorCard(sensor) {
        const card = document.createElement('div');
        card.className = `sensor-card ${sensor.status}`;
        
        const statusClass = sensor.status === 'active' ? 
            (sensor.last_reading > sensor.threshold_value ? 'critical' : 'normal') : 
            'inactive';

        card.innerHTML = `
            <div class="sensor-header">
                <h3 class="sensor-name">${sensor.sensor_name}</h3>
                <span class="sensor-status ${statusClass}">${statusClass}</span>
            </div>
            <div class="sensor-details">
                <div class="detail-item">
                    <span class="label">Type:</span>
                    <span class="value">${sensor.sensor_type}</span>
                </div>
                <div class="detail-item">
                    <span class="label">Location:</span>
                    <span class="value">${sensor.location}</span>
                </div>
                <div class="detail-item">
                    <span class="label">Last Reading:</span>
                    <span class="value">${sensor.last_reading} ppm</span>
                </div>
                <div class="detail-item">
                    <span class="label">Threshold:</span>
                    <span class="value">${sensor.threshold_value} ppm</span>
                </div>
            </div>
            <div class="sensor-actions">
                <button class="btn-edit" onclick="sensorsManager.editSensor(${sensor.id})">
                    Edit
                </button>
                <button class="btn-delete" onclick="sensorsManager.deleteSensor(${sensor.id})">
                    Delete
                </button>
            </div>
        `;

        return card;
    }

    showAddSensorModal() {
        const modal = document.querySelector('#addSensorModal');
        if (modal) {
            modal.style.display = 'block';
        }
    }

    async handleAddSensor(form) {
        const formData = new FormData(form);
        const sensorData = {
            sensor_name: formData.get('sensor_name'),
            sensor_type: formData.get('sensor_type'),
            location: formData.get('location'),
            threshold_value: parseFloat(formData.get('threshold_value')),
            status: formData.get('status')
        };

        try {
            const response = await api.createSensor(sensorData);
            
            if (response.status === 'success') {
                this.showNotification('Sensor added successfully!', 'success');
                this.closeModal('#addSensorModal');
                await this.loadSensors();
            } else {
                throw new Error(response.message || 'Failed to add sensor');
            }
        } catch (error) {
            console.error('Error adding sensor:', error);
            this.showNotification('Failed to add sensor. Please try again.', 'error');
        }
    }

    async editSensor(sensorId) {
        try {
            const response = await api.getSensorById(sensorId);
            
            if (response.status === 'success') {
                const sensor = response.data;
                this.showEditSensorModal(sensor);
            }
        } catch (error) {
            console.error('Error loading sensor:', error);
            this.showNotification('Failed to load sensor data.', 'error');
        }
    }

    showEditSensorModal(sensor) {
        // Create edit modal if it doesn't exist
        let modal = document.querySelector('#editSensorModal');
        if (!modal) {
            modal = this.createEditModal();
            document.body.appendChild(modal);
        }

        // Populate form with sensor data
        const form = modal.querySelector('#editSensorForm');
        form.querySelector('input[name="sensor_name"]').value = sensor.sensor_name;
        form.querySelector('select[name="sensor_type"]').value = sensor.sensor_type;
        form.querySelector('input[name="location"]').value = sensor.location;
        form.querySelector('input[name="threshold_value"]').value = sensor.threshold_value;
        form.querySelector('select[name="status"]').value = sensor.status;

        modal.style.display = 'block';
    }

    createEditModal() {
        const modal = document.createElement('div');
        modal.id = 'editSensorModal';
        modal.className = 'modal';
        modal.innerHTML = `
            <div class="modal-content">
                <div class="modal-header">
                    <h2>Edit Sensor</h2>
                    <span class="modal-close">&times;</span>
                </div>
                <form id="editSensorForm">
                    <div class="form-group">
                        <label for="edit_sensor_name">Sensor Name:</label>
                        <input type="text" name="sensor_name" id="edit_sensor_name" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_sensor_type">Sensor Type:</label>
                        <select name="sensor_type" id="edit_sensor_type" required>
                            <option value="CO">CO</option>
                            <option value="CO2">CO2</option>
                            <option value="Methane">Methane</option>
                            <option value="Temperature">Temperature</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label for="edit_location">Location:</label>
                        <input type="text" name="location" id="edit_location" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_threshold_value">Threshold Value:</label>
                        <input type="number" name="threshold_value" id="edit_threshold_value" step="0.1" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_status">Status:</label>
                        <select name="status" id="edit_status" required>
                            <option value="active">Active</option>
                            <option value="inactive">Inactive</option>
                        </select>
                    </div>
                    <div class="form-actions">
                        <button type="button" class="btn-cancel" onclick="sensorsManager.closeModal('#editSensorModal')">
                            Cancel
                        </button>
                        <button type="submit" class="btn-save">Save Changes</button>
                    </div>
                </form>
            </div>
        `;

        // Add form submit handler
        const form = modal.querySelector('#editSensorForm');
        form.addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleEditSensor(e.target);
        });

        return modal;
    }

    async handleEditSensor(form) {
        const formData = new FormData(form);
        const sensorData = {
            sensor_name: formData.get('sensor_name'),
            sensor_type: formData.get('sensor_type'),
            location: formData.get('location'),
            threshold_value: parseFloat(formData.get('threshold_value')),
            status: formData.get('status')
        };

        try {
            // Get sensor ID from the form (you might need to store this when opening the modal)
            const sensorId = this.currentEditingSensorId;
            const response = await api.updateSensor(sensorId, sensorData);
            
            if (response.status === 'success') {
                this.showNotification('Sensor updated successfully!', 'success');
                this.closeModal('#editSensorModal');
                await this.loadSensors();
            } else {
                throw new Error(response.message || 'Failed to update sensor');
            }
        } catch (error) {
            console.error('Error updating sensor:', error);
            this.showNotification('Failed to update sensor. Please try again.', 'error');
        }
    }

    async deleteSensor(sensorId) {
        if (!confirm('Are you sure you want to delete this sensor?')) {
            return;
        }

        try {
            const response = await api.deleteSensor(sensorId);
            
            if (response.status === 'success') {
                this.showNotification('Sensor deleted successfully!', 'success');
                await this.loadSensors();
            } else {
                throw new Error(response.message || 'Failed to delete sensor');
            }
        } catch (error) {
            console.error('Error deleting sensor:', error);
            this.showNotification('Failed to delete sensor. Please try again.', 'error');
        }
    }

    closeModal(modalId) {
        const modal = document.querySelector(modalId);
        if (modal) {
            modal.style.display = 'none';
        }
    }

    showNotification(message, type = 'info') {
        const notification = document.createElement('div');
        notification.className = `notification notification-${type}`;
        notification.textContent = message;

        document.body.appendChild(notification);

        setTimeout(() => {
            notification.classList.add('show');
        }, 100);

        setTimeout(() => {
            notification.classList.remove('show');
            setTimeout(() => {
                if (notification.parentNode) {
                    notification.parentNode.removeChild(notification);
                }
            }, 300);
        }, 3000);
    }
}

// Initialize sensors when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    window.sensorsManager = new SensorsManager();
});
