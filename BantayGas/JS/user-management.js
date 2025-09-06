// User Management Page JavaScript
class UserManagementManager {
    constructor() {
        this.users = [];
        this.filteredUsers = [];
        this.currentFilter = 'all';
        this.init();
    }

    async init() {
        try {
            await this.loadUsers();
            this.setupEventListeners();
            this.renderUsers();
        } catch (error) {
            console.error('Failed to initialize user management:', error);
            this.loadStaticUsers();
        }
    }

    async loadUsers() {
        try {
            const response = await api.getAllUsers();
            
            if (response.status === 'success') {
                this.users = response.data;
                this.filteredUsers = [...this.users];
                this.renderUsers();
            }
        } catch (error) {
            console.error('Error loading users:', error);
            throw error;
        }
    }

    loadStaticUsers() {
        // Fallback static data
        this.users = [
            {
                id: 1,
                email: 'baguiojosh@gmail.com',
                first_name: 'Josh',
                last_name: 'Baguio',
                role: 'admin',
                status: 'active',
                created_at: '2024-01-15T10:30:00Z'
            },
            {
                id: 2,
                email: 'john.doe@example.com',
                first_name: 'John',
                last_name: 'Doe',
                role: 'user',
                status: 'active',
                created_at: '2024-01-16T14:20:00Z'
            },
            {
                id: 3,
                email: 'jane.smith@example.com',
                first_name: 'Jane',
                last_name: 'Smith',
                role: 'user',
                status: 'inactive',
                created_at: '2024-01-17T09:15:00Z'
            }
        ];

        this.filteredUsers = [...this.users];
        this.renderUsers();
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
                this.searchUsers(e.target.value);
            });
        }

        // Add user button
        const addUserBtn = document.querySelector('.add-user-btn');
        if (addUserBtn) {
            addUserBtn.addEventListener('click', () => {
                this.showAddUserModal();
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

        // Add user form
        const addUserForm = document.querySelector('#addUserForm');
        if (addUserForm) {
            addUserForm.addEventListener('submit', (e) => {
                e.preventDefault();
                this.handleAddUser(e.target);
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

        // Filter users
        if (filter === 'all') {
            this.filteredUsers = [...this.users];
        } else if (filter === 'active') {
            this.filteredUsers = this.users.filter(user => user.status === 'active');
        } else if (filter === 'inactive') {
            this.filteredUsers = this.users.filter(user => user.status === 'inactive');
        } else if (filter === 'admin') {
            this.filteredUsers = this.users.filter(user => user.role === 'admin');
        } else if (filter === 'user') {
            this.filteredUsers = this.users.filter(user => user.role === 'user');
        }

        this.renderUsers();
    }

    searchUsers(query) {
        if (!query.trim()) {
            this.applyFilter(this.currentFilter);
            return;
        }

        const searchTerm = query.toLowerCase();
        this.filteredUsers = this.users.filter(user => 
            user.first_name.toLowerCase().includes(searchTerm) ||
            user.last_name.toLowerCase().includes(searchTerm) ||
            user.email.toLowerCase().includes(searchTerm)
        );

        this.renderUsers();
    }

    renderUsers() {
        const usersContainer = document.querySelector('.users-table tbody');
        if (!usersContainer) return;

        // Clear existing users
        usersContainer.innerHTML = '';

        if (this.filteredUsers.length === 0) {
            usersContainer.innerHTML = `
                <tr>
                    <td colspan="6" class="no-users">
                        No users found matching your criteria.
                    </td>
                </tr>
            `;
            return;
        }

        // Render user rows
        this.filteredUsers.forEach(user => {
            const userRow = this.createUserRow(user);
            usersContainer.appendChild(userRow);
        });
    }

    createUserRow(user) {
        const row = document.createElement('tr');
        row.className = `user-row ${user.status}`;
        
        const statusClass = user.status === 'active' ? 'active' : 'inactive';
        const roleClass = user.role === 'admin' ? 'admin' : 'user';

        row.innerHTML = `
            <td>
                <div class="user-info">
                    <div class="user-avatar">${user.first_name.charAt(0)}${user.last_name.charAt(0)}</div>
                    <div class="user-details">
                        <div class="user-name">${user.first_name} ${user.last_name}</div>
                        <div class="user-email">${user.email}</div>
                    </div>
                </div>
            </td>
            <td>
                <span class="role-badge ${roleClass}">${user.role}</span>
            </td>
            <td>
                <span class="status-badge ${statusClass}">${user.status}</span>
            </td>
            <td>${this.formatDate(user.created_at)}</td>
            <td>
                <div class="user-actions">
                    <button class="btn-edit" onclick="userManager.editUser(${user.id})" title="Edit User">
                        <span>✏️</span>
                    </button>
                    <button class="btn-delete" onclick="userManager.deleteUser(${user.id})" title="Delete User">
                        <span>🗑️</span>
                    </button>
                </div>
            </td>
        `;

        return row;
    }

    showAddUserModal() {
        const modal = document.querySelector('#addUserModal');
        if (modal) {
            modal.style.display = 'block';
        }
    }

    async handleAddUser(form) {
        const formData = new FormData(form);
        const userData = {
            email: formData.get('email'),
            password: formData.get('password'),
            first_name: formData.get('first_name'),
            last_name: formData.get('last_name'),
            role: formData.get('role'),
            status: formData.get('status')
        };

        try {
            const response = await api.createUser(userData);
            
            if (response.status === 'success') {
                this.showNotification('User added successfully!', 'success');
                this.closeModal('#addUserModal');
                form.reset();
                await this.loadUsers();
            } else {
                throw new Error(response.message || 'Failed to add user');
            }
        } catch (error) {
            console.error('Error adding user:', error);
            this.showNotification('Failed to add user. Please try again.', 'error');
        }
    }

    async editUser(userId) {
        try {
            const response = await api.getUserById(userId);
            
            if (response.status === 'success') {
                const user = response.data;
                this.showEditUserModal(user);
            }
        } catch (error) {
            console.error('Error loading user:', error);
            this.showNotification('Failed to load user data.', 'error');
        }
    }

    showEditUserModal(user) {
        // Create edit modal if it doesn't exist
        let modal = document.querySelector('#editUserModal');
        if (!modal) {
            modal = this.createEditModal();
            document.body.appendChild(modal);
        }

        // Populate form with user data
        const form = modal.querySelector('#editUserForm');
        form.querySelector('input[name="first_name"]').value = user.first_name;
        form.querySelector('input[name="last_name"]').value = user.last_name;
        form.querySelector('input[name="email"]').value = user.email;
        form.querySelector('select[name="role"]').value = user.role;
        form.querySelector('select[name="status"]').value = user.status;

        // Store user ID for update
        this.currentEditingUserId = user.id;

        modal.style.display = 'block';
    }

    createEditModal() {
        const modal = document.createElement('div');
        modal.id = 'editUserModal';
        modal.className = 'modal';
        modal.innerHTML = `
            <div class="modal-content">
                <div class="modal-header">
                    <h2>Edit User</h2>
                    <span class="modal-close">&times;</span>
                </div>
                <form id="editUserForm">
                    <div class="form-group">
                        <label for="edit_first_name">First Name:</label>
                        <input type="text" name="first_name" id="edit_first_name" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_last_name">Last Name:</label>
                        <input type="text" name="last_name" id="edit_last_name" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_email">Email:</label>
                        <input type="email" name="email" id="edit_email" required>
                    </div>
                    <div class="form-group">
                        <label for="edit_role">Role:</label>
                        <select name="role" id="edit_role" required>
                            <option value="user">User</option>
                            <option value="admin">Admin</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label for="edit_status">Status:</label>
                        <select name="status" id="edit_status" required>
                            <option value="active">Active</option>
                            <option value="inactive">Inactive</option>
                        </select>
                    </div>
                    <div class="form-actions">
                        <button type="button" class="btn-cancel" onclick="userManager.closeModal('#editUserModal')">
                            Cancel
                        </button>
                        <button type="submit" class="btn-save">Save Changes</button>
                    </div>
                </form>
            </div>
        `;

        // Add form submit handler
        const form = modal.querySelector('#editUserForm');
        form.addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleEditUser(e.target);
        });

        return modal;
    }

    async handleEditUser(form) {
        const formData = new FormData(form);
        const userData = {
            first_name: formData.get('first_name'),
            last_name: formData.get('last_name'),
            email: formData.get('email'),
            role: formData.get('role'),
            status: formData.get('status')
        };

        try {
            const response = await api.updateUser(this.currentEditingUserId, userData);
            
            if (response.status === 'success') {
                this.showNotification('User updated successfully!', 'success');
                this.closeModal('#editUserModal');
                await this.loadUsers();
            } else {
                throw new Error(response.message || 'Failed to update user');
            }
        } catch (error) {
            console.error('Error updating user:', error);
            this.showNotification('Failed to update user. Please try again.', 'error');
        }
    }

    async deleteUser(userId) {
        if (!confirm('Are you sure you want to delete this user? This action cannot be undone.')) {
            return;
        }

        try {
            const response = await api.deleteUser(userId);
            
            if (response.status === 'success') {
                this.showNotification('User deleted successfully!', 'success');
                await this.loadUsers();
            } else {
                throw new Error(response.message || 'Failed to delete user');
            }
        } catch (error) {
            console.error('Error deleting user:', error);
            this.showNotification('Failed to delete user. Please try again.', 'error');
        }
    }

    closeModal(modalId) {
        const modal = document.querySelector(modalId);
        if (modal) {
            modal.style.display = 'none';
        }
    }

    formatDate(dateString) {
        const date = new Date(dateString);
        return date.toLocaleDateString('en-US', {
            year: 'numeric',
            month: 'short',
            day: 'numeric'
        });
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

// Initialize user management when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    window.userManager = new UserManagementManager();
});
