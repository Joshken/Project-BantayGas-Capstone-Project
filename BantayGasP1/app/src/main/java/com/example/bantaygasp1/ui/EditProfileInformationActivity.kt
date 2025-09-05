package com.example.bantaygasp1.ui

import android.os.Bundle
import android.util.Log
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class EditProfileInformationActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var saveButton: TextView
    private lateinit var profileImage: ImageView
    private lateinit var firstNameEditText: EditText
    private lateinit var lastNameEditText: EditText
    private lateinit var emailEditText: EditText
    private lateinit var phoneEditText: EditText
    private lateinit var addressEditText: EditText
    private lateinit var roleEditText: EditText
    private lateinit var departmentEditText: EditText

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d("EditProfileInformation", "onCreate started")
        
        try {
            setContentView(R.layout.activity_edit_profile_information)
            Log.d("EditProfileInformation", "setContentView successful")
            
            supportActionBar?.hide()
            Log.d("EditProfileInformation", "supportActionBar hidden")

            initializeViews()
            Log.d("EditProfileInformation", "Views initialized")
            
            setupClickListeners()
            Log.d("EditProfileInformation", "Click listeners set up")
            
            loadProfileData()
            Log.d("EditProfileInformation", "Profile data loaded")
            
            Log.d("EditProfileInformation", "onCreate completed successfully")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error in onCreate: ${e.message}", e)
            Toast.makeText(this, "Error loading edit profile: ${e.message}", Toast.LENGTH_LONG).show()
            finish()
        }
    }

    override fun onResume() {
        super.onResume()
        try {
            Log.d("EditProfileInformation", "onResume called")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error in onResume: ${e.message}", e)
        }
    }

    override fun onPause() {
        super.onPause()
        try {
            Log.d("EditProfileInformation", "onPause called")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error in onPause: ${e.message}", e)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            Log.d("EditProfileInformation", "onDestroy called")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error in onDestroy: ${e.message}", e)
        }
    }

    private fun initializeViews() {
        try {
            Log.d("EditProfileInformation", "Starting view initialization")
            
            backButton = findViewById(R.id.backButton)
            Log.d("EditProfileInformation", "backButton found")
            
            saveButton = findViewById(R.id.saveButton)
            Log.d("EditProfileInformation", "saveButton found")
            
            profileImage = findViewById(R.id.profileImage)
            Log.d("EditProfileInformation", "profileImage found")
            
            firstNameEditText = findViewById(R.id.firstNameEditText)
            Log.d("EditProfileInformation", "firstNameEditText found")
            
            lastNameEditText = findViewById(R.id.lastNameEditText)
            Log.d("EditProfileInformation", "lastNameEditText found")
            
            emailEditText = findViewById(R.id.emailEditText)
            Log.d("EditProfileInformation", "emailEditText found")
            
            phoneEditText = findViewById(R.id.phoneEditText)
            Log.d("EditProfileInformation", "phoneEditText found")
            
            addressEditText = findViewById(R.id.addressEditText)
            Log.d("EditProfileInformation", "addressEditText found")
            
            roleEditText = findViewById(R.id.roleEditText)
            Log.d("EditProfileInformation", "roleEditText found")
            
            departmentEditText = findViewById(R.id.departmentEditText)
            Log.d("EditProfileInformation", "departmentEditText found")
            
            Log.d("EditProfileInformation", "All views initialized successfully")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error initializing views: ${e.message}", e)
            throw e
        }
    }

    private fun setupClickListeners() {
        try {
            Log.d("EditProfileInformation", "Setting up click listeners")
            
            backButton.setOnClickListener {
                Log.d("EditProfileInformation", "Back button clicked")
                finish()
            }

            saveButton.setOnClickListener {
                Log.d("EditProfileInformation", "Save button clicked")
                saveProfileData()
            }

            profileImage.setOnClickListener {
                Log.d("EditProfileInformation", "Profile image clicked")
                Toast.makeText(this, "Profile image picker coming soon!", Toast.LENGTH_SHORT).show()
            }
            
            Log.d("EditProfileInformation", "Click listeners set up successfully")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error setting up click listeners: ${e.message}", e)
        }
    }

    private fun loadProfileData() {
        try {
            Log.d("EditProfileInformation", "Loading profile data")
            
            // Load profile data from SharedPreferences
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
            Log.d("EditProfileInformation", "SharedPreferences accessed")
            
            firstNameEditText.setText(sharedPrefs.getString("firstName", ""))
            lastNameEditText.setText(sharedPrefs.getString("lastName", ""))
            emailEditText.setText(sharedPrefs.getString("email", ""))
            phoneEditText.setText(sharedPrefs.getString("phone", ""))
            addressEditText.setText(sharedPrefs.getString("address", ""))
            roleEditText.setText(sharedPrefs.getString("role", ""))
            departmentEditText.setText(sharedPrefs.getString("department", ""))
            
            Log.d("EditProfileInformation", "Profile data set to EditTexts")
            
            // All fields are enabled for editing by default
            setEditMode(true)
            Log.d("EditProfileInformation", "Edit mode enabled")
            
            Log.d("EditProfileInformation", "Profile data loaded successfully")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error loading profile data: ${e.message}", e)
        }
    }

    private fun setEditMode(enabled: Boolean) {
        try {
            Log.d("EditProfileInformation", "Setting edit mode: $enabled")
            
            firstNameEditText.isEnabled = enabled
            lastNameEditText.isEnabled = enabled
            emailEditText.isEnabled = enabled
            phoneEditText.isEnabled = enabled
            addressEditText.isEnabled = enabled
            roleEditText.isEnabled = false // Role cannot be edited
            departmentEditText.isEnabled = enabled
            
            if (enabled) {
                firstNameEditText.requestFocus()
            }
            
            Log.d("EditProfileInformation", "Edit mode set successfully")
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error setting edit mode: ${e.message}", e)
        }
    }

    private fun saveProfileData() {
        try {
            Log.d("EditProfileInformation", "Saving profile data")
            
            // Validate input
            if (firstNameEditText.text.isNullOrBlank() || lastNameEditText.text.isNullOrBlank()) {
                Toast.makeText(this, "First name and last name are required", Toast.LENGTH_SHORT).show()
                return
            }

            if (emailEditText.text.isNullOrBlank() || !android.util.Patterns.EMAIL_ADDRESS.matcher(emailEditText.text.toString()).matches()) {
                Toast.makeText(this, "Please enter a valid email address", Toast.LENGTH_SHORT).show()
                return
            }

            // Save to SharedPreferences
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
            val editor = sharedPrefs.edit()
            
            editor.putString("firstName", firstNameEditText.text.toString())
            editor.putString("lastName", lastNameEditText.text.toString())
            editor.putString("email", emailEditText.text.toString())
            editor.putString("phone", phoneEditText.text.toString())
            editor.putString("address", addressEditText.text.toString())
            // Role is not editable, so we don't update it
            editor.putString("department", departmentEditText.text.toString())
            
            // Update username if email changed
            val oldEmail = sharedPrefs.getString("email", "")
            if (oldEmail != emailEditText.text.toString()) {
                editor.putString("username", emailEditText.text.toString())
            }
            
            editor.apply()

            // Show success message
            Toast.makeText(this, "Profile updated successfully!", Toast.LENGTH_SHORT).show()
            
            Log.d("EditProfileInformation", "Profile data saved successfully")
            
            // Return to previous screen
            finish()
        } catch (e: Exception) {
            Log.e("EditProfileInformation", "Error saving profile data: ${e.message}", e)
            Toast.makeText(this, "Error saving profile data", Toast.LENGTH_SHORT).show()
        }
    }
}
