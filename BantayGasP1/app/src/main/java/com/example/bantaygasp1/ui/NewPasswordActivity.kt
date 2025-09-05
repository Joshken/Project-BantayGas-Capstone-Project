package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class NewPasswordActivity : AppCompatActivity() {
    
    private lateinit var newPasswordEditText: EditText
    private lateinit var confirmPasswordEditText: EditText
    private lateinit var newPasswordToggleButton: ImageButton
    private lateinit var confirmPasswordToggleButton: ImageButton
    private lateinit var updateButton: Button
    private lateinit var backButton: Button
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_new_password)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
    }
    
    private fun initializeViews() {
        newPasswordEditText = findViewById(R.id.newPasswordEditText)
        confirmPasswordEditText = findViewById(R.id.confirmPasswordEditText)
        newPasswordToggleButton = findViewById(R.id.newPasswordToggleButton)
        confirmPasswordToggleButton = findViewById(R.id.confirmPasswordToggleButton)
        updateButton = findViewById(R.id.updateButton)
        backButton = findViewById(R.id.backButton)
    }
    
    private fun setupClickListeners() {
        updateButton.setOnClickListener {
            performPasswordUpdate()
        }
        
        backButton.setOnClickListener {
            onBackPressed()
        }
        
        newPasswordToggleButton.setOnClickListener {
            togglePasswordVisibility(newPasswordEditText, newPasswordToggleButton)
        }
        
        confirmPasswordToggleButton.setOnClickListener {
            togglePasswordVisibility(confirmPasswordEditText, confirmPasswordToggleButton)
        }
    }
    
    private fun performPasswordUpdate() {
        val newPassword = newPasswordEditText.text.toString()
        val confirmPassword = confirmPasswordEditText.text.toString()
        
        // Check if fields are empty
        if (newPassword.isEmpty()) {
            newPasswordEditText.error = "New password is required"
            return
        }
        
        if (confirmPassword.isEmpty()) {
            confirmPasswordEditText.error = "Confirm password is required"
            return
        }
        
        // Check if passwords match
        if (newPassword != confirmPassword) {
            confirmPasswordEditText.error = "Passwords do not match"
            return
        }
        
        // Check password length
        if (newPassword.length < 6) {
            newPasswordEditText.error = "Password must be at least 6 characters"
            return
        }
        
        // Temporary password update functionality
        Toast.makeText(this, "Password updated successfully!", Toast.LENGTH_SHORT).show()
        
        // Navigate to password updated confirmation screen
        val intent = Intent(this, PasswordUpdatedActivity::class.java)
        startActivity(intent)
        finish() // Close this activity
    }
    
    private fun togglePasswordVisibility(editText: EditText, toggleButton: ImageButton) {
        if (editText.inputType == android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_PASSWORD) {
            // Show password
            editText.inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
            toggleButton.setImageResource(R.drawable.ic_visibility)
        } else {
            // Hide password
            editText.inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_PASSWORD
            toggleButton.setImageResource(R.drawable.ic_visibility_off)
        }
        // Move cursor to end of text
        editText.setSelection(editText.text.length)
    }
}


