package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.AutoCompleteTextView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.TextInputEditText
import com.google.android.material.textfield.TextInputLayout

class SignUpActivity : AppCompatActivity() {
    
    private lateinit var firstNameInput: TextInputEditText
    private lateinit var lastNameInput: TextInputEditText
    private lateinit var emailInput: TextInputEditText
    private lateinit var contactNumberInput: TextInputEditText
    private lateinit var addressInput: TextInputEditText
    private lateinit var roleSpinner: AutoCompleteTextView
    private lateinit var passwordInput: TextInputEditText
    private lateinit var confirmPasswordInput: TextInputEditText
    private lateinit var signUpButton: MaterialButton
    private lateinit var signInTextView: TextView
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_sign_up)
        supportActionBar?.hide()
        
        initializeViews()
        setupClickListeners()
    }
    
    private fun initializeViews() {
        firstNameInput = findViewById(R.id.firstNameInput)
        lastNameInput = findViewById(R.id.lastNameInput)
        emailInput = findViewById(R.id.emailInput)
        contactNumberInput = findViewById(R.id.contactNumberInput)
        addressInput = findViewById(R.id.addressInput)
        roleSpinner = findViewById(R.id.roleSpinner)
        passwordInput = findViewById(R.id.passwordInput)
        confirmPasswordInput = findViewById(R.id.confirmPasswordInput)
        signUpButton = findViewById(R.id.signUpButton)
        signInTextView = findViewById(R.id.signInTextView)
        
        setupRoleSpinner()
    }
    
    private fun setupClickListeners() {
        signUpButton.setOnClickListener {
            performSignUp()
        }
        
        signInTextView.setOnClickListener {
            onBackPressed()
        }
    }
    
    private fun setupRoleSpinner() {
        val roles = arrayOf("USER", "ADMIN")
        val adapter = ArrayAdapter(this, android.R.layout.simple_dropdown_item_1line, roles)
        roleSpinner.setAdapter(adapter)
        roleSpinner.setText("USER", false) // Set default value
    }
    
    private fun performSignUp() {
        val firstName = firstNameInput.text.toString().trim()
        val lastName = lastNameInput.text.toString().trim()
        val email = emailInput.text.toString().trim()
        val contactNumber = contactNumberInput.text.toString().trim()
        val address = addressInput.text.toString().trim()
        val selectedRole = roleSpinner.text.toString()
        val password = passwordInput.text.toString()
        val confirmPassword = passwordInput.text.toString()
        
        // Validation
        if (firstName.isEmpty()) {
            firstNameInput.error = "First name is required"
            return
        }
        
        if (lastName.isEmpty()) {
            lastNameInput.error = "Last name is required"
            return
        }
        
        if (email.isEmpty()) {
            emailInput.error = "Email is required"
            return
        }
        
        if (!android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches()) {
            emailInput.error = "Please enter a valid email address"
            return
        }
        
        if (contactNumber.isEmpty()) {
            contactNumberInput.error = "Contact number is required"
            return
        }
        
        if (address.isEmpty()) {
            addressInput.error = "Address is required"
            return
        }
        
        if (selectedRole.isEmpty()) {
            roleSpinner.error = "Please select a role"
            return
        }
        
        if (password.isEmpty()) {
            passwordInput.error = "Password is required"
            return
        }
        
        if (password.length < 6) {
            passwordInput.error = "Password must be at least 6 characters"
            return
        }
        
        if (confirmPassword.isEmpty()) {
            confirmPasswordInput.error = "Confirm password is required"
            return
        }
        
        if (password != confirmPassword) {
            confirmPasswordInput.error = "Passwords do not match"
            return
        }
        
        // Clear any previous errors
        clearErrors()
        
        // Store user data in SharedPreferences
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
        val editor = sharedPrefs.edit()
        
        editor.putString("firstName", firstName)
        editor.putString("lastName", lastName)
        editor.putString("email", email)
        editor.putString("phone", contactNumber)
        editor.putString("address", address)
        editor.putString("role", selectedRole)
        editor.putString("department", "General") // Default department
        editor.putString("username", email) // Use email as username
        editor.putString("password", password)
        
        editor.apply()
        
        // Temporary sign up functionality
        Toast.makeText(this, "Account created successfully!", Toast.LENGTH_SHORT).show()
        
        // Navigate to welcome screen
        val intent = Intent(this, WelcomeActivity::class.java)
        intent.putExtra("fullName", "$firstName $lastName")
        startActivity(intent)
        finish() // Close sign up activity
    }
    
    private fun clearErrors() {
        firstNameInput.error = null
        lastNameInput.error = null
        emailInput.error = null
        contactNumberInput.error = null
        addressInput.error = null
        roleSpinner.error = null
        passwordInput.error = null
        confirmPasswordInput.error = null
    }
}
