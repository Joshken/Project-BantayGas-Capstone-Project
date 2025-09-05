package com.example.bantaygasp1.ui

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.bantaygasp1.R

class ProfileInformationActivity : AppCompatActivity() {

    private lateinit var backButton: ImageView
    private lateinit var saveButton: TextView
    private lateinit var editButton: TextView
    private lateinit var profileImage: ImageView
    private lateinit var firstNameEditText: EditText
    private lateinit var lastNameEditText: EditText
    private lateinit var emailEditText: EditText
    private lateinit var phoneEditText: EditText
    private lateinit var addressEditText: EditText
    private lateinit var roleEditText: EditText
    private lateinit var departmentEditText: EditText

    private var isEditMode = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d("ProfileInformation", "onCreate started")
        
        try {
            setContentView(R.layout.activity_profile_information)
            Log.d("ProfileInformation", "setContentView successful")
            
            supportActionBar?.hide()
            Log.d("ProfileInformation", "supportActionBar hidden")

            initializeViews()
            Log.d("ProfileInformation", "Views initialized")
            
            setupClickListeners()
            Log.d("ProfileInformation", "Click listeners set up")
            
            loadProfileData()
            Log.d("ProfileInformation", "Profile data loaded")
            
            Log.d("ProfileInformation", "onCreate completed successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error in onCreate: ${e.message}", e)
            Toast.makeText(this, "Error loading profile information: ${e.message}", Toast.LENGTH_LONG).show()
            finish()
        }
    }

    override fun onResume() {
        super.onResume()
        try {
            Log.d("ProfileInformation", "onResume called")
            // Reload profile data in case it was updated in EditProfileInformationActivity
            loadProfileData()
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error in onResume: ${e.message}", e)
        }
    }

    override fun onPause() {
        super.onPause()
        try {
            Log.d("ProfileInformation", "onPause called")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error in onPause: ${e.message}", e)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            Log.d("ProfileInformation", "onDestroy called")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error in onDestroy: ${e.message}", e)
        }
    }

    private fun initializeViews() {
        try {
            Log.d("ProfileInformation", "Starting view initialization")
            
            backButton = findViewById(R.id.backButton)
            Log.d("ProfileInformation", "backButton found")
            
            saveButton = findViewById(R.id.saveButton)
            Log.d("ProfileInformation", "saveButton found")
            
            editButton = findViewById(R.id.editButton)
            Log.d("ProfileInformation", "editButton found")
            
            profileImage = findViewById(R.id.profileImage)
            Log.d("ProfileInformation", "profileImage found")
            
            firstNameEditText = findViewById(R.id.firstNameEditText)
            Log.d("ProfileInformation", "firstNameEditText found")
            
            lastNameEditText = findViewById(R.id.lastNameEditText)
            Log.d("ProfileInformation", "lastNameEditText found")
            
            emailEditText = findViewById(R.id.emailEditText)
            Log.d("ProfileInformation", "emailEditText found")
            
            phoneEditText = findViewById(R.id.phoneEditText)
            Log.d("ProfileInformation", "phoneEditText found")
            
            addressEditText = findViewById(R.id.addressEditText)
            Log.d("ProfileInformation", "addressEditText found")

            roleEditText = findViewById(R.id.roleEditText)
            Log.d("ProfileInformation", "roleEditText found")
            
            departmentEditText = findViewById(R.id.departmentEditText)
            Log.d("ProfileInformation", "departmentEditText found")
            
            Log.d("ProfileInformation", "All views initialized successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error initializing views: ${e.message}", e)
            throw e
        }
    }

    private fun setupClickListeners() {
        try {
            Log.d("ProfileInformation", "Setting up click listeners")
            
            backButton.setOnClickListener {
                Log.d("ProfileInformation", "Back button clicked")
                finish()
            }

            editButton.setOnClickListener {
                Log.d("ProfileInformation", "Edit button clicked")
                val intent = Intent(this, EditProfileInformationActivity::class.java)
                startActivity(intent)
            }

            saveButton.setOnClickListener {
                Log.d("ProfileInformation", "Save button clicked")
                saveProfileData()
            }

            profileImage.setOnClickListener {
                Log.d("ProfileInformation", "Profile image clicked")
                Toast.makeText(this, "Profile image picker coming soon!", Toast.LENGTH_SHORT).show()
            }
            
            Log.d("ProfileInformation", "Click listeners set up successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error setting up click listeners: ${e.message}", e)
        }
    }

    private fun loadProfileData() {
        try {
            Log.d("ProfileInformation", "Loading profile data")
            
            // Load profile data from SharedPreferences or other storage
            val sharedPrefs = getSharedPreferences("BantayGasPrefs", android.content.Context.MODE_PRIVATE)
            Log.d("ProfileInformation", "SharedPreferences accessed")
            
            firstNameEditText.setText(sharedPrefs.getString("firstName", "John"))
            lastNameEditText.setText(sharedPrefs.getString("lastName", "Doe"))
            emailEditText.setText(sharedPrefs.getString("email", "john.doe@example.com"))
            phoneEditText.setText(sharedPrefs.getString("phone", "+63 912 345 6789"))
            addressEditText.setText(sharedPrefs.getString("address", "123 Main St, City, Country"))
            roleEditText.setText(sharedPrefs.getString("role", "Staff"))
            departmentEditText.setText(sharedPrefs.getString("department", "IT Department"))
            
            Log.d("ProfileInformation", "Profile data set to EditTexts")
            
            // Initially disable editing
            setEditMode(false)
            Log.d("ProfileInformation", "Edit mode disabled")
            
            Log.d("ProfileInformation", "Profile data loaded successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error loading profile data: ${e.message}", e)
        }
    }

    private fun toggleEditMode() {
        try {
            Log.d("ProfileInformation", "Toggling edit mode")
            
            isEditMode = !isEditMode
            setEditMode(isEditMode)
            
            if (isEditMode) {
                editButton.text = "Cancel"
                saveButton.visibility = TextView.VISIBLE
                Log.d("ProfileInformation", "Edit mode enabled")
            } else {
                editButton.text = "Edit"
                saveButton.visibility = TextView.GONE
                // Reload original data
                loadProfileData()
                Log.d("ProfileInformation", "Edit mode disabled")
            }
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error toggling edit mode: ${e.message}", e)
        }
    }

    private fun setEditMode(enabled: Boolean) {
        try {
            Log.d("ProfileInformation", "Setting edit mode: $enabled")
            
            firstNameEditText.isEnabled = enabled
            lastNameEditText.isEnabled = enabled
            emailEditText.isEnabled = enabled
            phoneEditText.isEnabled = enabled
            addressEditText.isEnabled = enabled
            roleEditText.isEnabled = enabled
            departmentEditText.isEnabled = enabled
            
            if (enabled) {
                firstNameEditText.requestFocus()
            }
            
            Log.d("ProfileInformation", "Edit mode set successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error setting edit mode: ${e.message}", e)
        }
    }

    private fun saveProfileData() {
        try {
            Log.d("ProfileInformation", "Saving profile data")
            
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
            editor.putString("role", roleEditText.text.toString())
            editor.putString("department", departmentEditText.text.toString())
            
            editor.apply()

            // Show success message
            Toast.makeText(this, "Profile updated successfully!", Toast.LENGTH_SHORT).show()
            
            // Exit edit mode
            isEditMode = false
            setEditMode(false)
            editButton.text = "Edit"
            saveButton.visibility = TextView.GONE
            
            Log.d("ProfileInformation", "Profile data saved successfully")
        } catch (e: Exception) {
            Log.e("ProfileInformation", "Error saving profile data: ${e.message}", e)
            Toast.makeText(this, "Error saving profile data", Toast.LENGTH_SHORT).show()
        }
    }
}
