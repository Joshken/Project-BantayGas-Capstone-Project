// Alerts Page JavaScript
class AlertsManager {
    constructor() {
        this.alerts = [];
        this.filteredAlerts = [];
        this.currentFilter = 'all';
        this.init();
    }

    async init() {
        try {
            await this.loadAlerts();
            this.setupEventListeners();
            this.renderAlerts();
        } catch (error) {
            console.error('Failed to initialize alerts:', error);
            this.loadStaticAlerts();
        }
    }

    async loadAlerts() {
        try {
            const response = await api.getAlerts('all', 100);
            
            if (response.status === 'success') {
                this.alerts = response.data.alerts;
                this.filteredAlerts = [...this.alerts];
                this.updateStatistics(response.data.statistics);
                this.renderAlerts();
            }
        } catch (error) {
            console.error('Error loading alerts:', error);
            throw error;
        }
    }

    loadStaticAlerts() {
        // Fallback static data
        this.alerts = [
            {
                id: 1,
                sensor_name: 'Sensor_001',
                location: 'Kitchen',
                sensor_type: 'CO',
                alert_type: 'critical',
                message: 'CO level exceeds threshold (88 ppm > 50 ppm)',
                status: 'active',
                created_at: '2024-01-20T10:30:00Z',
                threshold_value: 50,
                last_reading: 88
            },
            {
                id: 2,
                sensor_name: 'Sensor_002',
                location: 'Living Room',
                sensor_type: 'CO',
                alert_type: 'warning',
                message: 'CO level approaching threshold (45 ppm / 50 ppm)',
                status: 'acknowledged',
                created_at: '2024-01-20T09:15:00Z',
                threshold_value: 50,
                last_reading: 45
            },
            {
                id: 3,
                sensor_name: 'Sensor_003',
                location: 'Bedroom',
                sensor_type: 'CO',
                alert_type: 'critical',
                message: 'CO level exceeds threshold (75 ppm > 50 ppm)',
                status: 'resolved',
                created_at: '2024-01-19T16:45:00Z',
                threshold_value: 50,
                last_reading: 75
            }
        ];

        this.filteredAlerts = [...this.alerts];
        this.updateStatistics({
            total: this.alerts.length,
            active: this.alerts.filter(a => a.status === 'active').length,
            acknowledged: this.alerts.filter(a => a.status === 'acknowledged').length,
            resolved: this.alerts.filter(a => a.status === 'resolved').length,
            critical: this.alerts.filter(a => a.alert_type === 'critical').length,
            warning: this.alerts.filter(a => a.alert_type === 'warning').length
        });
        this.renderAlerts();
    }

    updateStatistics(stats) {
        // Update summary cards
        const totalEl = document.querySelector('.summary-card.total .summary-count');
        const activeEl = document.querySelector('.summary-card.active .summary-count');
        const acknowledgedEl = document.querySelector('.summary-card.acknowledged .summary-count');
        const resolvedEl = document.querySelector('.summary-card.resolved .summary-count');
        const criticalEl = document.querySelector('.summary-card.critical .summary-count');
        const warningEl = document.querySelector('.summary-card.warning .summary-count');

        if (totalEl) totalEl.textContent = stats.total || 0;
        if (activeEl) activeEl.textContent = stats.active || 0;
        if (acknowledgedEl) acknowledgedEl.textContent = stats.acknowledged || 0;
        if (resolvedEl) resolvedEl.textContent = stats.resolved || 0;
        if (criticalEl) criticalEl.textContent = stats.critical || 0;
        if (warningEl) warningEl.textContent = stats.warning || 0;
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
                this.searchAlerts(e.target.value);
            });
        }

        // Bulk actions
        const selectAllCheckbox = document.querySelector('#selectAll');
        if (selectAllCheckbox) {
            selectAllCheckbox.addEventListener('change', (e) => {
                this.toggleSelectAll(e.target.checked);
            });
        }

        const bulkActionBtn = document.querySelector('.bulk-action-btn');
        if (bulkActionBtn) {
            bulkActionBtn.addEventListener('click', () => {
                this.showBulkActionMenu();
            });
        }

        // Refresh button
        const refreshBtn = document.querySelector('.refresh-btn');
        if (refreshBtn) {
            refreshBtn.addEventListener('click', () => {
                this.loadAlerts();
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

        // Filter alerts
        if (filter === 'all') {
            this.filteredAlerts = [...this.alerts];
        } else if (filter === 'active') {
            this.filteredAlerts = this.alerts.filter(alert => alert.status === 'active');
        } else if (filter === 'acknowledged') {
            this.filteredAlerts = this.alerts.filter(alert => alert.status === 'acknowledged');
        } else if (filter === 'resolved') {
            this.filteredAlerts = this.alerts.filter(alert => alert.status === 'resolved');
        } else if (filter === 'critical') {
            this.filteredAlerts = this.alerts.filter(alert => alert.alert_type === 'critical');
        } else if (filter === 'warning') {
            this.filteredAlerts = this.alerts.filter(alert => alert.alert_type === 'warning');
        }

        this.renderAlerts();
    }

    searchAlerts(query) {
        if (!query.trim()) {
            this.applyFilter(this.currentFilter);
            return;
        }

        const searchTerm = query.toLowerCase();
        this.filteredAlerts = this.alerts.filter(alert => 
            alert.sensor_name.toLowerCase().includes(searchTerm) ||
            alert.location.toLowerCase().includes(searchTerm) ||
            alert.message.toLowerCase().includes(searchTerm)
        );

        this.renderAlerts();
    }

    renderAlerts() {
        const alertsContainer = document.querySelector('.alerts-list');
        if (!alertsContainer) return;

        // Clear existing alerts
        alertsContainer.innerHTML = '';

        if (this.filteredAlerts.length === 0) {
            alertsContainer.innerHTML = `
                <div class="no-alerts">
                    <p>No alerts found matching your criteria.</p>
                </div>
            `;
            return;
        }

        // Render alert cards
        this.filteredAlerts.forEach(alert => {
            const alertCard = this.createAlertCard(alert);
            alertsContainer.appendChild(alertCard);
        });
    }

    createAlertCard(alert) {
        const card = document.createElement('div');
        card.className = `alert-card ${alert.alert_type} ${alert.status}`;
        
        const timeAgo = this.getTimeAgo(alert.created_at);
        const statusIcon = this.getStatusIcon(alert.status);
        const alertIcon = this.getAlertIcon(alert.alert_type);

        card.innerHTML = `
            <div class="alert-checkbox">
                <input type="checkbox" class="alert-select" value="${alert.id}">
            </div>
            <div class="alert-icon">
                ${alertIcon}
            </div>
            <div class="alert-content">
                <div class="alert-header">
                    <h3 class="alert-title">${alert.sensor_name}</h3>
                    <span class="alert-type ${alert.alert_type}">${alert.alert_type}</span>
                </div>
                <div class="alert-location">📍 ${alert.location}</div>
                <div class="alert-message">${alert.message}</div>
                <div class="alert-details">
                    <span class="alert-time">🕒 ${timeAgo}</span>
                    <span class="alert-status ${alert.status}">${statusIcon} ${alert.status}</span>
                </div>
            </div>
            <div class="alert-actions">
                ${this.getActionButtons(alert)}
            </div>
        `;

        return card;
    }

    getStatusIcon(status) {
        switch (status) {
            case 'active': return '🔴';
            case 'acknowledged': return '🟡';
            case 'resolved': return '🟢';
            default: return '⚪';
        }
    }

    getAlertIcon(alertType) {
        switch (alertType) {
            case 'critical': return '🚨';
            case 'warning': return '⚠️';
            case 'info': return 'ℹ️';
            default: return '📢';
        }
    }

    getActionButtons(alert) {
        let buttons = '';

        if (alert.status === 'active') {
            buttons += `<button class="btn-acknowledge" onclick="alertsManager.acknowledgeAlert(${alert.id})">Acknowledge</button>`;
        }

        if (alert.status === 'acknowledged') {
            buttons += `<button class="btn-resolve" onclick="alertsManager.resolveAlert(${alert.id})">Resolve</button>`;
        }

        if (alert.status === 'resolved') {
            buttons += `<button class="btn-reopen" onclick="alertsManager.reopenAlert(${alert.id})">Reopen</button>`;
        }

        return buttons;
    }

    async acknowledgeAlert(alertId) {
        try {
            const response = await api.updateAlertStatus(alertId, 'acknowledged');
            
            if (response.status === 'success') {
                this.showNotification('Alert acknowledged successfully!', 'success');
                await this.loadAlerts();
            } else {
                throw new Error(response.message || 'Failed to acknowledge alert');
            }
        } catch (error) {
            console.error('Error acknowledging alert:', error);
            this.showNotification('Failed to acknowledge alert. Please try again.', 'error');
        }
    }

    async resolveAlert(alertId) {
        try {
            const response = await api.updateAlertStatus(alertId, 'resolved');
            
            if (response.status === 'success') {
                this.showNotification('Alert resolved successfully!', 'success');
                await this.loadAlerts();
            } else {
                throw new Error(response.message || 'Failed to resolve alert');
            }
        } catch (error) {
            console.error('Error resolving alert:', error);
            this.showNotification('Failed to resolve alert. Please try again.', 'error');
        }
    }

    async reopenAlert(alertId) {
        try {
            const response = await api.updateAlertStatus(alertId, 'active');
            
            if (response.status === 'success') {
                this.showNotification('Alert reopened successfully!', 'success');
                await this.loadAlerts();
            } else {
                throw new Error(response.message || 'Failed to reopen alert');
            }
        } catch (error) {
            console.error('Error reopening alert:', error);
            this.showNotification('Failed to reopen alert. Please try again.', 'error');
        }
    }

    toggleSelectAll(checked) {
        const checkboxes = document.querySelectorAll('.alert-select');
        checkboxes.forEach(checkbox => {
            checkbox.checked = checked;
        });
    }

    showBulkActionMenu() {
        const selectedAlerts = this.getSelectedAlerts();
        
        if (selectedAlerts.length === 0) {
            this.showNotification('Please select alerts to perform bulk actions.', 'warning');
            return;
        }

        // Create bulk action menu
        const menu = document.createElement('div');
        menu.className = 'bulk-action-menu';
        menu.innerHTML = `
            <div class="bulk-action-item" onclick="alertsManager.bulkAcknowledge()">
                Acknowledge Selected (${selectedAlerts.length})
            </div>
            <div class="bulk-action-item" onclick="alertsManager.bulkResolve()">
                Resolve Selected (${selectedAlerts.length})
            </div>
            <div class="bulk-action-item" onclick="alertsManager.bulkDelete()">
                Delete Selected (${selectedAlerts.length})
            </div>
        `;

        document.body.appendChild(menu);

        // Remove menu when clicking outside
        setTimeout(() => {
            document.addEventListener('click', function removeMenu() {
                menu.remove();
                document.removeEventListener('click', removeMenu);
            });
        }, 100);
    }

    getSelectedAlerts() {
        const checkboxes = document.querySelectorAll('.alert-select:checked');
        return Array.from(checkboxes).map(cb => parseInt(cb.value));
    }

    async bulkAcknowledge() {
        const selectedIds = this.getSelectedAlerts();
        
        try {
            for (const id of selectedIds) {
                await api.updateAlertStatus(id, 'acknowledged');
            }
            
            this.showNotification(`${selectedIds.length} alerts acknowledged successfully!`, 'success');
            await this.loadAlerts();
        } catch (error) {
            console.error('Error in bulk acknowledge:', error);
            this.showNotification('Failed to acknowledge some alerts.', 'error');
        }
    }

    async bulkResolve() {
        const selectedIds = this.getSelectedAlerts();
        
        try {
            for (const id of selectedIds) {
                await api.updateAlertStatus(id, 'resolved');
            }
            
            this.showNotification(`${selectedIds.length} alerts resolved successfully!`, 'success');
            await this.loadAlerts();
        } catch (error) {
            console.error('Error in bulk resolve:', error);
            this.showNotification('Failed to resolve some alerts.', 'error');
        }
    }

    async bulkDelete() {
        const selectedIds = this.getSelectedAlerts();
        
        if (!confirm(`Are you sure you want to delete ${selectedIds.length} alerts? This action cannot be undone.`)) {
            return;
        }

        try {
            // Note: This would require a delete endpoint in the API
            this.showNotification('Bulk delete functionality not implemented yet.', 'info');
        } catch (error) {
            console.error('Error in bulk delete:', error);
            this.showNotification('Failed to delete some alerts.', 'error');
        }
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

// Initialize alerts when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    window.alertsManager = new AlertsManager();
});
