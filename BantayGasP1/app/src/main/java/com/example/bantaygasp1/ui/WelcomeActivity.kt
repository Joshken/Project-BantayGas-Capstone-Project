package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class WelcomeActivity : AppCompatActivity() {
    
    private lateinit var welcomeText: TextView
    private lateinit var loginButton: Button
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_welcome_screen)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
        
        // Get user info from intent
        val fullName = intent.getStringExtra("fullName") ?: ""
        val role = intent.getStringExtra("role") ?: ""
        
        welcomeText.text = "Welcome, $fullName!\nYour account has been created successfully.\nRole: $role"
    }
    
    private fun initializeViews() {
        welcomeText = findViewById(R.id.welcomeText)
        loginButton = findViewById(R.id.loginButton)
    }
    
    private fun setupClickListeners() {
        loginButton.setOnClickListener {
            // Navigate to login screen
            val intent = Intent(this, LoginActivity::class.java)
            intent.flags = Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_NEW_TASK
            startActivity(intent)
            finish()
        }
    }
}
