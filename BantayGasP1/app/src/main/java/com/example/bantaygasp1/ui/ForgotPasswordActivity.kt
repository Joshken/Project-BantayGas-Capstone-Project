package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.ImageView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class ForgotPasswordActivity : AppCompatActivity() {
    
    private lateinit var emailEditText: EditText
    private lateinit var sendButton: Button
    private lateinit var backButton: Button
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_forgot_password)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
    }
    
    private fun initializeViews() {
        emailEditText = findViewById(R.id.emailEditText)
        sendButton = findViewById(R.id.sendButton)
        backButton = findViewById(R.id.backButton)
    }
    
    private fun setupClickListeners() {
        sendButton.setOnClickListener {
            performSendEmail()
        }
        
        backButton.setOnClickListener {
            onBackPressed()
        }
    }
    
    private fun performSendEmail() {
        val email = emailEditText.text.toString().trim()
        
        // Check if email is empty
        if (email.isEmpty()) {
            emailEditText.error = "Email is required"
            return
        }
        
        // Check if email format is valid (basic validation)
        if (!android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches()) {
            emailEditText.error = "Please enter a valid email address"
            return
        }
        
        // Temporary functionality - navigate to verify account screen
        Toast.makeText(this, "Verification code sent to $email", Toast.LENGTH_SHORT).show()
        
        val intent = Intent(this, VerifyAccountActivity::class.java)
        intent.putExtra("email", email)
        startActivity(intent)
    }
}


