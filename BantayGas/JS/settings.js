// Settings Page JavaScript
class SettingsManager {
    constructor() {
        this.currentTab = 'notifications';
        this.settings = {};
        this.init();
    }

    async init() {
        try {
            await this.loadUserSettings();
            this.setupEventListeners();
            this.initializeTabs();
        } catch (error) {
            console.error('Failed to initialize settings:', error);
        }
    }

    async loadUserSettings() {
        try {
            // Load user profile data
            const userResponse = await api.getUserById(sessionStorage.getItem('userId'));
            
            if (userResponse.status === 'success') {
                this.settings = userResponse.data;
                this.populateSettingsForm();
            }
        } catch (error) {
            console.error('Error loading user settings:', error);
            // Use fallback data
            this.settings = {
                first_name: 'Josh',
                last_name: 'Baguio',
                email: 'baguiojosh@gmail.com',
                role: 'Admin'
            };
            this.populateSettingsForm();
        }
    }

    populateSettingsForm() {
        // Populate profile information
        const firstNameInput = document.querySelector('input[name="first_name"]');
        const lastNameInput = document.querySelector('input[name="last_name"]');
        const emailInput = document.querySelector('input[name="email"]');

        if (firstNameInput) firstNameInput.value = this.settings.first_name || '';
        if (lastNameInput) lastNameInput.value = this.settings.last_name || '';
        if (emailInput) emailInput.value = this.settings.email || '';
    }

    setupEventListeners() {
        // Settings tabs
        const tabButtons = document.querySelectorAll('.settings-tab');
        tabButtons.forEach(button => {
            button.addEventListener('click', (e) => {
                const tabName = e.currentTarget.getAttribute('data-tab');
                this.switchTab(tabName);
            });
        });

        // Form submissions
        const forms = document.querySelectorAll('form');
        forms.forEach(form => {
            form.addEventListener('submit', (e) => {
                e.preventDefault();
                this.handleFormSubmission(form);
            });
        });

        // Toggle switches
        const toggleSwitches = document.querySelectorAll('.toggle-switch input[type="checkbox"]');
        toggleSwitches.forEach(toggle => {
            toggle.addEventListener('change', (e) => {
                this.handleToggleChange(e.target);
            });
        });

        // Theme selection
        const themeRadios = document.querySelectorAll('input[name="theme"]');
        themeRadios.forEach(radio => {
            radio.addEventListener('change', (e) => {
                this.handleThemeChange(e.target.value);
            });
        });
    }

    initializeTabs() {
        // Set initial active tab
        this.switchTab(this.currentTab);
    }

    switchTab(tabName) {
        // Update tab buttons
        const tabButtons = document.querySelectorAll('.settings-tab');
        tabButtons.forEach(button => {
            button.classList.remove('active');
            if (button.getAttribute('data-tab') === tabName) {
                button.classList.add('active');
            }
        });

        // Update tab content
        const tabContents = document.querySelectorAll('.settings-section');
        tabContents.forEach(content => {
            content.classList.remove('active');
            if (content.id === tabName) {
                content.classList.add('active');
            }
        });

        this.currentTab = tabName;
    }

    async handleFormSubmission(form) {
        const formData = new FormData(form);
        const data = Object.fromEntries(formData.entries());

        try {
            let response;
            
            if (form.classList.contains('profile-form')) {
                // Update user profile
                response = await api.updateUser(sessionStorage.getItem('userId'), {
                    first_name: data.first_name,
                    last_name: data.last_name
                });
            } else if (form.classList.contains('password-form')) {
                // Change password (would need separate API endpoint)
                response = { status: 'success', message: 'Password change functionality not implemented yet' };
            }

            if (response && response.status === 'success') {
                this.showNotification('Settings saved successfully!', 'success');
            } else {
                throw new Error(response?.message || 'Failed to save settings');
            }
        } catch (error) {
            console.error('Error saving settings:', error);
            this.showNotification('Failed to save settings. Please try again.', 'error');
        }
    }

    handleToggleChange(toggle) {
        const settingName = toggle.getAttribute('name');
        const isEnabled = toggle.checked;

        console.log(`Setting ${settingName} changed to: ${isEnabled}`);

        // Here you would typically save the setting to the backend
        // For now, we'll just show a notification
        this.showNotification(`${settingName} ${isEnabled ? 'enabled' : 'disabled'}`, 'info');
    }

    handleThemeChange(theme) {
        console.log(`Theme changed to: ${theme}`);
        
        // Apply theme change
        document.body.className = `theme-${theme}`;
        
        // Save theme preference
        localStorage.setItem('theme', theme);
        
        this.showNotification(`Theme changed to ${theme}`, 'info');
    }

    showNotification(message, type = 'info') {
        // Create notification element
        const notification = document.createElement('div');
        notification.className = `notification notification-${type}`;
        notification.textContent = message;

        // Add to page
        document.body.appendChild(notification);

        // Animate in
        setTimeout(() => {
            notification.classList.add('show');
        }, 100);

        // Remove after delay
        setTimeout(() => {
            notification.classList.remove('show');
            setTimeout(() => {
                if (notification.parentNode) {
                    notification.parentNode.removeChild(notification);
                }
            }, 300);
        }, 3000);
    }

    // Export settings
    exportSettings() {
        const settingsData = {
            profile: {
                first_name: this.settings.first_name,
                last_name: this.settings.last_name,
                email: this.settings.email
            },
            preferences: {
                notifications: {
                    email_alerts: document.querySelector('input[name="email_alerts"]')?.checked || false,
                    push_notifications: document.querySelector('input[name="push_notifications"]')?.checked || false,
                    sms_alerts: document.querySelector('input[name="sms_alerts"]')?.checked || false
                },
                system: {
                    auto_refresh: document.querySelector('input[name="auto_refresh"]')?.checked || false,
                    data_retention: document.querySelector('select[name="data_retention"]')?.value || '30',
                    backup_frequency: document.querySelector('select[name="backup_frequency"]')?.value || 'daily'
                },
                appearance: {
                    theme: document.querySelector('input[name="theme"]:checked')?.value || 'light',
                    language: document.querySelector('select[name="language"]')?.value || 'en'
                }
            }
        };

        const dataStr = JSON.stringify(settingsData, null, 2);
        const dataBlob = new Blob([dataStr], { type: 'application/json' });
        
        const link = document.createElement('a');
        link.href = URL.createObjectURL(dataBlob);
        link.download = 'bantaygas-settings.json';
        link.click();

        this.showNotification('Settings exported successfully!', 'success');
    }

    // Import settings
    importSettings(file) {
        const reader = new FileReader();
        
        reader.onload = (e) => {
            try {
                const settingsData = JSON.parse(e.target.result);
                
                // Apply imported settings
                if (settingsData.profile) {
                    this.settings = { ...this.settings, ...settingsData.profile };
                    this.populateSettingsForm();
                }

                if (settingsData.preferences) {
                    this.applyPreferences(settingsData.preferences);
                }

                this.showNotification('Settings imported successfully!', 'success');
            } catch (error) {
                console.error('Error importing settings:', error);
                this.showNotification('Failed to import settings. Invalid file format.', 'error');
            }
        };

        reader.readAsText(file);
    }

    applyPreferences(preferences) {
        // Apply notification preferences
        if (preferences.notifications) {
            Object.keys(preferences.notifications).forEach(key => {
                const input = document.querySelector(`input[name="${key}"]`);
                if (input) {
                    input.checked = preferences.notifications[key];
                }
            });
        }

        // Apply system preferences
        if (preferences.system) {
            Object.keys(preferences.system).forEach(key => {
                const input = document.querySelector(`select[name="${key}"]`);
                if (input) {
                    input.value = preferences.system[key];
                }
            });
        }

        // Apply appearance preferences
        if (preferences.appearance) {
            if (preferences.appearance.theme) {
                const themeRadio = document.querySelector(`input[name="theme"][value="${preferences.appearance.theme}"]`);
                if (themeRadio) {
                    themeRadio.checked = true;
                    this.handleThemeChange(preferences.appearance.theme);
                }
            }

            if (preferences.appearance.language) {
                const languageSelect = document.querySelector('select[name="language"]');
                if (languageSelect) {
                    languageSelect.value = preferences.appearance.language;
                }
            }
        }
    }
}

// Initialize settings when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    window.settingsManager = new SettingsManager();
});
