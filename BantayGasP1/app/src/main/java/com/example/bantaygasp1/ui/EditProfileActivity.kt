package com.example.bantaygasp1.ui

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.FileProvider
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsControllerCompat
import com.yalantis.ucrop.UCrop
import android.graphics.Color
import com.example.bantaygasp1.R

class EditProfileActivity : AppCompatActivity() {

    private lateinit var profileImage: ImageView
    private lateinit var firstNameEditText: EditText
    private lateinit var lastNameEditText: EditText
    private lateinit var emailEditText: EditText
    private lateinit var phoneEditText: EditText
    private lateinit var addressEditText: EditText
    private lateinit var backButton: ImageView
    private lateinit var saveButton: TextView
    private lateinit var btnEditPhoto: View

    private var selectedImageUri: Uri? = null
    private var tempCameraUri: Uri? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_edit_profile_information)
        supportActionBar?.hide()

        // Make status bar area transparent and use dark icons for visibility on light bg
        WindowCompat.setDecorFitsSystemWindows(window, true)
        window.statusBarColor = Color.TRANSPARENT
        WindowInsetsControllerCompat(window, window.decorView).isAppearanceLightStatusBars = true

        initializeViews()
        setupClickListeners()
        loadUserData()
    }

    private fun initializeViews() {
        profileImage = findViewById(R.id.profileImage)
        firstNameEditText = findViewById(R.id.firstNameEditText)
        lastNameEditText = findViewById(R.id.lastNameEditText)
        emailEditText = findViewById(R.id.emailEditText)
        phoneEditText = findViewById(R.id.phoneEditText)
        addressEditText = findViewById(R.id.addressEditText)
        backButton = findViewById(R.id.backButton)
        saveButton = findViewById<TextView>(R.id.saveButton)
        btnEditPhoto = findViewById(R.id.btnEditPhoto)
    }

    private fun setupClickListeners() {
        backButton.setOnClickListener {
            Log.d("EditProfile", "Back button clicked")
            finish()
        }

        profileImage.setOnClickListener {
            Log.d("EditProfile", "Profile image clicked")
            showSourceChooser()
        }
        btnEditPhoto.setOnClickListener {
            Log.d("EditProfile", "Edit photo FAB clicked")
            showSourceChooser()
        }

        saveButton.setOnClickListener {
            Log.d("EditProfile", "Save button clicked")
            saveProfileChanges()
        }
    }

    private fun loadUserData() {
        // Load user data from SharedPreferences or database
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", MODE_PRIVATE)
        
        firstNameEditText.setText(sharedPrefs.getString("firstName", "John"))
        lastNameEditText.setText(sharedPrefs.getString("lastName", "Doe"))
        emailEditText.setText(sharedPrefs.getString("email", "john.doe@example.com"))
        phoneEditText.setText(sharedPrefs.getString("phone", "+1234567890"))
        addressEditText.setText(sharedPrefs.getString("address", "123 Main St, City, State"))
        
        // Load profile image if available
        val imageUriString = sharedPrefs.getString("profileImageUri", null)
        if (imageUriString != null) {
            selectedImageUri = Uri.parse(imageUriString)
            profileImage.setImageURI(selectedImageUri)
        }
    }

    private fun showImagePicker() {
        // Use the system picker; no storage permission required
        pickImageLauncher.launch("image/*")
    }

    private fun showSourceChooser() {
        val options = arrayOf("Take Photo", "Choose from Gallery")
        val builder = android.app.AlertDialog.Builder(this)
            .setTitle("Profile picture")
            .setItems(options) { _, which ->
                when (which) {
                    0 -> launchCamera()
                    1 -> showImagePicker()
                }
            }
        builder.show()
    }

    private fun launchCamera() {
        val imagesDir = java.io.File(cacheDir, "images")
        if (!imagesDir.exists()) imagesDir.mkdirs()
        val imageFile = java.io.File(imagesDir, "capture_${System.currentTimeMillis()}.jpg")
        tempCameraUri = FileProvider.getUriForFile(
            this,
            "${applicationContext.packageName}.fileprovider",
            imageFile
        )
        takePictureLauncher.launch(tempCameraUri)
    }

    private val pickImageLauncher = registerForActivityResult(
        ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        uri?.let { startCrop(it) }
    }

    private val takePictureLauncher = registerForActivityResult(
        ActivityResultContracts.TakePicture()
    ) { success: Boolean ->
        if (success && tempCameraUri != null) {
            startCrop(tempCameraUri!!)
        }
    }

    private fun saveProfileChanges() {
        val firstName = firstNameEditText.text.toString().trim()
        val lastName = lastNameEditText.text.toString().trim()
        val email = emailEditText.text.toString().trim()
        val phone = phoneEditText.text.toString().trim()
        val address = addressEditText.text.toString().trim()

        if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty()) {
            Toast.makeText(this, "Please fill in all required fields", Toast.LENGTH_SHORT).show()
            return
        }

        // Save to SharedPreferences
        val sharedPrefs = getSharedPreferences("BantayGasPrefs", MODE_PRIVATE)
        val editor = sharedPrefs.edit()
        
        editor.putString("firstName", firstName)
        editor.putString("lastName", lastName)
        editor.putString("email", email)
        editor.putString("phone", phone)
        editor.putString("address", address)
        
        selectedImageUri?.let { uri ->
            editor.putString("profileImageUri", uri.toString())
        }
        
        editor.apply()

        Toast.makeText(this, "Profile updated successfully!", Toast.LENGTH_SHORT).show()
        
        // Navigate back to ProfileInformationActivity
        val intent = Intent(this, ProfileInformationActivity::class.java)
        startActivity(intent)
        finish()
    }

    private fun startCrop(sourceUri: Uri) {
        val intent = Intent(this, AvatarCropActivity::class.java)
        intent.putExtra(AvatarCropActivity.EXTRA_SOURCE_URI, sourceUri)
        startActivityForResult(intent, REQ_CROP_AVATAR)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQ_CROP_AVATAR && resultCode == RESULT_OK && data != null) {
            val resultUri = data.getParcelableExtra<Uri>(AvatarCropActivity.EXTRA_RESULT_URI)
            if (resultUri != null) {
                selectedImageUri = resultUri
                profileImage.setImageURI(resultUri)
            }
        } else if (requestCode == REQ_CROP_AVATAR) {
            // user cancelled or error; nothing to do
        }
    }

    companion object {
        private const val REQ_CROP_AVATAR = 6001
    }

    // No runtime permissions needed for GetContent-based picker
}
