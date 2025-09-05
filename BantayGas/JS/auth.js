// Authentication Check
document.addEventListener('DOMContentLoaded', function() {
    // Check if user is logged in
    if (sessionStorage.getItem('isLoggedIn') !== 'true') {
        // Only redirect if not already on login/signup pages
        const currentPage = window.location.pathname;
        const allowedPages = ['/login.html', '/sign-up.html', '/forgot-password.html', '/forgot-confirmation.html', '/enter-new-password.html', '/password-update-forgot.html', '/account-created.html', '/email-otp-verify.html'];
        
        if (!allowedPages.some(page => currentPage.includes(page))) {
            window.location.href = 'login.html';
            return;
        }
    }

    // Update user info in the sidebar if elements exist
    const userNameElement = document.querySelector('.profile .name');
    const userRoleElement = document.querySelector('.profile .role');
    
    if (userNameElement) {
        userNameElement.textContent = sessionStorage.getItem('userName') || 'Josh Baguio';
    }
    
    if (userRoleElement) {
        userRoleElement.textContent = sessionStorage.getItem('userRole') || 'Admin';
    }
    
    // Logout functionality
    const logoutBtn = document.querySelector('.logout-btn');
    if (logoutBtn) {
        logoutBtn.addEventListener('click', function(e) {
            e.preventDefault();
            if (confirm('Are you sure you want to logout?')) {
                sessionStorage.clear();
                window.location.href = 'login.html';
            }
        });
    }
});
