# BantayGas Login Website

A modern, responsive login website for BantayGas with a beautiful dark theme and smooth animations.

## Project Structure

```
BantayGas/
├── CSS/
│   ├── login.css              # Login page styles
│   └── forgot-password.css    # Forgot password page styles
├── HTML/
│   ├── login.html             # Main login page
│   └── forgot-password.html   # Forgot password page
├── Logos and BG/
│   ├── bantaygas-bg.jpg       # Background image
│   ├── bantaygas-logo.png     # Company logo
│   ├── google.png             # Google icon
│   └── facebook.png           # Facebook icon
├── Properties/
│   └── AssemblyInfo.cs        # Project assembly information
├── BantayGas.csproj           # Visual Studio project file
├── BantayGas.sln              # Visual Studio solution file
├── Web.config                 # Web configuration
└── README.md                  # This file
```

## Features

- **Modern Design**: Dark blue theme with smoky background patterns
- **Responsive Layout**: Works on desktop, tablet, and mobile devices
- **Interactive Elements**: Hover effects and smooth animations
- **Social Login**: Google and Facebook login options
- **Password Recovery**: Dedicated forgot password page
- **Visual Studio Integration**: Full project structure for VS development

## Pages

### Login Page (`HTML/login.html`)
- BantayGas branding and logo
- Email and password input fields
- "Forgot your password?" link
- Social media login options (Google, Facebook)
- "Sign Up" link for new users

### Forgot Password Page (`HTML/forgot-password.html`)
- Back navigation to login page
- Lock icon with question mark
- Email input for password recovery
- "Next" button to proceed

## Setup Instructions

### Visual Studio
1. Open `BantayGas.sln` in Visual Studio
2. Set `HTML/login.html` as the startup page
3. Press F5 to run the project
4. The website will open in your default browser

### Manual Setup
1. Open `HTML/login.html` in any web browser
2. Navigate between pages using the links
3. All styling and images will load automatically

## Technical Details

- **HTML5**: Semantic markup with proper accessibility
- **CSS3**: Modern styling with gradients, animations, and responsive design
- **Images**: Optimized PNG and JPG files for web use
- **Browser Support**: Works in all modern browsers (Chrome, Firefox, Safari, Edge)

## Customization

### Colors
The main color scheme uses:
- Primary Blue: `#4a90e2`
- Dark Blue: `#1a1a2e`, `#16213e`
- Gold Border: `#d4af37`
- Text Colors: `#e0e0e0`, `#888`

### Adding New Pages
1. Create new HTML file in `HTML/` folder
2. Create corresponding CSS file in `CSS/` folder
3. Update project files to include new files
4. Link CSS file in HTML head section

## Development Notes

- All CSS files are organized by page for better maintenance
- Images are stored in `Logos and BG/` folder
- Project uses Visual Studio 2017+ compatibility
- Web.config includes proper MIME type mappings

## License

Copyright © 2024 BantayGas. All rights reserved.
