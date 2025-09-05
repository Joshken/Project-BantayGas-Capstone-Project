package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.ImageButton
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class LoginActivity : AppCompatActivity() {
    
    private lateinit var usernameEditText: EditText
    private lateinit var passwordEditText: EditText
    private lateinit var passwordToggleButton: ImageButton
    private lateinit var loginButton: Button
    private lateinit var forgotPasswordText: TextView
    private lateinit var signUpText: TextView
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_login)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
    }
    
    private fun initializeViews() {
        usernameEditText = findViewById(R.id.usernameEditText)
        passwordEditText = findViewById(R.id.passwordEditText)
        passwordToggleButton = findViewById(R.id.passwordToggleButton)
        loginButton = findViewById(R.id.loginButton)
        forgotPasswordText = findViewById(R.id.forgotPasswordTextView)
        signUpText = findViewById(R.id.signUpText)
    }
    
    private fun setupClickListeners() {
        loginButton.setOnClickListener {
            performLogin()
        }
        
        forgotPasswordText.setOnClickListener {
            val intent = Intent(this, ForgotPasswordActivity::class.java)
            startActivity(intent)
        }
        
        signUpText.setOnClickListener {
            val intent = Intent(this, SignUpActivity::class.java)
            startActivity(intent)
        }
        
        passwordToggleButton.setOnClickListener {
            togglePasswordVisibility()
        }
    }
    
    private fun performLogin() {
        val username = usernameEditText.text.toString().trim()
        val password = passwordEditText.text.toString().trim()
        
        // Check if fields are empty
        if (username.isEmpty()) {
            usernameEditText.error = "Username is required"
            return
        }
        
        if (password.isEmpty()) {
            passwordEditText.error = "Password is required"
            return
        }
        
        // Check stored user credentials first
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        val storedUsername = sharedPrefs.getString("username", "")
        val storedPassword = sharedPrefs.getString("password", "")
        
        // Check if credentials match stored user data
        if (username == storedUsername && password == storedPassword) {
            Toast.makeText(this, "Login successful!", Toast.LENGTH_SHORT).show()
            
            // Store current user info for profile access
            val editor = sharedPrefs.edit()
            editor.putBoolean("isLoggedIn", true)
            editor.putString("currentUser", username)
            editor.apply()
            
            // Always navigate to MonitoringActivity - it will handle content loading based on floor plan status
            val intent = Intent(this, MonitoringActivity::class.java)
            startActivity(intent)
            finish() // Close login activity
        }
        // Fallback to hardcoded credentials for testing
        else if (username == "123" && password == "123") {
            Toast.makeText(this, "Login successful! (Demo Mode)", Toast.LENGTH_SHORT).show()
            
            // Store demo user info
            val editor = sharedPrefs.edit()
            editor.putBoolean("isLoggedIn", true)
            editor.putString("currentUser", "demo")
            editor.putString("firstName", "Demo")
            editor.putString("lastName", "User")
            editor.putString("email", "demo@example.com")
            editor.putString("phone", "+63 912 345 6789")
            editor.putString("role", "USER")
            editor.putString("department", "Demo Department")
            editor.apply()
            
            // Always navigate to MonitoringActivity - it will handle content loading based on floor plan status
            val intent = Intent(this, MonitoringActivity::class.java)
            startActivity(intent)
            finish() // Close login activity
        } else {
            Toast.makeText(this, "Invalid username or password", Toast.LENGTH_SHORT).show()
        }
    }
    
    private fun togglePasswordVisibility() {
        if (passwordEditText.inputType == android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_PASSWORD) {
            // Show password
            passwordEditText.inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
            passwordToggleButton.setImageResource(R.drawable.ic_visibility)
        } else {
            // Hide password
            passwordEditText.inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_PASSWORD
            passwordToggleButton.setImageResource(R.drawable.ic_visibility_off)
        }
        // Move cursor to end of text
        passwordEditText.setSelection(passwordEditText.text.length)
    }
}


